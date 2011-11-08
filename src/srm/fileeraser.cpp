#include <ftw.h>
#include <fcntl.h>
#include <dirent.h>
#include <malloc.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/limits.h>

#include <QDir>
#include <QFileInfo>
#include <QFileIconProvider>

#include "fileeraser.hpp"
#include "overwriters.hpp"

#define ALIGN_BOUND 512

static qint64 totalSize;

static qint64 getFileSize(int fileDesc, const struct stat *fileStat)
{
    qint64 fileSize = 0;

    if (S_ISBLK(fileStat->st_mode)) {
        __u64 devSize;
        if (ioctl(fileDesc, BLKGETSIZE64, &devSize) != 0) {
            fileSize = -1;
        }
        else {
            fileSize = devSize;
        }
    }
    else {
        fileSize = fileStat->st_size;

        if (fileStat->st_blksize > 0) {
            if (qint64 blockRem = fileSize % fileStat->st_blksize) {
                fileSize += fileStat->st_blksize - blockRem;
            }
        }
        else {
            fileSize = -1;
        }
    }

    return fileSize;
}

static int processFile(const char *filePath, const struct stat *fileStat, int /* unused */, struct FTW * /* unused */)
{
    int ret = 0;

    if (S_ISDIR(fileStat->st_mode)) {
        if (access(filePath, R_OK | W_OK | X_OK) != 0) {
            ret = 1;
        }
    }
    else {
        if (faccessat(AT_FDCWD, filePath, R_OK | W_OK, AT_SYMLINK_NOFOLLOW) != 0) {
            ret = 1;
        }
        else {
            if (S_ISBLK(fileStat->st_mode) || S_ISREG(fileStat->st_mode)) {
                int fileDesc = open(filePath, O_RDONLY);
                if (fileDesc != -1) {
                    qint64 fileSize = getFileSize(fileDesc, fileStat);
                    if (fileSize < 0) {
                        ret = 2;
                    }
                    else {
                        totalSize += fileSize;
                    }

                    if (close(fileDesc) != 0) {
                        ret = 3;
                    }
                }
                else {
                    ret = 4;
                }
            }
        }
    }

    return ret;
}

FileEraser::FileEraser(QObject *parent)
    : QThread(parent)
{
    randSource = fopen(URANDOM_FILE, "r");

    overwriter = 0;

    blockWrite = (char *) memalign(ALIGN_BOUND, ERASE_BLOCK_SIZE);
    blockCheck = (char *) memalign(ALIGN_BOUND, ERASE_BLOCK_SIZE);
}

FileEraser::~FileEraser()
{
    stopThread();
    wait();

    fclose(randSource);

    cleanOverwriter();

    free(blockWrite);
    free(blockCheck);
}

bool FileEraser::cleanOverwriter(Overwriter *newOverwriter)
{
    if (overwriter) {
        delete overwriter;
    }

    overwriter = newOverwriter;

    return overwriter;
}

void FileEraser::stopThread()
{
    stopped = true;
}

void FileEraser::genRandName(int startPos, char *randName)
{
    for (int i = startPos; i < startPos + NAME_MAX; ++i) {
        do {
            randName[i] = getc(randSource);
        }
        while (randName[i] == '/' || randName[i] == '\0');
    }
}

int FileEraser::getRounds()
{
    return overwriter->getRounds();
}

QString FileEraser::getLastError()
{
    return errorString;
}

QString FileEraser::getFinishString()
{
    return finishString;
}

QString FileEraser::fromDouble(double num)
{
    return QString::number(num, 'f', 2);
}

int FileEraser::blockDevType(const QString &fileName)
{
    QString devName = QFileInfo(fileName).fileName();

    if (devName.startsWith("fd")) {
        return 0;
    }

    if (devName.startsWith("hd")) {
        return devName.at(devName.size() - 1).isDigit() ? 1 : 2;
    }

    if (devName.startsWith("dm")) {
        return 3;
    }

    if (devName.startsWith("sd")) {
        QStringList blockDevList = QDir("/sys/block").entryList();

        for (int i = 0; i < blockDevList.size(); ++i) {
            if (devName.startsWith(blockDevList.at(i))) {
                QFile typeFile(QString("/sys/block/%1/removable").arg(blockDevList.at(i)));
                if (typeFile.open(QFile::ReadOnly)) {
                    if (typeFile.read(1) == "1") {
                        return devName.at(devName.size() - 1).isDigit() ? 4 : 5;
                    }
                    else {
                        return devName.at(devName.size() - 1).isDigit() ? 6 : 7;
                    }
                }
                else {
                    break;
                }
            }
        }
    }

    return -1;
}

