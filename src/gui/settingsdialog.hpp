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

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QWidget>

class QLabel;
class QSettings;
class GeneralTab;
class QTabWidget;
class QPushButton;
class OverwritersTab;
class QDialogButtonBox;

class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);

    int getOwrType();
    bool getShowStat();
    bool getStopOnError();
    int getShutdownAfter();
    bool getShowTrayMessages();
    QString getOwrName(int owrType);

    static int getLangIndex();
    static QString getLangName(int langIndex = -1);
    static QStringList getQmList();

private:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void readSettings();
    void writeSettings();

    QTabWidget *tabs;

    QSettings *settings;

    GeneralTab *generalTab;

    OverwritersTab *owrTab;

    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;

    QDialogButtonBox *buttonBox;

private slots:
    void okClicked();
    void enableApply();
    void applyClicked();
    void resetSettings();
};

#endif // SETTINGSDIALOG_HPP
