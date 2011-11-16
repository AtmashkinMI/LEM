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

#ifndef DESCRIPTIONWIDGET_HPP
#define DESCRIPTIONWIDGET_HPP

#include <QWidget>

#include "taskinfo.hpp"

class QLabel;
class QPushButton;
class QToolButton;
class QTreeWidget;
class QVBoxLayout;

class DescriptionWidget : public QWidget
{
    Q_OBJECT

public:
    DescriptionWidget(TaskMap &tasks, QWidget *parent = 0);

    void setApplicationWindow();
    void setChildWindow(QTreeWidgetItem *task);
    void setParentWindow(QTreeWidgetItem *task);

private:
    void clearWidget();
    void clearLayout(QLayout *layout);

    QString fromDouble(double num);

    void createRamTree();
    void createFilesTree();
    void createUnusedTree();

    void fillCustomTree();
    void createCustomTree(QVBoxLayout *&buttonsLayout);

    TaskMap &tasks;

    QFont bigFont;
    QFont littleFont;
    QFont middleFont;

    QLabel *totalSizeLabel;

    QTreeWidget *filesTree;
    QTreeWidgetItem *currentTask;
    QTreeWidgetItem *unusedSpacetask;

    QToolButton *delEntriesButton;

    QMap<QPushButton *, QTreeWidgetItem *> buttonsMap;

private slots:
    void selectItem();
    void addDirsToCustomTree();
    void addFilesToCustomTree();
    void treeSelectionChanged();
    void delEntriesFromCustomTree();
    void checkUnusedSpace(QTreeWidgetItem *item, int column);
    void treeCheckStateChanged(QTreeWidgetItem *item, int column);
};

#endif // DESCRIPTIONWIDGET_HPP
