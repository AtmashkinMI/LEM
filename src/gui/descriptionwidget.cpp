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

#include "memerasedialog.hpp"
#include "fileerasedialog.hpp"
#include "descriptionwidget.hpp"
#include "freespaceerasedialog.hpp"

#include "../srm/systeminfo.hpp"

class FileTreeWidgetItem : public QTreeWidgetItem
{
public:
    FileTreeWidgetItem(QTreeWidget *parent, const QStringList &strings) : QTreeWidgetItem(parent, strings)
    {
    }

private:
    static double getMultiplier(const QString &suffix)
    {
        double ret = 1;

        if (suffix == QObject::tr("B")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("KiB")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("MiB")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("GiB")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("TiB")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("PiB")) {
            return ret;
        }

        ret *= 1024;
        if (suffix == QObject::tr("EiB")) {
            return ret;
        }

        return -1;
    }

    bool operator<(const QTreeWidgetItem &other) const
    {
        int column = treeWidget()->sortColumn();
        if (treeWidget()->headerItem()->text(column) == QObject::tr("Size")) {
            QStringList thisSizeList = text(column).split(' ');
            QStringList otherSizeList = other.text(column).split(' ');

            if (thisSizeList.size() == 2 && otherSizeList.size() == 2) {
                bool thisIsOk, otherIsOk;

                double thisFileSize = thisSizeList.at(0).toDouble(&thisIsOk);
                double otherFileSize = otherSizeList.at(0).toDouble(&otherIsOk);

                double thisMultiplier = getMultiplier(thisSizeList.at(1));
                double otherMultiplier = getMultiplier(otherSizeList.at(1));

                if (thisIsOk && otherIsOk && thisMultiplier > 0 && otherMultiplier > 0) {
                    thisFileSize *= thisMultiplier;
                    otherFileSize *= otherMultiplier;

                    return thisFileSize < otherFileSize;
                }
            }
        }

        return text(column).toLower() < other.text(column).toLower();
    }
};

DescriptionWidget::DescriptionWidget(TaskMap &tasks, QWidget *parent)
    : QWidget(parent), tasks(tasks)
{
    bigFont.setBold(true);
    bigFont.setPointSize(16);

    littleFont.setPointSize(12);

    middleFont.setBold(true);
    middleFont.setPointSize(12);
}

void DescriptionWidget::createRamTree()
{
    filesTree->setColumnCount(2);

    filesTree->setHeaderLabels(QStringList() << tr("Memory file") << tr("Memory size"));

    filesTree->header()->setResizeMode(0, QHeaderView::Stretch);
    filesTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);

    qint64 memTotalSize = MemEraseDialog::getMemTotal();

    QTreeWidgetItem *fileItem = new QTreeWidgetItem(filesTree, QStringList() << tasks[currentTask].filesList.first().fileName << fileSizeToStr(memTotalSize));

    fileItem->setIcon(0, currentTask->icon(0));

    totalSizeLabel->setText(tr("<u>Total size:</u> <b>%1</b>").arg(fileSizeToStr(memTotalSize)));
}

void DescriptionWidget::createFilesTree()
{
    filesTree->setColumnCount(4);

    filesTree->setHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Size") << tr("Delete after"));

    filesTree->header()->setResizeMode(0, QHeaderView::Stretch);
    filesTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(3, QHeaderView::Fixed);

    filesTree->header()->resizeSection(3, fontMetrics().width(filesTree->headerItem()->text(3) + "  "));

    qint64 totalSize = 0;

    FilesList *filesList = &tasks[currentTask].filesList;

    for (int i = 0; i < filesList->size(); ++i) {
        qint64 fileSize;
        if (tasks[currentTask].taskType == Files) {
            fileSize = FileEraseDialog::getFileSize(filesList->at(i).fileName, false, filesList->at(i).deleteAfter, true, false);
        }
        else {
            fileSize = FileEraseDialog::getFileSize(filesList->at(i).fileName, false, true, false, true);
        }

        if (fileSize >= 0) {
            totalSize += fileSize;

            QString deleteAfter = filesList->at(i).deleteAfter ? tr("yes") : tr("no");

            FileTreeWidgetItem *fileItem = new FileTreeWidgetItem(filesTree, QStringList() << filesList->at(i).fileName << FileEraseDialog::fileTypeStr(filesList->at(i).fileName) << fileSizeToStr(fileSize) << deleteAfter);

            fileItem->setIcon(0, FileEraseDialog::fileTypeIcon(filesList->at(i).fileName));
        }
    }

    if (tasks[currentTask].taskType != Files) {
        filesTree->setColumnHidden(3, true);
    }

    totalSizeLabel->setText(tr("<u>Total size:</u> <b>%1</b>").arg(fileSizeToStr(totalSize)));
}

