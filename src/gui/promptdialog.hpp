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

#ifndef PROMPTDIALOG_HPP
#define PROMPTDIALOG_HPP

#include <QWidget>

class QLabel;
class QCheckBox;
class QPushButton;
class SettingsDialog;
class FileEraseDialog;

class PromptDialog : public QWidget
{
    Q_OBJECT

public:
    PromptDialog(const QStringList &filesList, bool isTrash, QWidget *parent = 0);

private:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

    bool isTrash;

    QLabel *imageLabel;
    QLabel *promptLabel;

    QCheckBox *deleteBox;

    QPushButton *eraseButton;
    QPushButton *cancelButton;
    QPushButton *settingsButton;

    SettingsDialog *settingsDialog;
    FileEraseDialog *fileEraseDialog;

    QStringList filesList;

private slots:
    void startErasure();
    void erasureFinished();
};

#endif // PROMPTDIALOG_HPP
