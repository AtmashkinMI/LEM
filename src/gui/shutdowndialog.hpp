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

#ifndef SHUTDOWNDIALOG_HPP
#define SHUTDOWNDIALOG_HPP

#include <QWidget>

class QLabel;
class QTimer;
class QPushButton;
class QDialogButtonBox;

class ShutdownDialog : public QWidget
{
    Q_OBJECT

public:
    ShutdownDialog(QWidget *parent = 0);

    void startCount(int shutdownAfter);

private:
    void updateCount();

    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    int shutdownCount;
    int shutdownAfter;

    QLabel *textLabel;
    QLabel *imageLabel;

    QTimer *shutdownTimer;

    QPushButton *okButton;
    QPushButton *cancelButton;

    QDialogButtonBox *buttonBox;

private slots:
    void secondElapsed();
    void shutdownSystem();
};

#endif // SHUTDOWNDIALOG_HPP
