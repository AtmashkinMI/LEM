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

#include <QtGui>

#include "reportdialog.hpp"

ReportDialog::ReportDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    reportEdit = new QTextEdit(this);
    reportEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    reportEdit->setReadOnly(true);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SIGNAL(save()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(reportEdit);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(tr("Finished!"));
}

void ReportDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        buttonBox->button(QDialogButtonBox::Close)->setFocus();
    }
}

void ReportDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void ReportDialog::closeEvent(QCloseEvent *event)
{
    emit closed();

    event->accept();
}

void ReportDialog::showReport(const QString &reportString, bool showDialog)
{
    reportEdit->setHtml(reportString);
    resize(500, 430);

    if (showDialog) {
        show();
    }
}
