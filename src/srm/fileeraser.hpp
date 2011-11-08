#ifndef FILEERASER_HPP
#define FILEERASER_HPP

#include <stdio.h>

#include <sys/stat.h>

#include <QIcon>
#include <QTime>
#include <QThread>

#include "fileinfo.hpp"

class Overwriter;

class FileEraser : public QThread
{
    Q_OBJECT

public:
    FileEraser(QObject *parent = 0);
    ~FileEraser();

    int getRounds();

    void stopThread();

    QString getLastError();
    QString getFinishString();

    QString fromDouble(double num);

    void init(int owrType, const FilesList &filesList);

    static qint64 getFileSize(const QString &fileName);
    static QIcon fileTypeIcon(const QString &fileName);
    static QString fileTypeStr(const QString &fileName);

signals:
    void finishErasePart();
    void renamingRound(int);
    void fileProgressSignal(int);
    void totalProgressSignal(int);
    void updateStat(double, double, int);
    void erasingFile(const QString &, int, bool);

private:
    struct FileStruct {
        FileInfo fileInfo;
        struct stat fileStat;
    };

    void run();

    bool needCheck(int round);

    void genRandName(int startPos, char *randName);

    static int blockDevType(const QString &fileName);

    void eraseDir(const FileStruct &fileStruct);
    void eraseFile(const FileStruct &fileStruct);
    void eraseFileName(const FileStruct &fileStruct);
    void eraseFileBytes(const FileStruct &fileStruct);

    bool cleanOverwriter(Overwriter *newOverwriter = 0);

    void fileError(const QString &errorPattern, const FileStruct &fileStruct);

    void performOwrStep(int fileDesc, int round, qint64 numBytes, const FileStruct &fileStruct);

    char *blockCheck;
    char *blockWrite;

    qint64 fileSize;
    qint64 fileCount;
    int fileProgress;

    qint64 totalSize;
    qint64 totalCount;
    int totalProgress;

    qint64 filesErased;

    QTime roundClock;
    QTime progressClock;

    FILE *randSource;

    QString errorString;
    QString finishString;

    Overwriter *overwriter;

    QList<FileStruct> filesList;

    volatile bool stopped;
};

#endif // FILEERASER_HPP