qint64 FileEraser::getFileSize(const QString &fileName)
{
    ::totalSize = 0;

    return nftw(QFile::encodeName(fileName), processFile, 1, FTW_PHYS) != 0 ? -1 : ::totalSize;
}

QString FileEraser::fileTypeStr(const QString &fileName)
{
    struct stat fileStat;

    if (lstat(QFile::encodeName(fileName), &fileStat) == 0) {
        switch (fileStat.st_mode & S_IFMT) {
            case S_IFBLK:
                switch (blockDevType(fileName)) {
                    case 0:
                        return tr("floppy disk");
                    case 1:
                        return tr("IDE hard disk partition");
                    case 2:
                        return tr("IDE hard disk");
                    case 3:
                        return tr("LVM partition");
                    case 4:
                        return tr("USB flash disk partition");
                    case 5:
                        return tr("USB flash disk");
                    case 6:
                        return tr("SATA hard disk partition");
                    case 7:
                        return tr("SATA hard disk");
                    default:
                        return tr("block device");
                }
            case S_IFCHR:
                return tr("character device");
            case S_IFDIR:
                return tr("directory");
            case S_IFIFO:
                return tr("named pipe");
            case S_IFLNK:
                return tr("symbolic link");
            case S_IFREG:
                return tr("regular file");
            case S_IFSOCK:
                return tr("socket");
        }
    }

    return QString();
}

QIcon FileEraser::fileTypeIcon(const QString &fileName)
{
    struct stat fileStat;

    if (lstat(QFile::encodeName(fileName), &fileStat) == 0) {
        switch (fileStat.st_mode & S_IFMT) {
            case S_IFBLK:
                switch (blockDevType(fileName)) {
                    case 0:
                        return QIcon::fromTheme("media-floppy");
                    case 4:
                    case 5:
                        return QIcon::fromTheme("media-flash");
                    default:
                        return QIcon::fromTheme("drive-harddisk");
                }
            case S_IFCHR:
                return QIcon(":/character_device.png");
            case S_IFDIR:
            case S_IFREG:
                return QFileIconProvider().icon(QFileInfo(fileName));
            case S_IFIFO:
                return QIcon(":/named_pipe.png");
            case S_IFLNK:
                return QIcon::fromTheme("emblem-symbolic-link");
            case S_IFSOCK:
                return QIcon(":/socket.png");
        }
    }

    return QIcon();
}

void FileEraser::fileError(const QString &errorPattern, const FileStruct &fileStruct)
{
    errorString = errorPattern.arg(fileTypeStr(fileStruct.fileInfo.fileName), fileStruct.fileInfo.fileName);
}

void FileEraser::init(int owrType, const FilesList &filesList)
{
    errorString.clear();

    if (!cleanOverwriter(intToOwr(owrType))) {
        errorString = tr("Unsupported overwriter type!");
        return;
    }

    if (filesList.isEmpty()) {
        errorString = tr("List of files is empty!");
        return;
    }

    this->filesList.clear();
    this->filesList.reserve(filesList.size());

    QByteArray fileName;
    FileStruct fileStruct;

    totalSize = 0;

    for (int i = 0; i < filesList.size(); ++i) {
        fileName = QFile::encodeName(filesList.at(i).fileName);
        if (lstat(fileName, &fileStruct.fileStat) != 0) {
            errorString = tr("Can't get information about file '%1'!").arg(filesList.at(i).fileName);
            return;
        }

        fileStruct.fileInfo = filesList.at(i);

        ::totalSize = 0;

        switch (nftw(fileName, processFile, 1, FTW_PHYS)) {
            case 0:
                break;
            case 1:
                fileError(tr("Can't access %1 '%2'!"), fileStruct);
                return;
            case 2:
                fileError(tr("Can't get size of %1 '%2'!"), fileStruct);
                return;
            case 3:
                fileError(tr("Can't close %1 '%2'!"), fileStruct);
                return;
            case 4:
                fileError(tr("Can't open %1 '%2'!"), fileStruct);
                return;
            default:
                fileError(tr("Can't process %1 '%2'!"), fileStruct);
                return;
        }

        totalSize += ::totalSize;

        this->filesList.append(fileStruct);
    }

    stopped = false;

    filesErased = 0;

    totalCount = 0;
    totalProgress = 0;
}

