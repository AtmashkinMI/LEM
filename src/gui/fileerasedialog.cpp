#include <QtGui>

#include "fileerasedialog.hpp"
#include "../srm/fileeraser.hpp"
#include "../srm/systeminfo.hpp"

FileEraseDialog::FileEraseDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    statLabel = new QLabel(this);

    currentProgress = new QProgressBar(this);
    currentProgress->setFormat(tr("Current progress: %p%"));

    totalProgress = new QProgressBar(this);
    totalProgress->setFormat(tr("Erasing progress: %p%"));

    cancelButton = new QPushButton(QIcon::fromTheme("window-close"), tr("&Cancel"), this);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addStretch();
    rightLayout->addWidget(statLabel);
    rightLayout->addWidget(currentProgress);
    rightLayout->addWidget(totalProgress);
    rightLayout->addStretch();
    rightLayout->addLayout(buttonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);

    canceled = false;
    exitLock = false;

    eraseThread = new FileEraser(this);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(stopErasure()));
    connect(eraseThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(eraseThread, SIGNAL(finishErasePart()), this, SLOT(finishErasePart()));
    connect(eraseThread, SIGNAL(renamingRound(int)), this, SLOT(renamingRound(int)));
    connect(eraseThread, SIGNAL(totalProgressSignal(int)), totalProgress, SLOT(setValue(int)));
    connect(eraseThread, SIGNAL(fileProgressSignal(int)), currentProgress, SLOT(setValue(int)));
    connect(eraseThread, SIGNAL(updateStat(double, double, int)), this, SLOT(changeStatLabel(double, double, int)));
    connect(eraseThread, SIGNAL(erasingFile(const QString &, int, bool)), this, SLOT(changeFormType(const QString &, int, bool)), Qt::BlockingQueuedConnection);
}

QIcon FileEraseDialog::fileTypeIcon(const QString &fileName)
{
    return FileEraser::fileTypeIcon(fileName);
}

QString FileEraseDialog::fileTypeStr(const QString &fileName)
{
    return FileEraser::fileTypeStr(fileName);
}

qint64 FileEraseDialog::getFileSize(const QString &fileName)
{
    return FileEraser::getFileSize(fileName);
}

QString FileEraseDialog::fromDouble(double num)
{
    return QString::number(num, 'f', 2);
}

void FileEraseDialog::closeEvent(QCloseEvent *event)
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

void FileEraseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void FileEraseDialog::stopErasure()
{
    if (!exitLock) {
        canceled = true;
        exitLock = true;
        cancelButton->setEnabled(false);
        eraseThread->stopThread();
    }
}

void FileEraseDialog::changeFormType(const QString &fileName, int type, bool firstTime)
{
    switch (type) {
        case 0:
            setWindowTitle(tr("Erasing %1 '%2'").arg(fileTypeStr(fileName), fileName));
            statLabel->setText(tr("Starting erasing procedure..."));
            break;
        case 1:
            setWindowTitle(tr("Erasing name of %1 '%2'").arg(fileTypeStr(fileName), fileName));
            statLabel->setText(tr("Preparing renaming..."));
            break;
        case 2:
            setWindowTitle(tr("Erasing bytes of %1 '%2' [%3]").arg(fileTypeStr(fileName), fileName, owrName));
            statLabel->setText(tr("Calculating stats..."));
            break;
    }

    if (firstTime) {
        imageLabel->setPixmap(fileTypeIcon(fileName).pixmap(64, 64));

        if (currentProgress->maximum() != 100) {
            currentProgress->setMaximum(100);
        }

        currentProgress->setValue(0);

        currentProgress->repaint();

        if (totalProgress->maximum() != 100) {
            totalProgress->setMaximum(100);

            totalProgress->setValue(0);

            totalProgress->repaint();
        }
    }

    leftPart = 100 - currentProgress->value();
}

void FileEraseDialog::changeStatLabel(double speed, double time, int round)
{
    QString statString = tr("Average speed [round %1/%2]: %3 KiB/s").arg(QString::number(round + 1), QString::number(roundsNumber), fromDouble(speed));
    if (time >= 0.01) {
        statString += tr(" (%1 seconds remaining)").arg(fromDouble(time));
    }

    statLabel->setText(statString);
}

