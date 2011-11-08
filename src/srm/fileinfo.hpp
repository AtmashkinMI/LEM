#ifndef FILEINFO_HPP
#define FILEINFO_HPP

#include <QList>
#include <QString>

struct FileInfo {
    QString fileName;
    bool deleteAfter;
};

typedef QList<FileInfo> FilesList;

#endif // FILEINFO_HPP