bool FileEraser::needCheck(int round)
{
    return overwriter->verificationType() == Overwriter::AllRouns ||
           (overwriter->verificationType() == Overwriter::LastRound &&
            round == overwriter->getRounds() - 1);
}

void FileEraser::performOwrStep(int fileDesc, int round, qint64 numBytes, const FileStruct &fileStruct)
{
    if (numBytes != write(fileDesc, blockWrite, numBytes)) {
        fileError(tr("Can't write all bytes to %1 '%2'!"), fileStruct);
        return;
    }

    if (needCheck(round)) {
        if (lseek(fileDesc, -numBytes, SEEK_CUR) == -1) {
            fileError(tr("Can't change %1 '%2' offset for verification!"), fileStruct);
            return;
        }
        if (read(fileDesc, blockCheck, numBytes) != numBytes || memcmp(blockWrite, blockCheck, numBytes) != 0) {
            fileError(tr("Error in verification of written data to %1 '%2'!"), fileStruct);
            return;
        }
    }

    fileCount += numBytes;
    totalCount += numBytes;

    if (progressClock.elapsed() < PROGRESS_UPDATE_FREQ_MS) {
        return;
    }

    int timeElapsed = roundClock.elapsed();
    if (timeElapsed <= 0) {
        timeElapsed = 1;
    }

    double speed = fileCount;
    speed *= 1000;
    speed /= 1024;
    speed /= timeElapsed;
    if (speed < 0) {
        speed = 0;
    }

    double timeLeft = fileSize;
    timeLeft -= fileCount;
    timeLeft /= 1024;

    if (speed != 0) {
        timeLeft /= speed;
    }
    if (timeLeft < 0 || speed == 0) {
        timeLeft = 0;
    }

    emit updateStat(speed, timeLeft, round);

    double filePart = fileCount;
    filePart /= fileSize;
    filePart += round;
    filePart /= overwriter->getRounds();
    filePart *= 100;

    if (filePart < 0) {
        filePart = 0;
    }

    int newFileProgress = (int) filePart;
    if (newFileProgress >= 100) {
        newFileProgress = 99;
    }

    if (newFileProgress > fileProgress && fileProgress <= 99) {
        fileProgress = newFileProgress;
        emit fileProgressSignal(fileProgress);
    }

    double totalPart = totalCount;
    totalPart /= totalSize;
    totalPart /= overwriter->getRounds();
    totalPart *= 100;

    if (totalPart < 0) {
        totalPart = 0;
    }

    int newTotalProgress = (int) totalPart;
    if (newTotalProgress >= 100) {
        newTotalProgress = 99;
    }

    if (newTotalProgress > totalProgress && totalProgress <= 99) {
        totalProgress = newTotalProgress;
        emit totalProgressSignal(totalProgress);
    }

    progressClock.start();
}

