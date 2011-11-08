#include <errno.h>
#include <fcntl.h>
#include <utime.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <linux/limits.h>

#include <QFileInfo>

#include "overwriters.hpp"
#include "freespaceeraser.hpp"

#define ALIGN_BOUND 512

#define MAX_TRIAL_NUMBER 5

#define REISERFS_SUPER_MAGIC 0x52654973

FreeSpaceEraser::FreeSpaceEraser(QObject *parent)
    : QThread(parent)
{
    randSource = fopen(URANDOM_FILE, "r");

    overwriter = 0;

    blockWrite = (char *) memalign(ALIGN_BOUND, ERASE_BLOCK_SIZE);
    blockCheck = (char *) memalign(ALIGN_BOUND, ERASE_BLOCK_SIZE);
}

FreeSpaceEraser::~FreeSpaceEraser()
{
    stopThread();
    wait();

    fclose(randSource);

    cleanOverwriter();

    free(blockWrite);
    free(blockCheck);
}

bool FreeSpaceEraser::cleanOverwriter(Overwriter *newOverwriter)
{
    if (overwriter) {
        delete overwriter;
    }

    overwriter = newOverwriter;

    return overwriter;
}

void FreeSpaceEraser::stopThread()
{
    stopped = true;
}

QString FreeSpaceEraser::getRounds()
{
    return QString::number(overwriter->getRounds());
}

QString FreeSpaceEraser::getLastError()
{
    return errorString;
}

QString FreeSpaceEraser::getFinishString()
{
    return finishString;
}

QString FreeSpaceEraser::fromDouble(double num)
{
    return QString::number(num, 'f', 2);
}

QString FreeSpaceEraser::fileTypeStr(bool isDir)
{
    return isDir ? tr("directory") : tr("file");
}

void FreeSpaceEraser::genRandName(char *randName)
{
    for (int i = startPos; i < startPos + NAME_MAX; ++i) {
        do {
            randName[i] = getc(randSource);
        }
        while (randName[i] == '/' || randName[i] == '\0');
    }
}

void FreeSpaceEraser::init(bool doInodes, bool doBlocks, const QString &mountPath, int owrType)
{
    errorString.clear();

    QFileInfo mountInfo(mountPath);
    if (!mountInfo.isDir() || !mountInfo.isReadable() || !mountInfo.isWritable() || !mountInfo.isExecutable()) {
        errorString = tr("Mount path is not a valid directory!");
        return;
    }

    if (doBlocks && !cleanOverwriter(intToOwr(owrType))) {
        errorString = tr("Unsupported overwriter type!");
        return;
    }

    if (!doBlocks && !doInodes) {
        errorString = tr("No operation requested!");
        return;
    }

    tmpDirPath = QFile::encodeName(mountPath);

    if (doInodes) {
        haveInodes = inodesSupported();
        if (!errorString.isEmpty()) {
            return;
        }

        if (haveInodes) {
            freeInodeSpace = getFreeInodes();
            if (!errorString.isEmpty()) {
                return;
            }
        }
    }

    if (doBlocks) {
        const struct rlimit resLimits = {RLIM_INFINITY, RLIM_INFINITY};

        if (setrlimit(RLIMIT_FSIZE, &resLimits)) {
            errorString = tr("Can't change maximum file size limit!");
            return;
        }
    }

    tmpDirPath += '/';
    startPos = tmpDirPath.size();
    tmpDirPath.resize(startPos + NAME_MAX);

    forever {
        genRandName(tmpDirPath.data());
        if (mkdir(tmpDirPath, S_IRWXU) != 0) {
            if (errno != EEXIST) {
                errorString = tr("Can't create temporary directory!");
                return;
            }
        }
        else {
            break;
        }
    }

    filesCount = 1;

    tmpFilePath = tmpDirPath + '/';
    startPos = tmpFilePath.size();
    tmpFilePath.resize(startPos + NAME_MAX);

    stopped = false;

    this->doInodes = doInodes;
    this->doBlocks = doBlocks;

    finishString = tr("<b><u>Erasing statistics:</u></b><br><br>");
}

