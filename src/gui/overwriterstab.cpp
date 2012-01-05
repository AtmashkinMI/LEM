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

#include "overwriterstab.hpp"

OverwritersTab::OverwritersTab(QWidget *parent)
    : QWidget(parent)
{
    owrTable = new QTableWidget(OVERWRITERS_COUNT, 3, this);

    for (int i = 0; i < OVERWRITERS_COUNT; ++i) {
        QTableWidgetItem *rowItem = new QTableWidgetItem(QString::number(i + 1));
        rowItem->setTextAlignment(Qt::AlignCenter);
        owrTable->setVerticalHeaderItem(i, rowItem);
    }

    owrTable->setTextElideMode(Qt::ElideNone);

    owrTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Passes") << tr("Verification"));
    owrTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    for (int i = 0; i < OVERWRITERS_COUNT; ++i) {
        if (Overwriter *owr = intToOwr(i)) {
            initTableRow(i, QStringList() << owr->getName() << QString::number(owr->getRounds()) << owr->verificationName(owr->verificationType()));

            delete owr;
        }
    }

    owrTable->setSelectionBehavior(QTableWidget::SelectRows);
    owrTable->setSelectionMode(QTableWidget::SingleSelection);

    owrTable->verticalHeader()->setResizeMode(QHeaderView::Stretch);

    owrTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    owrTable->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    owrTable->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);

    owrTable->setEditTriggers(QTableWidget::NoEditTriggers);

    descLabel = new QLabel(tr("<u>Description:</u>"), this);

    infoLabel = new QLabel(this);
    infoLabel->setWordWrap(true);
    infoLabel->setMinimumHeight(50);
    infoLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(owrTable);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(descLabel);
    mainLayout->addWidget(infoLabel);

    setLayout(mainLayout);

    connect(owrTable, SIGNAL(itemSelectionChanged()), this, SLOT(setInfoLabel()));
}

void OverwritersTab::initTableRow(int row, const QStringList &cells)
{
    for (int i = 0; i < 3; ++i) {
        QTableWidgetItem *rowItem = new QTableWidgetItem(cells.at(i));
        rowItem->setTextAlignment(Qt::AlignCenter);
        owrTable->setItem(row, i, rowItem);
    }
}

int OverwritersTab::getOwrType()
{
    return owrTable->selectedItems().first()->row();
}

void OverwritersTab::setOwrType(int owrType)
{
    owrTable->selectRow(owrType);
}

QString OverwritersTab::getOwrName(int owrType)
{
    return owrTable->item(owrType, 0)->text();
}

void OverwritersTab::setInfoLabel()
{
    if (Overwriter *owr = intToOwr(owrTable->selectedItems().first()->row())) {
        infoLabel->setText(owr->getDescription());

        delete owr;
    }

    emit tabChanged();
}
