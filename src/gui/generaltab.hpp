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

#ifndef GENERALTAB_HPP
#define GENERALTAB_HPP

#include <QWidget>

class QSpinBox;
class QCheckBox;
class QComboBox;
class QToolButton;

class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    GeneralTab(QWidget *parent = 0);

    bool getShowStat();
    void setShowStat(bool showStat);

    bool getStopOnError();
    void setStopOnError(bool stopOnError);

    bool getShowTrayMessages();
    void setShowTrayMessages(bool showTrayMessages);

    int getLangIndex();
    void setLangIndex(int langIndex);

    int getShutdownAfter();
    void setShutdownAfter(int shutdownAfter);

signals:
    void tabChanged();

private:
    QString languageName(const QString &qmFile);

    QComboBox *langBox;

    QCheckBox *showStatBox;
    QCheckBox *stopOnErrorBox;
    QCheckBox *showTrayMessagesBox;

    QToolButton *kdeAddButton;
    QToolButton *kdeDelButton;

    QToolButton *xfceAddButton;
    QToolButton *xfceDelButton;

    QToolButton *gnomeAddButton;
    QToolButton *gnomeDelButton;

    QSpinBox *shutdownAfterBox;

public slots:
    void kdeDelSlot();
    void xfceDelSlot();
    void gnomeDelSlot();

private slots:
    void kdeAddSlot();
    void xfceAddSlot();
    void gnomeAddSlot();
};

#endif // GENERALTAB_HPP