void DescriptionWidget::createUnusedTree()
{
    filesTree->setColumnCount(4);

    filesTree->setHeaderLabels(QStringList() << tr("Mount point") << tr("File system") << tr("Free inodes") << tr("Free blocks"));

    filesTree->header()->setResizeMode(0, QHeaderView::Stretch);
    filesTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);

    qint64 inodeSpace = FreeSpaceEraseDialog::getFreeInodes(tasks[currentTask].filesList.first().fileName);
    qint64 blockSpace = FreeSpaceEraseDialog::getFreeBlocks(tasks[currentTask].filesList.first().fileName);

    if (inodeSpace >= 0 && blockSpace >= 0) {
        QString inodeStr;
        if (tasks[currentTask].fileSystem != "ReiserFS") {
            inodeStr = QString::number(inodeSpace);
        }
        else {
            inodeStr = tr("unspecified");
        }
        QTreeWidgetItem *fileItem = new QTreeWidgetItem(filesTree, QStringList() << tasks[currentTask].filesList.first().fileName << tasks[currentTask].fileSystem << inodeStr << fileSizeToStr(blockSpace));

        fileItem->setIcon(0, currentTask->icon(0));

        fileItem->setCheckState(2, tasks[currentTask].doInodes ? Qt::Checked : Qt::Unchecked);
        fileItem->setCheckState(3, tasks[currentTask].doBlocks ? Qt::Checked : Qt::Unchecked);

        unusedSpacetask = currentTask;

        connect(filesTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(checkUnusedSpace(QTreeWidgetItem *, int)));

        checkUnusedSpace(fileItem, -1);
    }
    else {
        totalSizeLabel->setText(tr("<u>Total size:</u> <b>unknown</b>"));
    }
}

void DescriptionWidget::fillCustomTree()
{
    filesTree->clear();

    qint64 totalSize = 0;

    QList<int> toRemove;
    FilesList *filesList = &tasks[currentTask].filesList;

    for (int i = 0; i < filesList->size(); ++i) {
        qint64 fileSize = FileEraseDialog::getFileSize(filesList->at(i).fileName, false, filesList->at(i).deleteAfter, true, false);

        if (fileSize >= 0) {
            totalSize += fileSize;

            FileTreeWidgetItem *fileItem = new FileTreeWidgetItem(filesTree, QStringList() << filesList->at(i).fileName << FileEraseDialog::fileTypeStr(filesList->at(i).fileName) << fileSizeToStr(fileSize));
            fileItem->setIcon(0, FileEraseDialog::fileTypeIcon(filesList->at(i).fileName));
            if (!FileEraseDialog::isBlockDev(filesList->at(i).fileName)) {
                fileItem->setCheckState(3, filesList->at(i).deleteAfter ? Qt::Checked : Qt::Unchecked);
            }
            else {
                fileItem->setText(3, tr("no"));
            }
        }
        else {
            toRemove << i - toRemove.size();
        }
    }

    for (int i = 0; i < toRemove.size(); ++i) {
        filesList->removeAt(i);
    }

    totalSizeLabel->setText(tr("<u>Total size:</u> <b>%1</b>").arg(fileSizeToStr(totalSize)));
}

