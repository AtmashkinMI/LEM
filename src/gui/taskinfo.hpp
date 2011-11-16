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