bool FreeSpaceEraser::tmpDirExists()
{
    return filesCount > 0;
}

void FreeSpaceEraser::openBigFile()
{
    if (filesCount == 1) {
        genRandName(tmpFilePath.data());

        bigFileDesc = open(tmpFilePath, O_CREAT | O_RDWR | O_SYNC | O_DIRECT, S_IRWXU);
        if (bigFileDesc == -1) {
            errorString = tr("Can't create big file for erasing!");
            return;
        }

        ++filesCount;
    }
    else {
        while (struct dirent *tmpDirEntry = readdir(tmpDirPtr)) {
            if (strcmp(tmpDirEntry->d_name, ".") == 0 || strcmp(tmpDirEntry->d_name, "..") == 0) {
                continue;
            }

            tmpFilePath.replace(startPos, NAME_MAX, tmpDirEntry->d_name, NAME_MAX);

            bigFileDesc = open(tmpFilePath, O_RDWR | O_SYNC | O_DIRECT);
            if (bigFileDesc != -1) {
                return;
            }
        }

        errorString = tr("Can't use any file in temporary directory for erasing!");
    }
}

bool FreeSpaceEraser::needCheck(int round)
{
    return overwriter->verificationType() == Overwriter::AllRouns ||
           (overwriter->verificationType() == Overwriter::LastRound &&
            round == overwriter->getRounds() - 1);
}

qint64 FreeSpaceEraser::getFreeBlocks(const QByteArray &mountPoint)
{
    struct statfs fsStat;
    if (statfs(mountPoint, &fsStat)) {
        return -1;
    }

    return fsStat.f_bfree * fsStat.f_bsize;
}

qint64 FreeSpaceEraser::getFreeInodes(const QByteArray &mountPoint)
{
    struct statfs fsStat;
    if (statfs(mountPoint, &fsStat)) {
        return -1;
    }

    return fsStat.f_ffree;
}

qint64 FreeSpaceEraser::getFreeBlocks()
{
    qint64 ret = getFreeBlocks(tmpDirPath);

    if (ret < 0) {
        errorString = tr("Can't get information about file system!");
    }

    return ret;
}

qint64 FreeSpaceEraser::getFreeInodes()
{
    qint64 ret = getFreeInodes(tmpDirPath);

    if (ret < 0) {
        errorString = tr("Can't get information about file system!");
    }

    return ret;
}

bool FreeSpaceEraser::inodesSupported()
{
    struct statfs fsStat;
    if (statfs(tmpDirPath, &fsStat)) {
        errorString = tr("Can't get information about file system!");
        return false;
    }

    return fsStat.f_type != REISERFS_SUPER_MAGIC;
}

bool FreeSpaceEraser::performInodeStep()
{
    for (int trial = 0; ; ++trial) {
        if (trial == MAX_TRIAL_NUMBER || stopped) {
            return false;
        }

        int emptyFile;
        do {
            genRandName(tmpFilePath.data());

            emptyFile = open(tmpFilePath, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);
        }
        while (emptyFile == -1 && errno == EEXIST);

        if (emptyFile == -1) {
            sync();

            if (haveInodes) {
                totalCount = getFreeInodes(tmpDirPath);
                if (totalCount <= 0) {
                    return false;
                }
            }

            continue;
        }

        if (close(emptyFile) != 0) {
            errorString = tr("Can't close empty file!");
            return false;
        }

        ++filesCount;

        if (!stopped) {
            break;
        }
    }

    if (progressClock.elapsed() < PROGRESS_UPDATE_FREQ_MS) {
        return true;
    }

    if (haveInodes) {
        totalCount = getFreeInodes();
        if (totalCount <= 0) {
            return false;
        }
    }

    int timeElapsed = totalClock.elapsed();
    if (timeElapsed <= 0) {
        timeElapsed = 1;
    }

    double speed;
    if (haveInodes) {
        speed = freeInodeSpace;
        speed -= totalCount;
    }
    else {
        speed = filesCount;
    }
    speed *= 1000;
    speed /= timeElapsed;
    if (speed < 0) {
        speed = 0;
    }

    double timeLeft = -1;
    if (haveInodes) {
        timeLeft = totalCount;

        if (speed != 0) {
            timeLeft /= speed;
        }
        if (timeLeft < 0 || speed == 0) {
            timeLeft = 0;
        }
    }

    emit updateStat(speed, timeLeft, -1);

    if (haveInodes) {
        double erasedPart = freeInodeSpace;
        erasedPart -= totalCount;
        erasedPart /= freeInodeSpace;
        erasedPart *= 100;

        if (erasedPart < 0) {
            erasedPart = 0;
        }

        int newProgress = (int) erasedPart;
        if (newProgress >= 100) {
            newProgress = 99;
        }

        if (newProgress > progress && progress <= 99) {
            progress = newProgress;
            emit updateProgress(progress);
        }
    }

    progressClock.start();

    return true;
}

