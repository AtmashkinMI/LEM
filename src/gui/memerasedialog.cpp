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

#include <linux/oom.h>

#include "memerasedialog.hpp"

#include "../srm/systeminfo.hpp"

#define AVAILABLE_MEM_PART 0.885

MemEraseDialog::MemEraseDialog(QWidget *parent)
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
    exitLock = false;

    eraseProc = new QProcess(this);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopErasure()));
    connect(eraseProc, SIGNAL(readyReadStandardOutput()), this, SLOT(processOut()));
    connect(eraseProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processExit()));
}

void MemEraseDialog::closeEvent(QCloseEvent *event)
{
    if (exitLock) {
        if (eraseProc->state() == QProcess::NotRunning) {
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
        if (cancelButton->isEnabled()) {
            stopErasure();
        }

        event->ignore();
    }
}

void MemEraseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

QString MemEraseDialog::getErrorString()
{
    return errorString;
}

QString MemEraseDialog::getFinishString()
{
    return finishString;
}

void MemEraseDialog::processStart()
{
    setWindowTitle(tr("Erasing RAM [%1]").arg(owrName));
    statLabel->setText(tr("Calculating stats..."));
    imageLabel->setPixmap(QIcon(":/ram.png").pixmap(64, 64));

    cancelButton->setEnabled(true);
    cancelButton->setFocus();

    progressBar->setMaximum(100);
    progressBar->setValue(0);
    progressBar->repaint();

    totalClock.start();
    progressClock.start();

    eraseProc->start(QString("%1 --memory %2").arg(QApplication::applicationFilePath(), QString::number(owrType)));

    eraseProc->waitForStarted(-1);
}

void MemEraseDialog::processOut()
{
    if (!exitLock) {
        procBuffer += eraseProc->readAllStandardOutput();
        int lastNewLine = procBuffer.lastIndexOf('\n');
        if (lastNewLine > 0) {
            int preLastNewLine = procBuffer.lastIndexOf('\n', lastNewLine - 1);
            bytesCount = procBuffer.mid(preLastNewLine + 1, lastNewLine - preLastNewLine - 1).toLongLong();
            procBuffer = procBuffer.right(procBuffer.size() - lastNewLine - 1);

            if (progressClock.elapsed() < PROGRESS_UPDATE_FREQ_MS) {
                return;
            }

            int timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }

            double speed = bytesCount;
            speed *= 1000;
            speed /= 1024 * 1024;
            speed /= timeElapsed;
            if (speed < 0) {
                speed = 0;
            }

            double timeLeft = totalMemSize;
            timeLeft *= AVAILABLE_MEM_PART;
            timeLeft -= bytesCount;
            timeLeft /= 1024 * 1024;
            if (speed != 0) {
                timeLeft /= speed;
            }
            if (timeLeft < 0 || speed == 0) {
                timeLeft = 0;
            }

            QString statString = tr("Average speed: %1 MiB/s").arg(fromDouble(speed));
            if (timeLeft >= 0.01) {
                statString += tr(" (%1 seconds remaining)").arg(fromDouble(timeLeft));
            }

            statLabel->setText(statString);

            double erasedPart = bytesCount;
            erasedPart /= totalMemSize;
            erasedPart /= AVAILABLE_MEM_PART;
            erasedPart *= 100;
            int progress = (int) erasedPart;
            if (progress > progressBar->value() && progress <= 99) {
                progressBar->setValue(progress);
            }

            progressClock.start();
        }
    }
}

void MemEraseDialog::stopErasure()
{
    if (!exitLock) {
        canceled = true;
        exitLock = true;
        cancelButton->setEnabled(false);
        progressBar->setValue(0);
        progressBar->repaint();
        statLabel->setText(tr("Canceling..."));
        eraseProc->kill();
    }
}

qint64 MemEraseDialog::getMemTotal()
{
    QFile memInfoFile("/proc/meminfo");
    memInfoFile.open(QFile::ReadOnly);

    return memInfoFile.readLine().simplified().split(' ').at(1).toLongLong() * 1024;
}

void MemEraseDialog::saveOvercommitState()
{
    QFile overcommitFile("/proc/sys/vm/overcommit_memory");
    overcommitFile.open(QFile::ReadOnly);
    overcommitFile.read(&overcommitState, 1);
}

void MemEraseDialog::changeOvercommitState(char newState)
{
    QFile overcommitFile("/proc/sys/vm/overcommit_memory");
    overcommitFile.open(QFile::WriteOnly);
    overcommitFile.write(&newState, 1);
}

void MemEraseDialog::processExit()
{
    changeOvercommitState(overcommitState);

    if (!canceled) {
        exitLock = true;
        cancelButton->setEnabled(false);

        if (eraseProc->exitStatus() == QProcess::NormalExit && eraseProc->exitCode() == EXIT_ERROR) {
            canceled = true;
            progressBar->setValue(0);
            progressBar->repaint();
            statLabel->setText(tr("Error!"));
            imageLabel->setPixmap(QIcon::fromTheme("dialog-error").pixmap(64, 64));
            errorString = tr("An error occured during RAM erasing!");
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
            double timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }
            timeElapsed /= 1000;

            double avgSpeed = bytesCount;
            avgSpeed /= timeElapsed;
            avgSpeed /= 1024;
            avgSpeed /= 1024;

            QString exitType;
            if (eraseProc->exitStatus() == QProcess::NormalExit) {
                if (eraseProc->exitCode() == EXIT_SUCCESS) {
                    exitType = tr("by normal 'return' statement");
                }
                else {
                    exitType = tr("by hidden 'exit()' call");
                }
            }
            else {
                exitType = tr("by OOM killer");
            }

            finishString = tr("<b><u>Erasing statistics:</u></b><br><br>"
                              "Average speed: <b>%1 MiB/s</b><br>"
                              "Memory erased: <b>%2</b><br>"
                              "Time elapsed: <b>%3 seconds</b><br>"
                              "Exit type: <b>%4</b>").arg(fromDouble(avgSpeed), fileSizeToStr(bytesCount), fromDouble(timeElapsed), exitType);

            progressBar->setValue(100);
            progressBar->repaint();
            statLabel->setText(tr("Finished!"));
        }
    }

    close();
}

void MemEraseDialog::startErasure(bool showError, bool showDialog, int owrType, const QString &owrName)
{
    setWindowTitle(tr("Starting erasing process..."));

    statLabel->setText(tr("Preparing erasing procedure..."));

    cancelButton->setEnabled(false);

    errorString.clear();
    finishString.clear();

    canceled = false;
    exitLock = false;

    imageLabel->setPixmap(QIcon(":/hourglass.png").pixmap(64, 64));

    progressBar->setMaximum(0);
    progressBar->repaint();

    setFixedHeight(sizeHint().height());
    resize(700, height());

    this->owrType = owrType;
    this->owrName = owrName;
    this->showError = showError;

    totalMemSize = getMemTotal();

    saveOvercommitState();
    changeOvercommitState('1');

    bytesCount = 0;

    procBuffer.clear();

    processStart();

    if (showDialog) {
        show();
    }
}
