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

#include "freespaceerasedialog.hpp"
#include "../srm/freespaceeraser.hpp"

FreeSpaceEraseDialog::FreeSpaceEraseDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    statLabel = new QLabel(this);

    progressBar = new QProgressBar(this);
    progressBar->setFormat(tr("Erasing progress: %p%"));

    cancelButton = new QPushButton(QIcon::fromTheme("window-close"), tr("&Cancel"), this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addStretch();
    rightLayout->addWidget(statLabel);
    rightLayout->addWidget(progressBar);
    rightLayout->addStretch();
    rightLayout->addLayout(buttonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);

    canceled = false;
    errorSet = false;
    exitLock = false;

    eraseThread = new FreeSpaceEraser(this);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopErasure()));
    connect(eraseThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(eraseThread, SIGNAL(finishErasePart()), this, SLOT(finishErasePart()));
    connect(eraseThread, SIGNAL(updateType(int)), this, SLOT(changeFormType(int)));
    connect(eraseThread, SIGNAL(updateProgress(int)), progressBar, SLOT(setValue(int)));
    connect(eraseThread, SIGNAL(updateStat(double, double, int)), this, SLOT(changeStatLabel(double, double, int)));
}

QString FreeSpaceEraseDialog::fromDouble(double num)
{
    return QString::number(num, 'f', 2);
}

QString FreeSpaceEraseDialog::getErrorString()
{
    return errorString;
}

QString FreeSpaceEraseDialog::getFinishString()
{
    return finishString;
}

void FreeSpaceEraseDialog::closeEvent(QCloseEvent *event)
{
    if (exitLock) {
        if (eraseThread->isFinished()) {
            if (canceled) {
                emit stopped();
            }
            else {
                emit finished();
            }

            event->accept();
        }
        else {
            event->ignore();
        }
    }
    else {
        stopErasure();
        event->ignore();
    }
}

void FreeSpaceEraseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void FreeSpaceEraseDialog::changeStatLabel(double speed, double time, int round)
{
    if (time >= 0) {
        QString newStat;
        if (round >= 0) {
            newStat = statPattern.arg(fromDouble(speed), fromDouble(time), QString::number(round + 1), roundsNumber);
        }
        else {
            newStat = statPattern.arg(fromDouble(speed), fromDouble(time));
        }

        if (time < 0.01) {
            newStat.chop(newStat.size() - newStat.indexOf('(') + 1);
        }

        statLabel->setText(newStat);
    }
    else {
        statLabel->setText(statPattern.arg(fromDouble(speed)));
    }
}

void FreeSpaceEraseDialog::finishErasePart()
{
    statLabel->setText(tr("Finished!"));

    if (progressBar->maximum() != 100) {
        progressBar->setMaximum(100);
    }

    progressBar->setValue(100);

    progressBar->repaint();
}

void FreeSpaceEraseDialog::setErrorImage()
{
    if (!errorSet) {
        errorSet = true;

        imageLabel->setPixmap(QIcon::fromTheme("dialog-error").pixmap(64, 64));
    }
}

void FreeSpaceEraseDialog::changeFormType(int eraseType)
{
    statLabel->setText(tr("Calculating stats..."));

    if (eraseType != 1) {
        progressBar->setMaximum(100);
        progressBar->setValue(0);
    }
    else {
        progressBar->setMaximum(0);
    }

    progressBar->repaint();

    switch (eraseType) {
        case 0:
            setWindowTitle(tr("Erasing free inode space on '%1'").arg(mountPath));
            statPattern = tr("Average speed: %1 inodes/s (%2 seconds remaining)");
            imageLabel->setPixmap(QIcon(":/inode_space.png").pixmap(64, 64));
            break;

        case 1:
            setWindowTitle(tr("Erasing free inode space on '%1' [ReiserFS]").arg(mountPath));
            statPattern = tr("Average speed: %1 inodes/s");
            imageLabel->setPixmap(QIcon(":/inode_space.png").pixmap(64, 64));
            break;

        case 2:
            setWindowTitle(tr("Erasing free block space on '%1' [%2]").arg(mountPath, owrName));
            statPattern = tr("Average speed [round %3/%4]: %1 KiB/s (%2 seconds remaining)");
            imageLabel->setPixmap(QIcon(":/block_space.png").pixmap(64, 64));
            break;

        case 3:
            exitLock = true;
            cancelButton->setEnabled(false);
            setWindowTitle(tr("Cleaning allocated space on '%1'").arg(mountPath));
            statPattern = tr("Average speed: %1 files/s (%2 seconds remaining)");
            imageLabel->setPixmap(QIcon(":/clear.png").pixmap(64, 64));
            break;

        case 4:
            exitLock = true;
            cancelButton->setEnabled(false);
            setWindowTitle(tr("Cleaning after error on '%1'").arg(mountPath));
            statPattern = tr("Average speed: %1 files/s (%2 seconds remaining)");
            setErrorImage();
            break;
    }
}

