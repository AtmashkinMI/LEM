#ifndef FREESPACEERASER_HPP
#define FREESPACEERASER_HPP

#include <stdio.h>
#include <dirent.h>

#include <QTime>
#include <QThread>

class Overwriter;

class FreeSpaceEraser : public QThread
{
    Q_OBJECT

public:
    FreeSpaceEraser(QObject *parent = 0);
    ~FreeSpaceEraser();

    void stopThread();

    bool tmpDirExists();

    QString getRounds();
    QString getLastError();
    QString getFinishString();

    void init(bool doInodes, bool doBlocks, const QString &mountPath, int owrType = -1);

    static qint64 getFreeBlocks(const QByteArray &mountPoint);
    static qint64 getFreeInodes(const QByteArray &mountPoint);

signals:
    void updateType(int);
    void finishErasePart();
    void updateProgress(int);
    void updateStat(double, double, int);

private:
    void run();

    void openBigFile();

    qint64 getFreeBlocks();
    qint64 getFreeInodes();

    bool needCheck(int round);

    QString fromDouble(double num);
    QString fileTypeStr(bool isDir);

    void genRandName(char *randName);

    bool cleanOverwriter(Overwriter *newOverwriter = 0);

    void removeTmpFile(const QByteArray &tmpFile, bool isDir);

    void performCleanStep();
    bool performInodeStep();
    bool performBlockStep(int round);

    bool inodesSupported();

    bool doBlocks;
    bool doInodes;

    bool haveInodes;

    qint64 freeBlockSpace;
    qint64 freeInodeSpace;

    qint64 filesCount;
    qint64 totalCount;

    QTime roundClock;
    QTime totalClock;
    QTime progressClock;

    char *blockCheck;
    char *blockWrite;

    int progress;
    int startPos;
    int bigFileDesc;

    DIR *tmpDirPtr;
    FILE *randSource;

    QString errorString;
    QString finishString;

    QByteArray tmpDirPath;
    QByteArray tmpFilePath;

    Overwriter *overwriter;

    volatile bool stopped;
};

#endif // FREESPACEERASER_HPP
