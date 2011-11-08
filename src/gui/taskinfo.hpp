#ifndef TASKINFO_HPP
#define TASKINFO_HPP

#include <QMap>
#include <QIcon>

#include "../srm/fileinfo.hpp"

enum TaskType {Disk, Ram, Swap, Files, Unused, Custom};

struct TaskInfo {
    bool doBlocks;
    bool doInodes;
    TaskType taskType;
    QString fileSystem;
    QString description;
    FilesList filesList;
};

struct UserTask {
    QIcon taskIcon;
    QString taskName;
    TaskInfo taskInfo;
};

struct UserProfile {
    UserTask parentTask;
    QList<UserTask> childTaks;
};

class QTreeWidgetItem;

typedef QMap<QTreeWidgetItem *, TaskInfo> TaskMap;

#endif // TASKINFO_HPP
