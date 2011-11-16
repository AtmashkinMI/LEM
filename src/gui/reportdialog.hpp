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

#ifndef REPORTDIALOG_HPP
#define REPORTDIALOG_HPP

#include <QWidget>

class QTextEdit;
class QDialogButtonBox;

class ReportDialog : public QWidget
{
    Q_OBJECT

public:
    ReportDialog(QWidget *parent = 0);

    void showReport(const QString &reportString, bool showDialog);

signals:
    void save();
    void closed();

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QTextEdit *reportEdit;
    QDialogButtonBox *buttonBox;
};

#endif // REPORTDIALOG_HPP