void FreeSpaceEraseDialog::stopErasure()
{
    if (!exitLock) {
        canceled = true;
        exitLock = true;
        cancelButton->setEnabled(false);
        eraseThread->stopThread();
    }
}

void FreeSpaceEraseDialog::threadFinished()
{
    errorString = eraseThread->getLastError();

    if (!canceled || !errorString.isEmpty()) {
        exitLock = true;
        cancelButton->setEnabled(false);

        if (!errorString.isEmpty()) {
            canceled = true;

            statLabel->setText(tr("Error!"));

            if (eraseThread->tmpDirExists()) {
                errorString += tr("<br><br><u><b>Not all files were deleted!</b></u>");
            }

            if (progressBar->maximum() != 100) {
                progressBar->setMaximum(100);
            }

            progressBar->setValue(0);
            progressBar->repaint();

            setErrorImage();

            if (showError) {
                QMessageBox errorMessage(this);
                errorMessage.setTextFormat(Qt::PlainText);
                errorMessage.setIcon(QMessageBox::Critical);
                errorMessage.addButton(QMessageBox::Ok);
                errorMessage.setWindowTitle(statLabel->text());
                errorMessage.setText(errorString);
                errorMessage.exec();
            }
        }
        else {
            finishErasePart();

            finishString = eraseThread->getFinishString();

            double timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }
            timeElapsed /= 1000;

            finishString += tr("<u>Total time:</u> <b>%1 seconds</b>").arg(fromDouble(timeElapsed));
        }
    }

    close();
}

qint64 FreeSpaceEraseDialog::getFreeBlocks(const QString &mountPoint)
{
    return FreeSpaceEraser::getFreeBlocks(QFile::encodeName(mountPoint));
}

qint64 FreeSpaceEraseDialog::getFreeInodes(const QString &mountPoint)
{
    return FreeSpaceEraser::getFreeInodes(QFile::encodeName(mountPoint));
}

void FreeSpaceEraseDialog::startErasure(bool showError, bool showDialog, bool doInodes, bool doBlocks, const QString &mountPath, int owrType, const QString &owrName)
{
    setWindowTitle(tr("Starting erasing thread..."));

    statLabel->setText(tr("Preparing erasing procedure..."));

    cancelButton->setEnabled(true);
    cancelButton->setFocus();

    errorString.clear();
    finishString.clear();

    canceled = false;
    errorSet = false;
    exitLock = false;

    imageLabel->setPixmap(QIcon(":/hourglass.png").pixmap(64, 64));

    progressBar->setMaximum(0);
    progressBar->repaint();

    setFixedHeight(sizeHint().height());
    resize(700, height());

    this->owrName = owrName;
    this->mountPath = mountPath;
    this->showError = showError;

    eraseThread->init(doInodes, doBlocks, mountPath, owrType);
    errorString = eraseThread->getLastError();
    if (!errorString.isEmpty()) {
        if (showError) {
            QMessageBox errorMessage(this);
            errorMessage.setTextFormat(Qt::PlainText);
            errorMessage.setIcon(QMessageBox::Critical);
            errorMessage.addButton(QMessageBox::Ok);
            errorMessage.setWindowTitle(tr("Error!"));
            errorMessage.setText(errorString);
            errorMessage.exec();
        }

        emit stopped();

        return;
    }

    if (doBlocks) {
        roundsNumber = eraseThread->getRounds();
    }

    totalClock.start();

    eraseThread->start();

    if (showDialog) {
        show();
    }
}