bool FreeSpaceEraser::performBlockStep(int round)
{
    for (int trial = 0; ; ++trial) {
        if (trial == MAX_TRIAL_NUMBER || stopped) {
            return false;
        }

        ssize_t numBytes = write(bigFileDesc, blockWrite, ERASE_BLOCK_SIZE);

        if (numBytes <= 0) {
            sync();

            totalCount = getFreeBlocks();
            if (totalCount <= 0) {
                return false;
            }
            else {
                qint64 blockSize = totalCount > ERASE_BLOCK_SIZE ? ERASE_BLOCK_SIZE : totalCount;
                qint64 writeSize = blockSize;

                numBytes = 0;

                while (numBytes < blockSize && !stopped) {
                    ssize_t chunkSize = write(bigFileDesc, blockWrite + numBytes, writeSize);
                    if (chunkSize <= 0) {
                        if (writeSize <= ALIGN_BOUND) {
                            break;
                        }

                        writeSize /= 2;
                        writeSize -= writeSize % ALIGN_BOUND;
                        writeSize = writeSize > ALIGN_BOUND ? writeSize : ALIGN_BOUND;
                    }
                    else {
                        numBytes += chunkSize;
                    }
                }

                if (numBytes == 0) {
                    continue;
                }
            }
        }

        if (needCheck(round)) {
            if (lseek(bigFileDesc, -numBytes, SEEK_CUR) == -1) {
                errorString = tr("Can't change big file offset for verification!");
                return false;
            }
            if (read(bigFileDesc, blockCheck, numBytes) != numBytes || memcmp(blockWrite, blockCheck, numBytes) != 0) {
                errorString = tr("Error in verification of written data!");
                return false;
            }
        }

        if (!stopped) {
            break;
        }
    }

    if (progressClock.elapsed() < PROGRESS_UPDATE_FREQ_MS) {
        return true;
    }

    totalCount = getFreeBlocks();
    if (totalCount <= 0) {
        return false;
    }

    int timeElapsed = roundClock.elapsed();
    if (timeElapsed <= 0) {
        timeElapsed = 1;
    }

    double speed = freeBlockSpace;
    speed -= totalCount;
    speed *= 1000;
    speed /= 1024;
    speed /= timeElapsed;
    if (speed < 0) {
        speed = 0;
    }

    double timeLeft = totalCount;
    timeLeft /= 1024;

    if (speed != 0) {
        timeLeft /= speed;
    }
    if (timeLeft < 0 || speed == 0) {
        timeLeft = 0;
    }

    emit updateStat(speed, timeLeft, round);

    double erasedPart = freeBlockSpace;
    erasedPart -= totalCount;
    erasedPart /= freeBlockSpace;
    erasedPart += round;
    erasedPart /= overwriter->getRounds();
    erasedPart *= 100;

    if (erasedPart < 0) {
        erasedPart = 0;
    }

    int newProgress = (int) erasedPart;
    if (newProgress >= 100) {
        newProgress = 99;
    }

    if (newProgress > progress && progress <= 99) {
        progress = newProgress;
        emit updateProgress(progress);
    }

    progressClock.start();

    return true;
}

