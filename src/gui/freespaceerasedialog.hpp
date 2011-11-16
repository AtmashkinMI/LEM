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

#ifndef FREESPACEERASEDIALOG_HPP
#define FREESPACEERASEDIALOG_HPP

#include <QTime>
#include <QWidget>

class QLabel;
class QPushButton;
class QProgressBar;
class FreeSpaceEraser;

class FreeSpaceEraseDialog : public QWidget
{
    Q_OBJECT

public:
    FreeSpaceEraseDialog(QWidget *parent = 0);

    QString getErrorString();
    QString getFinishString();

    void startErasure(bool showError, bool showDialog, bool doInodes, bool doBlocks, const QString &mountPath, int owrType = -1, const QString &owrName = QString());

    static qint64 getFreeBlocks(const QString &mountPoint);
    static qint64 getFreeInodes(const QString &mountPoint);

private:
    void setErrorImage();

    QString fromDouble(double num);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QString owrName;
    QString mountPath;
    QString statPattern;
    QString roundsNumber;

    QString errorString;
    QString finishString;

    QLabel *statLabel;
    QLabel *imageLabel;

    QPushButton *cancelButton;

    QProgressBar *progressBar;

    FreeSpaceEraser *eraseThread;

    bool canceled;
    bool errorSet;
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

    void changeFormType(int eraseType);
    void changeStatLabel(double speed, double time, int round);
};

#endif // FREESPACEERASEDIALOG_HPP
