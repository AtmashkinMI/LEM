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

#include "promptdialog.hpp"
#include "settingsdialog.hpp"
#include "fileerasedialog.hpp"

PromptDialog::PromptDialog(const QStringList &filesList, bool isTrash, QWidget *parent)
    : QWidget(parent, Qt::Dialog), filesList(filesList)
{
    this->isTrash = isTrash;

    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("Confirm action - %1").arg(qApp->applicationName()));

    settingsDialog = new SettingsDialog(this);
    fileEraseDialog = new FileEraseDialog(this);

    imageLabel = new QLabel(this);
    imageLabel->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(64, 64));
    imageLabel->setAlignment(Qt::AlignCenter);

    QString promptTail;
    if (!isTrash) {
        if (filesList.size() > 1) {
            promptTail = tr("erase these %1 items").arg(filesList.size());
        }
        else {
            QString fileType = FileEraseDialog::fileTypeStr(filesList.first());
            if (!fileType.isEmpty()) {
                promptTail = tr("erase %1 '%2'").arg(fileType, filesList.first());
            }
            else {
                promptTail = tr("erase '%1'").arg(filesList.first());
            }

            deleteBox = new QCheckBox(tr("&Delete after overwriting?"), this);
            deleteBox->setChecked(true);
        }

        eraseButton = new QPushButton(qApp->windowIcon(), tr("&Erase"), this);
    }
    else {
        promptTail = tr("empty the Trash");

        eraseButton = new QPushButton(qApp->windowIcon(), tr("&Empty"), this);
    }

    promptLabel = new QLabel(tr("Are you sure you want to securely %1?").arg(promptTail), this);

    cancelButton = new QPushButton(QIcon::fromTheme("window-close"), tr("&Cancel"), this);
    settingsButton = new QPushButton(QIcon::fromTheme("preferences-system"), tr("&Settings..."), this);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(eraseButton);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(settingsButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(promptLabel);
    rightLayout->addStretch();

    if (!isTrash && filesList.size() == 1) {
        rightLayout->addSpacing(10);
        rightLayout->addWidget(deleteBox);
        rightLayout->addSpacing(5);
    }

    rightLayout->addLayout(buttonsLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);

    setFixedHeight(sizeHint().height());

    setWindowModality(Qt::ApplicationModal);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(eraseButton, SIGNAL(clicked()), this, SLOT(startErasure()));
    connect(settingsButton, SIGNAL(clicked()), settingsDialog, SLOT(show()));
    connect(fileEraseDialog, SIGNAL(finished()), this, SLOT(erasureFinished()));
}

void PromptDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        eraseButton->setFocus();

        resize(450, height());
    }
}

void PromptDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void PromptDialog::erasureFinished()
{
    if (settingsDialog->getShowStat()) {
        QMessageBox::information(this, tr("Finished!"), fileEraseDialog->getFinishString());
    }
}

void PromptDialog::startErasure()
{
    close();

    int owrType = settingsDialog->getOwrType();

    FilesList fileInfoList;

    if (!isTrash) {
        FileInfo fileInfo;

        if (filesList.size() == 1) {
            fileInfo.deleteAfter = deleteBox->isChecked();
        }
        else {
            fileInfo.deleteAfter = true;
        }

        for (int i = 0; i < filesList.size(); ++i) {
            fileInfo.fileName = filesList.at(i);

            fileInfoList << fileInfo;
        }
    }
    else {
        fileInfoList = fileEraseDialog->getTrashList(filesList.first());
    }

    fileEraseDialog->startErasure(true, true, owrType, settingsDialog->getOwrName(owrType), fileInfoList);
}