void FreeSpaceEraser::performCleanStep()
{
    ++totalCount;

    if (progressClock.elapsed() < PROGRESS_UPDATE_FREQ_MS) {
        return;
    }

    int timeElapsed = totalClock.elapsed();
    if (timeElapsed <= 0) {
        timeElapsed = 1;
    }

    double speed = totalCount;
    speed *= 1000;
    speed /= timeElapsed;
    if (speed < 0) {
        speed = 0;
    }

    double timeLeft = filesCount;
    timeLeft -= totalCount;

    if (speed != 0) {
        timeLeft /= speed;
    }
    if (timeLeft < 0 || speed == 0) {
        timeLeft = 0;
    }

    emit updateStat(speed, timeLeft, -1);

    double erasedPart = totalCount;
    erasedPart /= filesCount;
    erasedPart *= 100;

    int newProgress = (int) erasedPart;
    if (newProgress >= 100) {
        newProgress = 99;
    }

    if (newProgress > progress && progress <= 99) {
        progress = newProgress;
        emit updateProgress(progress);
    }

    progressClock.start();
}

void FreeSpaceEraser::removeTmpFile(const QByteArray &tmpFile, bool isDir)
{
    if (chown(tmpFile, 0, 0) != 0) {
        errorString = tr("Can't zeroize owner of temporary %1!").arg(fileTypeStr(isDir));
        return;
    }

    if (chmod(tmpFile, 0) != 0) {
        errorString = tr("Can't zeroize permissions of temporary %1!").arg(fileTypeStr(isDir));
        return;
    }

    const struct utimbuf zeroTime = {0, 0};
    if (utime(tmpFile, &zeroTime) != 0) {
        errorString = tr("Can't zeroize access and modification times of temporary %1!").arg(fileTypeStr(isDir));
        return;
    }

    if (!isDir) {
        if (unlink(tmpFile) != 0) {
            errorString = tr("Can't unlink temporary %1!").arg(fileTypeStr(isDir));
            return;
        }
    }
    else {
        if (rmdir(tmpFile) != 0) {
            errorString = tr("Can't remove temporary %1!").arg(fileTypeStr(isDir));
            return;
        }
    }
}