void DescriptionWidget::createCustomTree(QVBoxLayout *&buttonsLayout)
{
    filesTree->setColumnCount(4);

    filesTree->setSelectionMode(QTreeWidget::MultiSelection);

    filesTree->setHeaderLabels(QStringList() << tr("Name") << tr("Type") << tr("Size") << tr("Delete after"));

    filesTree->header()->setResizeMode(0, QHeaderView::Stretch);
    filesTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
    filesTree->header()->setResizeMode(3, QHeaderView::Fixed);

    filesTree->header()->resizeSection(3, fontMetrics().width(filesTree->headerItem()->text(3) + "  "));

    fillCustomTree();

    QToolButton *addFilesButton = new QToolButton(this);
    addFilesButton->setIcon(QIcon::fromTheme("document-new"));
    addFilesButton->setStatusTip(tr("Add files to this wipe list"));
    connect(addFilesButton, SIGNAL(clicked()), this, SLOT(addFilesToCustomTree()));

    QToolButton *addDirsButton = new QToolButton(this);
    addDirsButton->setIcon(QIcon::fromTheme("document-open"));
    addDirsButton->setStatusTip(tr("Add directories to this wipe list"));
    connect(addDirsButton, SIGNAL(clicked()), this, SLOT(addDirsToCustomTree()));

    delEntriesButton = new QToolButton(this);
    delEntriesButton->setEnabled(false);
    delEntriesButton->setIcon(QIcon::fromTheme("list-remove"));
    delEntriesButton->setStatusTip(tr("Delete entries from this wipe list"));
    connect(delEntriesButton, SIGNAL(clicked()), this, SLOT(delEntriesFromCustomTree()));

    buttonsLayout = new QVBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(addFilesButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(addDirsButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(delEntriesButton);
    buttonsLayout->addStretch();

    connect(filesTree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));
    connect(filesTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(treeCheckStateChanged(QTreeWidgetItem *, int)));
}

void DescriptionWidget::treeSelectionChanged()
{
    if (filesTree->selectedItems().size() > 0) {
        delEntriesButton->setEnabled(true);
    }
    else {
        delEntriesButton->setEnabled(false);
    }
}

void DescriptionWidget::treeCheckStateChanged(QTreeWidgetItem *item, int column)
{
    if (column != 3) {
        return;
    }

    if (item->data(3, Qt::CheckStateRole) == QVariant()) {
        return;
    }

    qint64 totalSize = 0;

    FilesList &filesList = tasks[currentTask].filesList;

    for (int i = 0; i < filesList.size(); ++i) {
        bool match = filesList.at(i).fileName == item->text(0);
        if (match) {
            filesList[i].deleteAfter = item->checkState(3) == Qt::Checked;
        }

        qint64 fileSize = FileEraseDialog::getFileSize(filesList.at(i).fileName, false, filesList.at(i).deleteAfter, true, false);

        if (fileSize >= 0) {
            totalSize += fileSize;
        }

        if (match) {
            if (fileSize >= 0) {
                item->setText(2, fileSizeToStr(fileSize));
            }
            else {
                item->setText(2, tr("deleted"));
            }
        }
    }

    totalSizeLabel->setText(tr("<u>Total size:</u> <b>%1</b>").arg(fileSizeToStr(totalSize)));
}

void DescriptionWidget::addFilesToCustomTree()
{
    FilesList *filesList = &tasks[currentTask].filesList;

    QFileDialog openFilesDialog(this, tr("Select files you want to erase - %1").arg(qApp->applicationName()), QDir::rootPath());
    openFilesDialog.setViewMode(QFileDialog::List);
    openFilesDialog.setAcceptMode(QFileDialog::AcceptSave);
    openFilesDialog.setFileMode(QFileDialog::AnyFile);
    openFilesDialog.setOption(QFileDialog::DontUseNativeDialog);
    openFilesDialog.setOption(QFileDialog::DontResolveSymlinks);
    openFilesDialog.setOption(QFileDialog::DontConfirmOverwrite);
    openFilesDialog.setFilter(QDir::Dirs | QDir::Files | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);

    QStringList filesToAdd;
    if (openFilesDialog.exec()) {
        filesToAdd = openFilesDialog.selectedFiles();

        FileInfo fileInfo;

        for (int i = 0; i < filesToAdd.size(); ++i) {
            bool inList = false;

            for (int j = 0; j < filesList->size(); ++j) {
                if (filesToAdd.at(i) == filesList->at(j).fileName) {
                    inList = true;
                    break;
                }
            }

            if (!inList) {
                fileInfo.fileName = filesToAdd.at(i);
                fileInfo.deleteAfter = !FileEraseDialog::isBlockDev(fileInfo.fileName);
                filesList->append(fileInfo);
            }
        }

        fillCustomTree();
    }
}

void DescriptionWidget::addDirsToCustomTree()
{
    FilesList *filesList = &tasks[currentTask].filesList;

    QFileDialog openDirsDialog(this, tr("Select directories you want to erase - %1").arg(qApp->applicationName()), QDir::rootPath());
    openDirsDialog.setViewMode(QFileDialog::List);
    openDirsDialog.setFileMode(QFileDialog::Directory);
    openDirsDialog.setAcceptMode(QFileDialog::AcceptSave);
    openDirsDialog.setOption(QFileDialog::DontUseNativeDialog);
    openDirsDialog.setOption(QFileDialog::DontResolveSymlinks);
    openDirsDialog.setOption(QFileDialog::DontConfirmOverwrite);
    openDirsDialog.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);

    QStringList dirsToAdd;
    if (openDirsDialog.exec()) {
        dirsToAdd = openDirsDialog.selectedFiles();

        FileInfo dirInfo;
        dirInfo.deleteAfter = true;

        for (int i = 0; i < dirsToAdd.size(); ++i) {
            bool inList = false;

            for (int j = 0; j < filesList->size(); ++j) {
                if (dirsToAdd.at(i) == filesList->at(j).fileName) {
                    inList = true;
                    break;
                }
            }

            if (!inList) {
                dirInfo.fileName = dirsToAdd.at(i);
                filesList->append(dirInfo);
            }
        }

        fillCustomTree();
    }
}

