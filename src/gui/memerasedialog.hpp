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

#ifndef MEMERASEDIALOG_HPP
#define MEMERASEDIALOG_HPP

#include <QTime>
#include <QWidget>

class QLabel;
class QProcess;
class QPushButton;
class QProgressBar;

class MemEraseDialog : public QWidget
{
    Q_OBJECT

public:
    MemEraseDialog(QWidget *parent = 0);

    QString getErrorString();
    QString getFinishString();

    void startErasure(bool showError, bool showDialog, int owrType, const QString &owrName);

    static qint64 getMemTotal();

private:
    void saveOvercommitState();
    void changeOvercommitState(char newState);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    bool canceled;
    bool exitLock;

    bool showError;

    int owrType;

    QString owrName;

    QString errorString;
    QString finishString;

    qint64 bytesCount;
    qint64 totalMemSize;

    char overcommitState;

    QLabel *statLabel;
    QLabel *imageLabel;

    QTime totalClock;
    QTime progressClock;

    QPushButton *cancelButton;

    QProgressBar *progressBar;

    QProcess *eraseProc;

    QByteArray procBuffer;

signals:
    void stopped();
    void finished();

private slots:
    void processOut();
    void processExit();
    void processStart();

    void stopErasure();
};

#endif // MEMERASEDIALOG_HPP
