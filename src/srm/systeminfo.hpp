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

#ifndef SYSTEMINFO_HPP
#define SYSTEMINFO_HPP

#include <QStringList>

struct SwapInfo {
    QString swapPath;
    QString swapUUID;
    QString swapLabel;
};

QStringList getMountPoints();
QStringList getBlockDevices();

bool disableAllSwaps();
bool checkRunningKDE();
QStringList getUsersList();
QList<SwapInfo> getSwapFiles();
QString fromDouble(double num);
QString fileSizeToStr(qint64 fileSize);
QString getUserHome(const QString &userName);
QList<int> restoreSwapFiles(const QList<SwapInfo> &swapList);

#endif // SYSTEMINFO_HPP