void FileEraser::eraseFileName(const FileStruct &fileStruct)
{
    if (fileStruct.fileInfo.deleteAfter) {
        emit erasingFile(fileStruct.fileInfo.fileName, 1, S_ISDIR(fileStruct.fileStat.st_mode));

        QString parentDirDec = QFileInfo(fileStruct.fileInfo.fileName).path();
        QByteArray parentDirEnc = QFile::encodeName(parentDirDec);

        DIR *parentDirPtr = opendir(parentDirEnc);
        if (parentDirPtr != 0) {
            int parentDirDesc = dirfd(parentDirPtr);
            if (parentDirDesc != -1) {
                QByteArray oldPath = QFile::encodeName(fileStruct.fileInfo.fileName);

                QByteArray newPath = parentDirEnc + '/';
                int startPos = newPath.size();
                newPath.resize(startPos + NAME_MAX);

                for (int i = 0; i < overwriter->getRounds() && !stopped; ++i) {
                    emit renamingRound(i);

                    do {
                        genRandName(startPos, newPath.data());
                    }
                    while (faccessat(AT_FDCWD, newPath, F_OK, AT_SYMLINK_NOFOLLOW) == 0);

                    if (rename(oldPath, newPath) != 0) {
                        errorString = tr("Can't rename '%1' to '%2'!").arg(QFile::decodeName(oldPath), QFile::decodeName(newPath));
                        break;
                    }

                    oldPath = newPath;

                    if (fdatasync(parentDirDesc) != 0) {
                        errorString = tr("Can't commit renaming in directory '%1'!").arg(parentDirDec);
                        break;
                    }
                }

                while (!stopped && errorString.isEmpty()) {
                    emit renamingRound(overwriter->getRounds());

                    FileStruct newStruct = fileStruct;
                    newStruct.fileInfo.fileName = QFile::decodeName(newPath);

                    if (lchown(newPath, 0, 0) != 0) {
                        fileError(tr("Can't zeroize owner of %1 '%2'!"), newStruct);
                        break;
                    }

                    if (S_ISREG(newStruct.fileStat.st_mode)) {
                        if (truncate(newPath, 0) != 0) {
                            fileError(tr("Can't truncate %1 '%2'!"), newStruct);
                            break;
                        }
                    }

                    if (!S_ISLNK(newStruct.fileStat.st_mode)) {
                        if (chmod(newPath, 0) != 0) {
                            fileError(tr("Can't zeroize permissions of %1 '%2'!"), newStruct);
                            break;
                        }
                    }

                    const struct timespec zeroTime[2] = {{0, 0}, {0, 0}};
                    if (utimensat(AT_FDCWD, newPath, zeroTime, AT_SYMLINK_NOFOLLOW) != 0) {
                        fileError(tr("Can't zeroize access and modification times of %1 '%2'!"), newStruct);
                        break;
                    }

                    if (!S_ISDIR(newStruct.fileStat.st_mode)) {
                        if (unlink(newPath) != 0) {
                            fileError(tr("Can't unlink %1 '%2'!"), newStruct);
                            break;
                        }
                    }
                    else {
                        if (rmdir(newPath) != 0) {
                            fileError(tr("Can't remove %1 '%2'!"), newStruct);
                            break;
                        }
                    }

                    if (fdatasync(parentDirDesc) != 0) {
                        errorString = tr("Can't commit deleting in directory '%1'!").arg(parentDirDec);
                    }

                    break;
                }
            }
            else {
                errorString = tr("Can't get descriptor of directory '%1'!").arg(parentDirDec);
            }

            if (closedir(parentDirPtr) != 0) {
                errorString = tr("Can't close directory '%1'!").arg(parentDirDec);
            }
        }
        else {
            errorString = tr("Can't open directory '%1'!").arg(parentDirDec);
        }
    }

    if (!stopped && errorString.isEmpty()) {
        emit finishErasePart();

        if (fileStruct.fileInfo.deleteAfter) {
            ++filesErased;
        }
    }
}