void FreeSpaceEraser::run()
{
    tmpDirPtr = opendir(tmpDirPath);
    if (tmpDirPtr == 0) {
        errorString = tr("Can't open temporary directory!");
        stopped = true;
    }

    if (doInodes && !stopped) {
        progress = 0;

        if (haveInodes) {
            emit updateType(0);
        }
        else {
            emit updateType(1);
        }

        totalClock.start();
        progressClock.start();

        while (!stopped) {
            if (!performInodeStep()) {
                if (!errorString.isEmpty()) {
                    stopped = true;
                }

                break;
            }
        }

        sync();

        if (!stopped) {
            if (haveInodes) {
                totalCount = freeInodeSpace - totalCount;
            }
            else {
                totalCount = filesCount;
            }

            double timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }
            timeElapsed /= 1000;

            double avgSpeed = totalCount;
            avgSpeed /= timeElapsed;

            finishString += tr("<u>Inode space:</u><br>"
                               "Average speed: <b>%1 inodes/s</b><br>"
                               "Space erased: <b>%2 inodes</b><br>"
                               "Time elapsed: <b>%3 seconds</b><br><br>").arg(fromDouble(avgSpeed), QString::number(totalCount), fromDouble(timeElapsed));

            emit finishErasePart();
        }
    }

    if (doBlocks && !stopped) {
        emit updateType(2);

        progress = 0;

        openBigFile();
        if (!errorString.isEmpty()) {
            stopped = true;
        }
        else {
            totalClock.start();
        }

        for (int round = 0; round < overwriter->getRounds() && !stopped; ++round) {
            freeBlockSpace = getFreeBlocks();
            if (!errorString.isEmpty()) {
                stopped = true;
                break;
            }

            if (lseek(bigFileDesc, 0, SEEK_SET) == -1) {
                errorString = tr("Can't change big file offset for new round!");
                stopped = true;
                break;
            }

            roundClock.start();
            progressClock.start();

            if (overwriter->isConstantRound(round)) {
                overwriter->fillBlock(blockWrite, ERASE_BLOCK_SIZE, round);

                while (!stopped) {
                    if (!performBlockStep(round)) {
                        if (!errorString.isEmpty()) {
                            stopped = true;
                        }

                        break;
                    }
                }
            }
            else {
                while (!stopped) {
                    overwriter->fillBlock(blockWrite, ERASE_BLOCK_SIZE, round);

                    if (!performBlockStep(round)) {
                        if (!errorString.isEmpty()) {
                            stopped = true;
                        }

                        break;
                    }
                }
            }

            if (fdatasync(bigFileDesc) != 0) {
                errorString = tr("Can't transfer written data to the storage device!");
                stopped = true;
                break;
            }

            if (ftruncate(bigFileDesc, 0) != 0) {
                errorString = tr("Can't truncate big file!");
                stopped = true;
                break;
            }
        }

        if (bigFileDesc != -1) {
            if (close(bigFileDesc) != 0) {
                errorString = tr("Can't close big file!");
                stopped = true;
            }
        }

        sync();

        if (!stopped) {
            double bytesErased = freeBlockSpace;
            bytesErased -= totalCount;
            bytesErased /= 1024;

            double timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }
            timeElapsed /= 1000;

            double avgSpeed = bytesErased;
            avgSpeed /= timeElapsed;

            bytesErased /= 1024;

            finishString += tr("<u>Block space:</u><br>"
                               "Average speed: <b>%1 KiB/s</b><br>"
                               "Space erased: <b>%2 MiB</b><br>"
                               "Time elapsed: <b>%3 seconds</b><br><br>").arg(fromDouble(avgSpeed), fromDouble(bytesErased), fromDouble(timeElapsed));

            emit finishErasePart();
        }
    }

    if (errorString.isEmpty()) {
        emit updateType(3);
    }
    else {
        emit updateType(4);
    }

    progress = 0;
    totalCount = 0;

    if (tmpDirPtr != 0) {
        rewinddir(tmpDirPtr);

        if (!stopped) {
            totalClock.start();
        }

        progressClock.start();

        while (struct dirent *tmpDirEntry = readdir(tmpDirPtr)) {
            if (strcmp(tmpDirEntry->d_name, ".") == 0 || strcmp(tmpDirEntry->d_name, "..") == 0) {
                continue;
            }

            tmpFilePath.replace(startPos, NAME_MAX, tmpDirEntry->d_name, NAME_MAX);

            removeTmpFile(tmpFilePath, false);
            if (!errorString.isEmpty()) {
                return;
            }

            performCleanStep();
        }

        if (closedir(tmpDirPtr) != 0) {
            errorString = tr("Can't close temporary directory!");
            return;
        }
    }
    else {
        progressClock.start();
    }

    removeTmpFile(tmpDirPath, true);
    if (!errorString.isEmpty()) {
        return;
    }

    performCleanStep();

    filesCount = 0;

    sync();

    if (!stopped) {
        double timeElapsed = totalClock.elapsed();
        if (timeElapsed <= 0) {
            timeElapsed = 1;
        }
        timeElapsed /= 1000;

        double avgSpeed = totalCount;
        avgSpeed /= timeElapsed;

        finishString += tr("<u>Deleting files:</u><br>"
                           "Average speed: <b>%1 files/s</b><br>"
                           "Files deleted: <b>%2 files</b><br>"
                           "Time elapsed: <b>%3 seconds</b><br><br>").arg(fromDouble(avgSpeed), QString::number(totalCount), fromDouble(timeElapsed));
    }
}
