/*
   Copyright (C) 2011 by Atmashkin M.I. All Rights Reserved.

   This file is part of LEM.

   LEM is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   LEM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with LEM. If not, see <http://www.gnu.org/licenses/>.
*/

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

    void init(int owrType, const FilesList &filesList);

    static bool isBlockDev(const QString &fileName);
    static QIcon fileTypeIcon(const QString &fileName);
    static QString fileTypeStr(const QString &fileName);
    static qint64 getFileSize(const QString &fileName, bool withRem, bool withParent, bool withSpecial, bool withBlockDev);

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

    qint64 freedCount;

    qint64 filesErased;
    qint64 filesDeleted;

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