void DescriptionWidget::delEntriesFromCustomTree()
{
    FilesList *filesList = &tasks[currentTask].filesList;

    QList<QTreeWidgetItem *> selectedEntries = filesTree->selectedItems();

    for (int i = 0; i < selectedEntries.size(); ++i) {
        for (int j = 0; j < filesList->size(); ++j) {
            if (selectedEntries.at(i)->text(0) == filesList->at(j).fileName) {
                filesList->removeAt(j);
                break;
            }
        }
    }

    fillCustomTree();
}

void DescriptionWidget::setChildWindow(QTreeWidgetItem *task)
{
    clearWidget();

    currentTask = task;

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(currentTask->icon(0).pixmap(48, 48));
    imageLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");

    QLabel *titleShortLabel = new QLabel(this);
    titleShortLabel->setText(currentTask->text(0));
    titleShortLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleShortLabel->setFont(bigFont);
    titleShortLabel->setWordWrap(true);
    titleShortLabel->setIndent(10);

    QLabel *titleLongLabel = new QLabel(this);
    titleLongLabel->setText(tasks[currentTask].description);
    titleLongLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleLongLabel->setFont(littleFont);
    titleLongLabel->setWordWrap(true);
    titleLongLabel->setIndent(10);

    filesTree = new QTreeWidget(this);

    filesTree->setSortingEnabled(true);
    filesTree->setRootIsDecorated(false);
    filesTree->setAlternatingRowColors(true);

    filesTree->setFocusPolicy(Qt::NoFocus);
    filesTree->setTextElideMode(Qt::ElideLeft);
    filesTree->sortByColumn(0, Qt::AscendingOrder);
    filesTree->setSelectionMode(QTreeWidget::NoSelection);

    filesTree->header()->setMovable(false);
    filesTree->header()->setStretchLastSection(false);

    totalSizeLabel = new QLabel(this);
    totalSizeLabel->setFont(littleFont);
    totalSizeLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");

    QVBoxLayout *buttonsLayout = 0;

    switch (tasks[currentTask].taskType) {
        case Ram:
            createRamTree();
            break;
        case Disk:
        case Swap:
        case Files:
            createFilesTree();
            break;
        case Unused:
            createUnusedTree();
            break;
        case Custom:
            createCustomTree(buttonsLayout);
            break;
    }

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(imageLabel);
    titleLayout->addWidget(titleShortLabel, 1);

    QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addLayout(titleLayout);
    topLayout->addWidget(titleLongLabel);
    topLayout->setSpacing(0);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(totalSizeLabel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(20);

    if (buttonsLayout) {
        QVBoxLayout *treeLayout = new QVBoxLayout;
        treeLayout->addWidget(filesTree);
        treeLayout->addLayout(bottomLayout);

        QHBoxLayout *middleLayout = new QHBoxLayout;
        middleLayout->addLayout(treeLayout);
        middleLayout->addLayout(buttonsLayout);
        mainLayout->addLayout(middleLayout);
    }
    else {
        mainLayout->addWidget(filesTree);
        mainLayout->addLayout(bottomLayout);
    }

    setLayout(mainLayout);

    setStatusTip(tr("Details of selected erasing task"));
}

void DescriptionWidget::setParentWindow(QTreeWidgetItem *task)
{
    clearWidget();

    buttonsMap.clear();

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(task->icon(0).pixmap(48, 48));
    imageLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");

    QLabel *titleShortLabel = new QLabel(this);
    titleShortLabel->setText(task->text(0));
    titleShortLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleShortLabel->setFont(bigFont);
    titleShortLabel->setWordWrap(true);
    titleShortLabel->setIndent(10);

    QLabel *titleLongLabel = new QLabel(this);
    titleLongLabel->setText(tasks[task].description);
    titleLongLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleLongLabel->setFont(littleFont);
    titleLongLabel->setWordWrap(true);
    titleLongLabel->setIndent(10);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(imageLabel);
    titleLayout->addWidget(titleShortLabel, 1);

    QVBoxLayout *topLayout = new QVBoxLayout;
    topLayout->addLayout(titleLayout);
    topLayout->addWidget(titleLongLabel);
    topLayout->setSpacing(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);

    if (task->data(0, Qt::CheckStateRole) == QVariant()) {
        QLabel *customEmptyLabel = new QLabel(this);
        customEmptyLabel->setText(tr("No lists available!"));
        customEmptyLabel->setAlignment(Qt::AlignCenter);
        customEmptyLabel->setFont(middleFont);
        customEmptyLabel->setWordWrap(true);

        mainLayout->addStretch();
        mainLayout->addWidget(customEmptyLabel);
    }
    else {
        mainLayout->addSpacing(10);

        for (int i = 0; i < task->childCount(); ++i) {
            QTreeWidgetItem *childTask = task->child(i);

            QLabel *childTitleLabel = new QLabel(this);
            childTitleLabel->setText(childTask->text(0));
            childTitleLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
            childTitleLabel->setFont(middleFont);
            childTitleLabel->setWordWrap(true);

            QLabel *childImage = new QLabel(this);
            childImage->setPixmap(childTask->icon(0).pixmap(32, 32));
            childImage->setFixedWidth(childImage->sizeHint().width());

            QLabel *childDescLabel = new QLabel(this);
            childDescLabel->setText(tasks[childTask].description);
            childDescLabel->setWordWrap(true);

            QPushButton *detailsButton = new QPushButton(QIcon::fromTheme("dialog-information"), tr("Details..."), this);
            buttonsMap[detailsButton] = childTask;
            connect(detailsButton, SIGNAL(clicked()), this, SLOT(selectItem()));

            QHBoxLayout *childDescLayout = new QHBoxLayout;
            childDescLayout->addWidget(childImage);
            childDescLayout->addWidget(childDescLabel, 1);
            childDescLayout->addWidget(detailsButton);

            QVBoxLayout *childTitleLayout = new QVBoxLayout;
            childTitleLayout->addWidget(childTitleLabel);
            childTitleLayout->addLayout(childDescLayout);

            QHBoxLayout *childLayout = new QHBoxLayout;
            childLayout->addSpacing(10);
            childLayout->addLayout(childTitleLayout);

            mainLayout->addSpacing(10);
            mainLayout->addLayout(childLayout);
        }
    }

    mainLayout->addStretch();

    setLayout(mainLayout);

    setStatusTip(tr("Check details of required erasing tasks"));
}

void DescriptionWidget::setApplicationWindow()
{
    clearWidget();

    QLabel *imageLabel = new QLabel(this);
    imageLabel->setPixmap(qApp->windowIcon().pixmap(128, 128));
    imageLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");

    QLabel *titleShortLabel = new QLabel(this);
    titleShortLabel->setText(tr("%1 (%2)").arg(qApp->organizationName(), qApp->applicationName()));
    titleShortLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleShortLabel->setFont(bigFont);
    titleShortLabel->setWordWrap(true);
    titleShortLabel->setIndent(10);

    QLabel *titleLongLabel = new QLabel(this);
    titleLongLabel->setText(tr("%1 is a program that allows you to completely remove all traces of your online and offline activities and securely erase files and other data that may contain confidential information!").arg(qApp->applicationName()));
    titleLongLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLongLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    titleLongLabel->setFont(littleFont);
    titleLongLabel->setWordWrap(true);
    titleLongLabel->setIndent(10);

    QLabel *featuresShortLabel = new QLabel(this);
    featuresShortLabel->setText(tr("Using this program, you can wipe and remove:"));
    featuresShortLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    featuresShortLabel->setFont(middleFont);
    featuresShortLabel->setWordWrap(true);

    QLabel *featuresLongLabel = new QLabel(this);
    featuresLongLabel->setText(tr("• Selected files and directories on disks (also directly from your favorite file manager);\n"
                                  "• Items in Trash can (also directly from your favorite file manager);\n"
                                  "• Application specific data (cache, history, logs, passwords, temporary files, recent documents)."));
    featuresLongLabel->setFont(littleFont);
    featuresLongLabel->setWordWrap(true);
    featuresLongLabel->setIndent(30);

    QLabel *alsoShortLabel = new QLabel(this);
    alsoShortLabel->setText(tr("You also can:"));
    alsoShortLabel->setStyleSheet("QLabel { background-color : palette(highlight); }");
    alsoShortLabel->setFont(middleFont);
    alsoShortLabel->setWordWrap(true);

    QLabel *alsoLongLabel = new QLabel(this);
    alsoLongLabel->setText(tr("• Wipe unused block and inode space on mounted file systems (ext2/3/4, XFS, JFS and ReiserFS are supported);\n"
                              "• Safely erase unused RAM, swap partitions and files;\n"
                              "• Select either fast or strong erasing scheme (20 schemes are supported);\n"
                              "• Create wipe lists (lists of files to wipe);\n"
                              "• Erase entire disks (floppy, hard, flash) and partitions on them."));
    alsoLongLabel->setFont(littleFont);
    alsoLongLabel->setWordWrap(true);
    alsoLongLabel->setIndent(30);

    QVBoxLayout *titleLayout = new QVBoxLayout;
    titleLayout->addWidget(titleShortLabel);
    titleLayout->addWidget(titleLongLabel, 1);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(imageLabel);
    topLayout->addLayout(titleLayout, 1);
    topLayout->setSpacing(0);

    QHBoxLayout *featuresShortLayout = new QHBoxLayout;
    featuresShortLayout->addStretch(1);
    featuresShortLayout->addWidget(featuresShortLabel, 9);

    QHBoxLayout *featuresLongLayout = new QHBoxLayout;
    featuresLongLayout->addStretch(1);
    featuresLongLayout->addWidget(featuresLongLabel, 9);

    QHBoxLayout *alsoShortLayout = new QHBoxLayout;
    alsoShortLayout->addStretch(1);
    alsoShortLayout->addWidget(alsoShortLabel, 9);

    QHBoxLayout *alsoLongLayout = new QHBoxLayout;
    alsoLongLayout->addStretch(1);
    alsoLongLayout->addWidget(alsoLongLabel, 9);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(featuresShortLayout);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(featuresLongLayout);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(alsoShortLayout);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(alsoLongLayout);
    mainLayout->addStretch(1);

    setLayout(mainLayout);

    setStatusTip(tr("%1's main features").arg(qApp->applicationName()));
}

void DescriptionWidget::clearWidget()
{
    clearLayout(layout());
}

void DescriptionWidget::clearLayout(QLayout *layout)
{
    if (layout) {
        while (QLayoutItem *childItem = layout->takeAt(0)) {
            if (QLayout *childLayout = childItem->layout()) {
                clearLayout(childLayout);
            }
            else {
                delete childItem->widget();
            }
        }

        delete layout;
    }
}

void DescriptionWidget::selectItem()
{
    QTreeWidgetItem *task = buttonsMap[(QPushButton *) sender()];

    task->parent()->setExpanded(true);

    task->treeWidget()->setCurrentItem(task);

    task->setSelected(true);
}

void DescriptionWidget::checkUnusedSpace(QTreeWidgetItem *item, int column)
{
    if (column == 2) {
        tasks[unusedSpacetask].doInodes = item->checkState(column) == Qt::Checked;
    }

    if (column == 3) {
        tasks[unusedSpacetask].doBlocks = item->checkState(column) == Qt::Checked;
    }

    if (!tasks[unusedSpacetask].doInodes && !tasks[unusedSpacetask].doBlocks) {
        item->setCheckState(2, Qt::Checked);
        item->setCheckState(3, Qt::Checked);
    }

    QString sizeString;

    if (tasks[unusedSpacetask].doInodes & tasks[unusedSpacetask].doBlocks) {
        sizeString = tr("<b>%1 inodes</b> + <b>%2</b>").arg(item->text(2), item->text(3));
    }
    else {
        if (tasks[unusedSpacetask].doInodes) {
            sizeString = tr("<b>%1 inodes</b>").arg(item->text(2));
        }
        else {
            sizeString = tr("<b>%1</b>").arg(item->text(3));
        }
    }

    totalSizeLabel->setText(tr("<u>Total size:</u> %1").arg(sizeString));
}
