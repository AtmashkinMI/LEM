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
QString getUserHome(const QString &userName);
QList<int> restoreSwapFiles(const QList<SwapInfo> &swapList);

#endif // SYSTEMINFO_HPP