void FileEraser::eraseFileBytes(const FileStruct &fileStruct)
{
    int fileDesc = open(QFile::encodeName(fileStruct.fileInfo.fileName), O_RDWR | O_SYNC | O_DIRECT);
    if (fileDesc != -1) {
        fileSize = ::getFileSize(fileDesc, &fileStruct.fileStat);
        if (fileSize < 0) {
            fileError(tr("Can't get size of %1 '%2'!"), fileStruct);
        }
        else if (fileSize != 0) {
            emit erasingFile(fileStruct.fileInfo.fileName, 2, false);

            fileProgress = 0;

            for (int round = 0; round < overwriter->getRounds() && !stopped; ++round) {
                if (lseek(fileDesc, 0, SEEK_SET) == -1) {
                    fileError(tr("Can't change %1 '%2' offset for new round!"), fileStruct);
                    break;
                }

                fileCount = 0;

                roundClock.start();
                progressClock.start();

                if (overwriter->isConstantRound(round)) {
                    overwriter->fillBlock(blockWrite, ERASE_BLOCK_SIZE, round);

                    for (qint64 pos = 0; pos < fileSize && !stopped; ) {
                        qint64 numBytes = fileSize - pos;
                        if (numBytes > ERASE_BLOCK_SIZE) {
                            numBytes = ERASE_BLOCK_SIZE;
                        }

                        performOwrStep(fileDesc, round, numBytes, fileStruct);
                        if (!errorString.isEmpty()) {
                            stopped = true;
                            break;
                        }

                        pos += numBytes;
                    }
                }
                else {
                    for (qint64 pos = 0; pos < fileSize && !stopped; ) {
                        qint64 numBytes = fileSize - pos;
                        if (numBytes > ERASE_BLOCK_SIZE) {
                            numBytes = ERASE_BLOCK_SIZE;
                        }

                        overwriter->fillBlock(blockWrite, ERASE_BLOCK_SIZE, round);

                        performOwrStep(fileDesc, round, numBytes, fileStruct);
                        if (!errorString.isEmpty()) {
                            stopped = true;
                            break;
                        }

                        pos += numBytes;
                    }
                }

                if (fdatasync(fileDesc) != 0) {
                    fileError(tr("Can't synchronize the data of %1 '%2'!"), fileStruct);
                    break;
                }
            }
        }

        if (close(fileDesc) != 0) {
            fileError(tr("Can't close %1 '%2'!"), fileStruct);
        }
    }
    else {
        fileError(tr("Can't open %1 '%2'!"), fileStruct);
    }

    if (!stopped && errorString.isEmpty()) {
        if (S_ISREG(fileStruct.fileStat.st_mode)) {
            eraseFileName(fileStruct);
        }
        else {
            emit finishErasePart();

            ++filesErased;
        }
    }
}

void FileEraser::eraseDir(const FileStruct &fileStruct)
{
    QString dirName = QFileInfo(fileStruct.fileInfo.fileName).fileName();
    if (dirName == "." || dirName == "..") {
        return;
    }
    else {
        emit erasingFile(fileStruct.fileInfo.fileName, 0, true);
    }

    QByteArray baseName = QFile::encodeName(fileStruct.fileInfo.fileName);

    DIR *dirPtr = opendir(baseName);
    if (dirPtr == 0) {
        fileError(tr("Can't open %1 '%2'!"), fileStruct);
        return;
    }

    baseName += '/';

    QByteArray fileName;

    FileStruct newStruct;
    newStruct.fileInfo.deleteAfter = true;

    while (!stopped) {
        struct dirent *dirEntry = readdir(dirPtr);
        if (dirEntry == 0) {
            break;
        }

        fileName = baseName;
        fileName += dirEntry->d_name;

        newStruct.fileInfo.fileName = QFile::decodeName(fileName);

        if (lstat(fileName, &newStruct.fileStat) != 0) {
            errorString = tr("Can't get information about file '%1'!").arg(newStruct.fileInfo.fileName);
            break;
        }

        eraseFile(newStruct);
        if (!errorString.isEmpty()) {
            break;
        }
    }

    if (closedir(dirPtr) != 0) {
        fileError(tr("Can't close %1 '%2'!"), fileStruct);
        return;
    }

    if (!stopped && errorString.isEmpty()) {
        eraseFileName(fileStruct);
    }
}

void FileEraser::eraseFile(const FileStruct &fileStruct)
{
    if (!S_ISDIR(fileStruct.fileStat.st_mode)) {
        emit erasingFile(fileStruct.fileInfo.fileName, 0, true);
    }

    switch (fileStruct.fileStat.st_mode & S_IFMT) {
        case S_IFDIR:
            eraseDir(fileStruct);
            break;
        case S_IFBLK:
        case S_IFREG:
            eraseFileBytes(fileStruct);
            break;
        case S_IFCHR:
        case S_IFIFO:
        case S_IFLNK:
        case S_IFSOCK:
            eraseFileName(fileStruct);
            break;
    }
}

void FileEraser::run()
{
    for (int i = 0; i < filesList.size() && !stopped; ++i) {
        eraseFile(filesList.at(i));
        if (!errorString.isEmpty()) {
            break;
        }
    }

    if (!stopped && errorString.isEmpty()) {
        double bytesErased = totalSize;
        bytesErased /= 1024;
        bytesErased /= 1024;

        finishString = tr("<b><u>Erasing statistics:</u></b><br><br>"
                          "Files deleted: <b>%1 files</b><br>"
                          "Space erased: <b>%2 MiB</b><br>").arg(QString::number(filesErased), fromDouble(bytesErased));
    }

    sync();
}