void FileEraseDialog::renamingRound(int round)
{
    if (round < roundsNumber) {
        statLabel->setText(tr("Renaming... [round %1/%2]").arg(QString::number(round + 1), QString::number(roundsNumber)));
    }
    else {
        statLabel->setText(tr("Zeroizing attributes..."));
    }

    if (round > 0) {
        int step = leftPart / (roundsNumber + 1);
        if (step > 0) {
            int newProgress = currentProgress->value() + step;
            if (newProgress <= 99) {
                currentProgress->setValue(newProgress);
                currentProgress->repaint();
            }
        }
    }
}

void FileEraseDialog::finishErasePart()
{
    statLabel->setText(tr("Finished!"));

    currentProgress->setValue(100);

    currentProgress->repaint();
}

void FileEraseDialog::threadFinished()
{
    errorString = eraseThread->getLastError();

    if (!canceled || !errorString.isEmpty()) {
        exitLock = true;
        cancelButton->setEnabled(false);

        if (!errorString.isEmpty()) {
            canceled = true;

            imageLabel->setPixmap(QIcon::fromTheme("dialog-error").pixmap(64, 64));

            currentProgress->setValue(0);
            currentProgress->repaint();

            totalProgress->setValue(0);
            totalProgress->repaint();

            statLabel->setText(tr("Error!"));

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
            finishString = eraseThread->getFinishString();

            double timeElapsed = totalClock.elapsed();
            if (timeElapsed <= 0) {
                timeElapsed = 1;
            }
            timeElapsed /= 1000;

            finishString += tr("Time elapsed: <b>%1 seconds</b>").arg(fromDouble(timeElapsed));

            totalProgress->setValue(100);
            totalProgress->repaint();
        }
    }

    close();
}

QString FileEraseDialog::getErrorString()
{
    return errorString;
}

QString FileEraseDialog::getFinishString()
{
    return finishString;
}

FilesList FileEraseDialog::getTrashList(const QString &userName)
{
    FileInfo fileInfo;
    FilesList fileInfoList;

    fileInfo.deleteAfter = true;

    QString homeDir = getUserHome(userName);

    QDir trashDir(QString("%1/.local/share/Trash").arg(homeDir));
    trashDir.setFilter(QDir::Files | QDir::Dirs | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);

    QString infoDir = QString("%1/.local/share/Trash/info").arg(homeDir);
    QString filesDir = QString("%1/.local/share/Trash/files").arg(homeDir);

    QFileInfoList trashList = trashDir.entryInfoList();

    for (int i = 0; i < trashList.size(); ++i) {
        fileInfo.fileName = trashList.at(i).absoluteFilePath();

        if (fileInfo.fileName != infoDir && fileInfo.fileName != filesDir) {
            fileInfoList << fileInfo;
        }
    }

    fileInfo.deleteAfter = false;

    fileInfo.fileName = infoDir;
    fileInfoList << fileInfo;

    fileInfo.fileName = filesDir;
    fileInfoList << fileInfo;

    return fileInfoList;
}

void FileEraseDialog::startErasure(bool showError, bool showDialog, int owrType, const QString &owrName, const FilesList &filesList)
{
    setWindowTitle(tr("Starting erasing thread..."));

    statLabel->setText(tr("Preparing erasing procedure..."));

    cancelButton->setEnabled(true);
    cancelButton->setFocus();

    errorString.clear();
    finishString.clear();

    canceled = false;
    exitLock = false;

    imageLabel->setPixmap(QIcon(":/hourglass.png").pixmap(64, 64));

    totalProgress->setMaximum(0);
    totalProgress->repaint();

    currentProgress->setMaximum(0);
    currentProgress->repaint();

    setFixedHeight(sizeHint().height());
    resize(700, height());

    this->owrName = owrName;
    this->showError = showError;

    eraseThread->init(owrType, filesList);
    errorString = eraseThread->getLastError();
    if (!errorString.isEmpty()) {
        if (showError) {
            QMessageBox errorMessage(this);
            errorMessage.setTextFormat(Qt::PlainText);
            errorMessage.setIcon(QMessageBox::Critical);
            errorMessage.addButton(QMessageBox::Ok);
            errorMessage.setWindowTitle(QObject::tr("Error!"));
            errorMessage.setText(errorString);
            errorMessage.exec();
        }

        emit stopped();

        return;
    }

    roundsNumber = eraseThread->getRounds();

    totalClock.start();

    eraseThread->start();

    if (showDialog) {
        show();
    }
}
