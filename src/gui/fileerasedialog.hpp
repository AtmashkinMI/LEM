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

#ifndef FILEERASEDIALOG_HPP
#define FILEERASEDIALOG_HPP

#include <QTime>
#include <QWidget>

#include "../srm/fileinfo.hpp"

class QLabel;
class FileEraser;
class QPushButton;
class QProgressBar;

class FileEraseDialog : public QWidget
{
    Q_OBJECT

public:
    FileEraseDialog(QWidget *parent = 0);

    QString getErrorString();
    QString getFinishString();

    void startErasure(bool showError, bool showDialog, int owrType, const QString &owrName, const FilesList &filesList);

    static bool isBlockDev(const QString &fileName);
    static QIcon fileTypeIcon(const QString &fileName);
    static QString fileTypeStr(const QString &fileName);
    static FilesList getTrashList(const QString &userName);
    static qint64 getFileSize(const QString &fileName, bool withRem, bool withParent, bool withSpecial, bool withBlockDev);

private:
    QString fromDouble(double num);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QString owrName;

    QString errorString;
    QString finishString;

    QLabel *statLabel;
    QLabel *imageLabel;

    QPushButton *cancelButton;

    QProgressBar *totalProgress;
    QProgressBar *currentProgress;

    FileEraser *eraseThread;

    int leftPart;
    int roundsNumber;

    bool canceled;
    bool exitLock;

    bool showError;

    QTime totalClock;

signals:
    void stopped();
    void finished();

private slots:
    void stopErasure();
    void threadFinished();
    void finishErasePart();
    void renamingRound(int round);
    void changeStatLabel(double speed, double time, int round);
    void changeFormType(const QString &fileName, int type, bool firstTime);
};

#endif // FILEERASEDIALOG_HPP
