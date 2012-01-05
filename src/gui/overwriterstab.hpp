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

#ifndef OVERWRITERSTAB_HPP
#define OVERWRITERSTAB_HPP

#include <QWidget>

#include "../srm/overwriters.hpp"

class QLabel;
class QTableWidget;
class QTableWidgetItem;

class OverwritersTab : public QWidget
{
    Q_OBJECT

public:
    OverwritersTab(QWidget *parent = 0);

    int getOwrType();
    void setOwrType(int owrType);

    QString getOwrName(int owrType);

signals:
    void tabChanged();

private:
    void initTableRow(int row, const QStringList &cells);

    QLabel *descLabel;
    QLabel *infoLabel;

    QTableWidget *owrTable;

private slots:
    void setInfoLabel();
};

#endif // OVERWRITERSTAB_HPP
