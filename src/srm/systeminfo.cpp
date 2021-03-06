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

#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QTextStream>

#include "systeminfo.hpp"

QList<SwapInfo> getSwapFiles()
{
    SwapInfo swapInfo;

    QList<SwapInfo> swapList;

    QProcess swapon;
    swapon.start("/sbin/swapon -s");
    if (swapon.waitForFinished(-1)) {
        QString infoLine;

        QTextStream swaponStream(&swapon);

        forever {
            infoLine = swaponStream.readLine();
            if (infoLine.isEmpty()) {
                break;
            }

            if (!infoLine.startsWith('/')) {
                continue;
            }

            int fileIndex = infoLine.lastIndexOf("file");
            int partIndex = infoLine.lastIndexOf("partition");

            if (fileIndex == -1 && partIndex == -1) {
                continue;
            }

            int index = fileIndex > partIndex ? fileIndex : partIndex;
            swapInfo.swapPath = infoLine.left(index).trimmed();

            QFileInfo swapPathInfo(swapInfo.swapPath);
            if (swapPathInfo.exists() && swapPathInfo.isAbsolute() && !swapPathInfo.isDir() && !swapPathInfo.isSymLink()) {
                QProcess swaplabel;
                swaplabel.start("/sbin/swaplabel", QStringList() << swapInfo.swapPath);
                if (swaplabel.waitForFinished(-1)) {
                    QTextStream swaplabelStream(&swaplabel);

                    swapInfo.swapUUID.clear();
                    swapInfo.swapLabel.clear();

                    bool error = false;

                    forever {
                        infoLine = swaplabelStream.readLine();
                        if (infoLine.isEmpty()) {
                            break;
                        }

                        if (infoLine.startsWith("UUID:  ")) {
                            swapInfo.swapUUID = infoLine.right(infoLine.size() - 7);
                        }
                        else if (infoLine.startsWith("LABEL: ")) {
                            swapInfo.swapLabel = infoLine.right(infoLine.size() - 7);
                        }
                        else {
                            error = true;
                            break;
                        }
                    }

                    if (!error) {
                        swapList << swapInfo;
                    }
                }
            }

        }
    }

    return swapList;
}

bool disableAllSwaps()
{
    QProcess swapoff;
    swapoff.start("/sbin/swapoff -a");

    return swapoff.waitForFinished(-1) && swapoff.exitStatus() == QProcess::NormalExit && swapoff.exitCode() == 0;
}

QList<int> restoreSwapFiles(const QList<SwapInfo> &swapList)
{
    QList<int> numList;

    for (int i = 0; i < swapList.size(); ++i) {
        QProcess mkswap;
        QStringList mkswapParams;

        SwapInfo swapInfo = swapList.at(i);
        if (!swapInfo.swapLabel.isEmpty()) {
            mkswapParams << "-L" << swapInfo.swapLabel;
        }
        if (!swapInfo.swapUUID.isEmpty()) {
            mkswapParams << "-U" << swapInfo.swapUUID;
        }

        mkswapParams << swapInfo.swapPath;

        mkswap.start("/sbin/mkswap", mkswapParams);
        if (mkswap.waitForFinished(-1) && mkswap.exitStatus() == QProcess::NormalExit && mkswap.exitCode() == 0) {
            QProcess swapon;
            swapon.start("/sbin/swapon", QStringList() << swapInfo.swapPath);
            if (swapon.waitForFinished(-1) && swapon.exitStatus() == QProcess::NormalExit && swapon.exitCode() == 0) {
                continue;
            }
        }

        numList << i;
    }

    return numList;
}

QStringList getMountPoints()
{
    QStringList mountsList;

    QProcess mount;
    mount.start("mount");
    if (mount.waitForFinished(-1)) {
        QString infoLine;

        QStringList supportedFS;
        supportedFS << "ext2" << "ext3" << "ext4" << "JFS" << "XFS" << "ReiserFS";

        QTextStream mountStream(&mount);

        forever {
            infoLine = mountStream.readLine();
            if (infoLine.isEmpty()) {
                break;
            }

            int endPath = infoLine.lastIndexOf(" type ");
            if (endPath == -1) {
                continue;
            }

            int beginPath = 0;

            bool found = false;

            while (!found) {
                beginPath = infoLine.indexOf(" on /", beginPath);
                if (beginPath == -1 || beginPath >= endPath - 4) {
                    break;
                }

                beginPath += 4;

                int endFS = infoLine.indexOf(' ', endPath + 6);
                if (endFS != -1) {
                    QString nameFS = infoLine.mid(endPath + 6, endFS - endPath - 6);
                    for (int i = 0; i < supportedFS.size(); ++i) {
                        if (nameFS.compare(supportedFS.at(i), Qt::CaseInsensitive) == 0) {
                            QString mountPath = infoLine.mid(beginPath, endPath - beginPath);
                            QFileInfo mountPathInfo(mountPath);

                            if (mountPathInfo.exists() && mountPathInfo.isAbsolute() && mountPathInfo.isDir() && !mountPathInfo.isSymLink()) {
                                mountsList << mountPath << supportedFS.at(i);
                                found = true;
                            }

                            break;
                        }
                    }
                }
            }
        }
    }

    return mountsList;
}

QStringList getBlockDevices()
{
    QDir blockDir("/sys/class/block");
    blockDir.setSorting(QDir::Name);
    blockDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    QStringList blockDevList = blockDir.entryList();

    QStringList newList;

    for (int i = 0; i < blockDevList.size(); ++i) {
        QString fileName = blockDevList.at(i);
        if (!fileName.startsWith("sr")) {
            newList << fileName.prepend("/dev/");
        }
    }

    return newList;
}

bool checkRunningKDE()
{
    QProcess kcheck;
    kcheck.start("kcheckrunning");

    return kcheck.waitForFinished(-1) && kcheck.exitStatus() == QProcess::NormalExit && kcheck.exitCode() == 0;
}

QStringList getUsersList()
{
    QStringList users("root");

    QFile passwd("/etc/passwd");
    if (passwd.open(QFile::ReadOnly)) {
        QString readLine;

        QTextStream passwdStream(&passwd);

        forever {
            readLine = passwdStream.readLine();
            if (readLine.isEmpty()) {
                break;
            }

            if (readLine.contains("/home")) {
                users << readLine.section(':', 0, 0);
            }
        }
    }

    return users;
}

QString getUserHome(const QString &userName)
{
    if (userName == "root") {
        return "/root";
    }
    else {
        return QString("/home/%1").arg(userName);
    }
}

QString fromDouble(double num)
{
    return QString::number(num, 'f', 2);
}

QString fileSizeToStr(qint64 fileSize)
{
    if (fileSize < 1024) {
        return QObject::tr("%1 B").arg(QString::number(fileSize));
    }

    double fileSizeNew = fileSize;

    fileSizeNew /= 1024;
    if (fileSizeNew < 1024) {
        return QObject::tr("%1 KiB").arg(fromDouble(fileSizeNew));
    }

    fileSizeNew /= 1024;
    if (fileSizeNew < 1024) {
        return QObject::tr("%1 MiB").arg(fromDouble(fileSizeNew));
    }

    fileSizeNew /= 1024;
    if (fileSizeNew < 1024) {
        return QObject::tr("%1 GiB").arg(fromDouble(fileSizeNew));
    }

    fileSizeNew /= 1024;
    if (fileSizeNew < 1024) {
        return QObject::tr("%1 TiB").arg(fromDouble(fileSizeNew));
    }

    fileSizeNew /= 1024;
    if (fileSizeNew < 1024) {
        return QObject::tr("%1 PiB").arg(fromDouble(fileSizeNew));
    }

    fileSizeNew /= 1024;

    return QObject::tr("%1 EiB").arg(fromDouble(fileSizeNew));
}
