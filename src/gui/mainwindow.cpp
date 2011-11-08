#include <QtGui>
#include <QDomDocument>

#include "mainwindow.hpp"
#include "aboutdialog.hpp"
#include "reportdialog.hpp"
#include "memerasedialog.hpp"
#include "settingsdialog.hpp"
#include "shutdowndialog.hpp"
#include "fileerasedialog.hpp"
#include "descriptionwidget.hpp"
#include "freespaceerasedialog.hpp"

MainWindow::MainWindow(const QString &currentUserName)
{
    startAct = 0;
    trayIcon = 0;
    shutdownDialog = 0;

    currentTaskType = -1;

    showErrorReport = false;
    showNormalReport = false;

    aboutDialog = new AboutDialog(this);
    reportDialog = new ReportDialog(this);
    settingsDialog = new SettingsDialog(this);

    tasksTree = new QTreeWidget(this);
    tasksTree->setColumnCount(2);
    tasksTree->setHeaderHidden(true);
    tasksTree->setTextElideMode(Qt::ElideNone);
    tasksTree->setContextMenuPolicy(Qt::CustomContextMenu);
    tasksTree->setStatusTip(tr("Select required erasing tasks and then press 'Start' button"));

    tasksTree->header()->setStretchLastSection(false);
    tasksTree->header()->setResizeMode(0, QHeaderView::Stretch);
    tasksTree->header()->setResizeMode(1, QHeaderView::Fixed);

    connect(tasksTree, SIGNAL(itemSelectionChanged()), this, SLOT(treeSelectionChanged()));
    connect(tasksTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(itemCheckStateChanged(QTreeWidgetItem *, int)));
    connect(tasksTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeContextMenu(const QPoint &)));

    descriptionWidget = new DescriptionWidget(tasks, this);

    descriptionScroll = new QScrollArea(this);
    descriptionScroll->setWidgetResizable(true);
    descriptionScroll->setBackgroundRole(QPalette::Base);
    descriptionScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    descriptionScroll->setWidget(descriptionWidget);

    rebuildTree();

    splitter = new QSplitter(this);
    splitter->addWidget(tasksTree);
    splitter->addWidget(descriptionScroll);
    splitter->setChildrenCollapsible(false);

    setCentralWidget(splitter);

    memEraseDialog = new MemEraseDialog;
    fileEraseDialog = new FileEraseDialog;
    freeSpaceEraseDialog = new FreeSpaceEraseDialog;

    errorReport = new QMessageBox(this);
    errorReport->addButton(QMessageBox::Ok);
    errorReport->setTextFormat(Qt::RichText);
    errorReport->setWindowTitle(tr("Error!"));
    errorReport->setIcon(QMessageBox::Critical);
    errorReport->setWindowModality(Qt::NonModal);
    errorReport->setWindowFlags(errorReport->windowFlags() ^ Qt::WindowContextHelpButtonHint);

    connect(errorReport, SIGNAL(finished(int)), this, SLOT(reportClosed()), Qt::QueuedConnection);

    connect(reportDialog, SIGNAL(save()), this, SLOT(saveReport()));
    connect(reportDialog, SIGNAL(closed()), this, SLOT(reportClosed()), Qt::QueuedConnection);

    connect(memEraseDialog, SIGNAL(stopped()), this, SLOT(dialogStopped()), Qt::QueuedConnection);
    connect(memEraseDialog, SIGNAL(finished()), this, SLOT(dialogFinished()), Qt::QueuedConnection);

    connect(fileEraseDialog, SIGNAL(stopped()), this, SLOT(dialogStopped()), Qt::QueuedConnection);
    connect(fileEraseDialog, SIGNAL(finished()), this, SLOT(dialogFinished()), Qt::QueuedConnection);

    connect(freeSpaceEraseDialog, SIGNAL(stopped()), this, SLOT(dialogStopped()), Qt::QueuedConnection);
    connect(freeSpaceEraseDialog, SIGNAL(finished()), this, SLOT(dialogFinished()), Qt::QueuedConnection);

    createActions();
    createMenus();
    createToolBars();

    this->currentUserName = currentUserName;

    if (QSystemTrayIcon::isSystemTrayAvailable() && QSystemTrayIcon::supportsMessages()) {
        createTrayIcon();

        qApp->setQuitOnLastWindowClosed(false);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon) {
        hide();

        switch (currentTaskType) {
            case 0:
                memEraseDialog->hide();
                break;
            case 1:
                fileEraseDialog->hide();
                break;
            case 2:
                freeSpaceEraseDialog->hide();
                break;
        }

        if (showErrorReport) {
            errorReport->hide();
        }
        if (showNormalReport) {
            reportDialog->hide();
        }

        event->ignore();
    }
    else {
        event->accept();
    }
}

void MainWindow::setDefaultTrayTip()
{
    if (trayIcon) {
        trayIcon->setToolTip(tr("Ready for new erasing tasks"));
    }
}

void MainWindow::createTrayIcon()
{
    QMenu *trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(startAct);
    trayIconMenu->addAction(stopAct);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(powerOffAct);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(exitAct);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(qApp->windowIcon());
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

    setDefaultTrayTip();

    trayIcon->show();
}

void MainWindow::trayClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            if (!isVisible()) {
                setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                show();

                switch (currentTaskType) {
                    case 0:
                        memEraseDialog->setWindowState((memEraseDialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                        memEraseDialog->show();
                        break;
                    case 1:
                        fileEraseDialog->setWindowState((fileEraseDialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                        fileEraseDialog->show();
                        break;
                    case 2:
                        freeSpaceEraseDialog->setWindowState((freeSpaceEraseDialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                        freeSpaceEraseDialog->show();
                        break;
                }

                if (showErrorReport) {
                    errorReport->setWindowState((errorReport->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                    errorReport->show();
                }
                if (showNormalReport) {
                    reportDialog->setWindowState((reportDialog->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
                    reportDialog->show();
                }
            }
            else {
                close();
            }
            break;
        default:
            break;
        }
}

void MainWindow::createActions()
{
    startAct = new QAction(QIcon::fromTheme("media-playback-start"), tr("&Start"), this);
    startAct->setShortcut(QKeySequence(tr("F2")));
    startAct->setStatusTip(tr("Start erasing procedure"));
    startAct->setEnabled(false);
    connect(startAct, SIGNAL(triggered()), this, SLOT(startErasure()));

    stopAct = new QAction(QIcon::fromTheme("media-playback-stop"), tr("S&top"), this);
    stopAct->setShortcut(QKeySequence(tr("F3")));
    stopAct->setStatusTip(tr("Stop erasing procedure"));
    stopAct->setEnabled(false);
    connect(stopAct, SIGNAL(triggered()), this, SLOT(stopErasure()));

    reloadAct = new QAction(QIcon::fromTheme("view-refresh"), tr("&Reload"), this);
    reloadAct->setShortcut(QKeySequence::Refresh);
    reloadAct->setStatusTip(tr("Refresh %1's task tree").arg(qApp->applicationName()));
    connect(reloadAct, SIGNAL(triggered()), this, SLOT(rebuildTree()));

    exitAct = new QAction(QIcon::fromTheme("application-exit"), tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the %1").arg(qApp->applicationName()));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(quit()));

    powerOffAct = new QAction(QIcon::fromTheme("system-shutdown"), tr("&Power off"), this);
    powerOffAct->setCheckable(true);
    powerOffAct->setShortcut(QKeySequence(tr("Ctrl+P")));
    powerOffAct->setStatusTip(tr("Power off system when all tasks are ended"));

    settingsAct = new QAction(QIcon::fromTheme("preferences-system"), tr("&Settings..."), this);
    settingsAct->setShortcut(QKeySequence(tr("Ctrl+X")));
    settingsAct->setStatusTip(tr("Show the %1's settings window").arg(qApp->applicationName()));
    connect(settingsAct, SIGNAL(triggered()), settingsDialog, SLOT(show()));

    contentsAct = new QAction(QIcon::fromTheme("help-contents"), tr("&Contents..."), this);
    contentsAct->setShortcut(QKeySequence::HelpContents);
    contentsAct->setStatusTip(tr("Show the %1's help contents").arg(qApp->applicationName()));
    connect(contentsAct, SIGNAL(triggered()), this, SLOT(showContents()));

    aboutQtAct = new QAction(QIcon(":/trolltech/qmessagebox/images/qtlogo-64.png"), tr("About &Qt..."), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's about window"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutAct = new QAction(qApp->windowIcon(), tr("&About %1...").arg(qApp->applicationName()), this);
    aboutAct->setStatusTip(tr("Show the %1's about window").arg(qApp->applicationName()));
    connect(aboutAct, SIGNAL(triggered()), aboutDialog, SLOT(show()));

    selectAllAct = new QAction(QIcon(":/select.png"), tr("&Select all"), this);
    selectAllAct->setShortcut(QKeySequence(tr("Ctrl+S")));
    selectAllAct->setStatusTip(tr("Select all available tasks"));
    connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAllTasks()));

    unselectAllAct = new QAction(QIcon(":/unselect.png"), tr("&Unselect all"), this);
    unselectAllAct->setShortcut(QKeySequence(tr("Ctrl+U")));
    unselectAllAct->setStatusTip(tr("Unselect all available tasks"));
    connect(unselectAllAct, SIGNAL(triggered()), this, SLOT(unselectAllTasks()));

    expandAllAct = new QAction(QIcon(":/expand.png"), tr("&Expand all"), this);
    expandAllAct->setShortcut(QKeySequence(tr("Ctrl+E")));
    expandAllAct->setStatusTip(tr("Expand all available parent tasks"));
    connect(expandAllAct, SIGNAL(triggered()), tasksTree, SLOT(expandAll()));

    collapseAllAct = new QAction(QIcon(":/collapse.png"), tr("&Collapse all"), this);
    collapseAllAct->setShortcut(QKeySequence(tr("Ctrl+C")));
    collapseAllAct->setStatusTip(tr("Collapse all available parent tasks"));
    connect(collapseAllAct, SIGNAL(triggered()), tasksTree, SLOT(collapseAll()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(startAct);
    fileMenu->addAction(stopAct);
    fileMenu->addSeparator();
    fileMenu->addAction(reloadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(powerOffAct);
    toolsMenu->addSeparator();
    toolsMenu->addAction(settingsAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(contentsAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutAct);

    contextMenu = new QMenu(this);
    contextMenu->addAction(selectAllAct);
    contextMenu->addAction(unselectAllAct);
    contextMenu->addSeparator();
    contextMenu->addAction(expandAllAct);
    contextMenu->addAction(collapseAllAct);

    tasksTree->addAction(selectAllAct);
    tasksTree->addAction(unselectAllAct);
    tasksTree->addAction(expandAllAct);
    tasksTree->addAction(collapseAllAct);
}

void MainWindow::createToolBars()
{
    mainToolBar = new QToolBar(this);

    mainToolBar->addAction(startAct);
    mainToolBar->addAction(stopAct);
    mainToolBar->addAction(reloadAct);
    mainToolBar->addAction(settingsAct);

    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(32, 32));
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    addToolBar(mainToolBar);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

QMenu *MainWindow::createPopupMenu()
{
    return 0;
}

bool MainWindow::openPdf(const QString &filePath)
{
    if (checkRunningKDE()) {
        return QProcess::startDetached(QString("gksu -u %1 \"okular %2\"").arg(currentUserName, filePath));
    }
    else {
        return QProcess::startDetached(QString("gksu -u %1 \"evince %2\"").arg(currentUserName, filePath));
    }
}

void MainWindow::showContents()
{
    bool isOk = false;

    QFileInfo docCheck;

    QString docDir = QString("/usr/share/%1/doc/").arg(qApp->applicationName());

    QString langName = SettingsDialog::getLangName();
    if (langName != "en") {
        QString translatedDoc = QString("%1manual_%2.pdf").arg(docDir, langName);

        docCheck.setFile(translatedDoc);

        isOk = docCheck.exists() && openPdf(translatedDoc);
    }

    if (!isOk) {
        QString defaultDoc = docDir + "manual.pdf";

        docCheck.setFile(defaultDoc);

        isOk = docCheck.exists() && openPdf(defaultDoc);
    }

    if (!isOk) {
        QMessageBox::critical(this, tr("Error!"), tr("Can't open PDF file with documentation!"));
    }
}

void MainWindow::itemCheckStateChanged(QTreeWidgetItem *item, int column)
{
    QList<QTreeWidgetItem *> selectedItems = tasksTree->selectedItems();

    if (!selectedItems.isEmpty()) {
        QTreeWidgetItem *selectedTask = selectedItems.first();

        if (selectedTask == item) {
            if (!selectedTask->parent() || selectedTask->childCount() > 0) {
                descriptionWidget->setParentWindow(selectedTask);
            }
            else {
                descriptionWidget->setChildWindow(selectedTask);
            }
        }
    }

    if (item->childCount() > 0) {
        Qt::CheckState checkState = item->checkState(column);

        if (checkState != Qt::PartiallyChecked) {
            for (int i = 0; i < item->childCount(); ++i) {
                item->child(i)->setCheckState(column, checkState);
            }
        }
    }

    if (item->parent() != 0) {
        int checkCount = 0;
        int partCheckCount = 0;

        for (int i = 0; i < item->parent()->childCount(); ++i) {
            switch (item->parent()->child(i)->checkState(column)) {
                case Qt::Checked:
                    ++checkCount;
                    break;
                case Qt::PartiallyChecked:
                    ++partCheckCount;
                    break;
                default:
                    break;
            }
        }

        if (checkCount == 0 && partCheckCount == 0) {
            item->parent()->setCheckState(column, Qt::Unchecked);
        }
        else {
            if (checkCount == item->parent()->childCount()) {
                item->parent()->setCheckState(column, Qt::Checked);
            }
            else {
                item->parent()->setCheckState(column, Qt::PartiallyChecked);
            }
        }
    }

    enableStartAct();
}

void MainWindow::enableStartAct()
{
    if (findFirstSelectedTask()) {
        if (startAct) {
            startAct->setEnabled(true);
        }
    }
    else {
        if (startAct) {
            startAct->setEnabled(false);
        }
    }
}

QTreeWidgetItem *MainWindow::addNewTask(const QString &taskName, const QIcon &taskIcon, const TaskInfo &taskInfo, QTreeWidgetItem *parentTask)
{
    QTreeWidgetItem *newTask;

    if (parentTask) {
        newTask = new QTreeWidgetItem(parentTask);
    }
    else {
        newTask = new QTreeWidgetItem(tasksTree);
    }

    newTask->setText(0, taskName);
    newTask->setCheckState(0, Qt::Unchecked);
    newTask->setIcon(0, taskIcon);

    tasks[newTask] = taskInfo;

    return newTask;
}

void MainWindow::treeSelectionChanged()
{
    QList<QTreeWidgetItem *> selectedItems = tasksTree->selectedItems();

    if (!selectedItems.isEmpty()) {
        QTreeWidgetItem *selectedTask = selectedItems.first();

        if (!selectedTask->parent() || selectedTask->childCount() > 0) {
            descriptionWidget->setParentWindow(selectedTask);
        }
        else {
            descriptionWidget->setChildWindow(selectedTask);
        }
    }
}

void MainWindow::addDisksTask()
{
    TaskInfo taskInfo;

    QStringList devList = getBlockDevices();

    if (!devList.isEmpty()) {
        taskInfo.taskType = Disk;
        taskInfo.description = tr("You may erase block devices, i.e. disks and partitions on them");

        QTreeWidgetItem *blockDevTask = addNewTask(tr("Disks & Partitions"), QIcon(":/hard_drive.png"), taskInfo);

        FileInfo fileInfo;
        fileInfo.deleteAfter = false;

        for (int i = 0; i < devList.size(); ++i) {
            fileInfo.fileName = devList.at(i);

            taskInfo.filesList.clear();
            taskInfo.filesList << fileInfo;
            taskInfo.description = tr("You may erase this %1").arg(FileEraseDialog::fileTypeStr(devList.at(i)));

            addNewTask(devList.at(i), FileEraseDialog::fileTypeIcon(devList.at(i)), taskInfo, blockDevTask);
        }
    }
}

void MainWindow::addCustomTask()
{
    TaskInfo taskInfo;
    taskInfo.description = tr("You may create your own lists of files you want to erase");

    customTask = addNewTask(tr("Wipe Lists"), QIcon(":/wipe_task.png"), taskInfo);
    customTask->setData(0, Qt::CheckStateRole, QVariant());

    addTaskButton = new QToolButton(this);
    addTaskButton->setIcon(QIcon::fromTheme("list-add"));
    addTaskButton->setStatusTip(tr("Add new wipe list"));
    connect(addTaskButton, SIGNAL(clicked()), this, SLOT(createCustomTask()));

    tasksTree->setItemWidget(customTask, 1, addTaskButton);
    tasksTree->header()->resizeSection(1, addTaskButton->minimumSizeHint().width());
}

QString MainWindow::genListName()
{
    QString pattern = tr("<New List%1>");

    QString listName = pattern.arg("");

    int tail = 1;

    forever {
        bool unique = true;

        for (int i = 0; i < customTask->childCount(); ++i) {
            if (listName == customTask->child(i)->text(0)) {
                unique = false;
                listName = pattern.arg(tr(" %1").arg(tail++));
                break;
            }
        }

        if (unique) {
            return listName;
        }
    }
}

void MainWindow::createCustomTask()
{
    TaskInfo taskInfo;
    taskInfo.taskType = Custom;
    taskInfo.description = tr("Custom list of files to erase");

    QTreeWidgetItem *newListTask = addNewTask(genListName(), QIcon(":/wipe_list.png"), taskInfo, customTask);
    newListTask->setFlags(newListTask->flags() | Qt::ItemIsEditable);

    QToolButton *delTaskButton = new QToolButton(this);
    delTaskButton->setIcon(QIcon::fromTheme("list-remove"));
    delTaskButton->setStatusTip(tr("Delete this wipe list"));
    buttonsMap[delTaskButton] = newListTask;
    connect(delTaskButton, SIGNAL(clicked()), this, SLOT(deleteCustomTask()));

    tasksTree->setItemWidget(newListTask, 1, delTaskButton);

    customTask->setExpanded(true);

    QList<QTreeWidgetItem *> selectedItems = tasksTree->selectedItems();

    if (!selectedItems.isEmpty() && selectedItems.first() == customTask) {
        descriptionWidget->setParentWindow(customTask);
    }
}

void MainWindow::deleteCustomTask()
{
    QTreeWidgetItem *task = buttonsMap.take((QToolButton *) sender());
    task->setCheckState(0, Qt::Unchecked);

    customTask->removeChild(task);
    delete task;

    if (customTask->childCount() == 0) {
        customTask->setData(0, Qt::CheckStateRole, QVariant());
    }
    else {
        itemCheckStateChanged(customTask->child(0), 0);
    }

    QList<QTreeWidgetItem *> selectedItems = tasksTree->selectedItems();

    if (!selectedItems.isEmpty() && selectedItems.first() == customTask) {
        descriptionWidget->setParentWindow(customTask);
    }
}

void MainWindow::addUnusedTask()
{
    TaskInfo taskInfo;

    QStringList mountPoints = getMountPoints();

    if (!mountPoints.isEmpty()) {
        taskInfo.doBlocks = true;
        taskInfo.doInodes = true;
        taskInfo.taskType = Unused;
        taskInfo.description = tr("You may erase unused space on mounted file systems");

        QTreeWidgetItem *unusedSpaceTask = addNewTask(tr("Unused space"), QIcon(":/unused_space.png"), taskInfo);

        FileInfo fileInfo;

        for (int i = 0; i < mountPoints.size() / 2; ++i) {
            fileInfo.fileName = mountPoints.at(2 * i);

            taskInfo.filesList.clear();
            taskInfo.filesList << fileInfo;

            taskInfo.fileSystem = mountPoints.at(2 * i + 1);
            taskInfo.description = tr("You may erase unused inode and block space on this '%1' file system mounted on '%2' directory").arg(taskInfo.fileSystem, fileInfo.fileName);

            addNewTask(tr("%1 [%2]").arg(fileInfo.fileName, taskInfo.fileSystem), QIcon(":/mount_point.png"), taskInfo, unusedSpaceTask);
        }
    }
}

bool MainWindow::addTmpEntries(const QString &dirPath, const QStringList &whiteList, const QList<QRegExp> &whiteListRegExp, FilesList &filesList)
{
    bool delDir = true;

    FileInfo fileInfo;
    fileInfo.deleteAfter = true;

    QDir tmpDir(dirPath);
    tmpDir.setFilter(QDir::Files | QDir::Dirs | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);

    QStringList toDelList;
    QFileInfoList tmpFilesList = tmpDir.entryInfoList();

    for (int i = 0; i < tmpFilesList.size(); ++i) {
        QString filePath = tmpFilesList.at(i).absoluteFilePath();

        bool match = false;

        for (int j = 0; j < whiteListRegExp.size(); ++j) {
            if (whiteListRegExp.at(j).exactMatch(filePath)) {
                match = true;
                break;
            }
        }

        if (match) {
            delDir = false;
            continue;
        }

        if (whiteList.indexOf(filePath) != -1) {
            delDir = false;
            continue;
        }

        if (tmpFilesList.at(i).isDir()) {
            if (addTmpEntries(filePath, whiteList, whiteListRegExp, filesList)) {
                toDelList << filePath;
            }
            else {
               delDir = false;
            }
        }
        else {
            if (!tmpFilesList.at(i).isFile()) {
                delDir = false;
            }
            else {
                toDelList << filePath;
            }
        }
    }

    if (!delDir) {
        for (int i = 0; i < toDelList.size(); ++i) {
            fileInfo.fileName = toDelList.at(i);

            filesList << fileInfo;
        }
    }

    return delDir;
}

void MainWindow::addPkgEntries(const QString &dirPath, const QStringList &whiteList, FilesList &filesList)
{
    QDir pkgDir(dirPath);
    pkgDir.setFilter(QDir::Files | QDir::NoSymLinks);

    FileInfo fileInfo;
    fileInfo.deleteAfter = true;

    QFileInfoList pkgsList = pkgDir.entryInfoList();
    for (int i = 0; i < pkgsList.size(); ++i) {
        fileInfo.fileName = pkgsList.at(i).absoluteFilePath();
        if (whiteList.indexOf(fileInfo.fileName) == -1) {
            filesList << fileInfo;
        }
    }
}

void MainWindow::addSystemTask()
{
    TaskInfo taskInfo;

    taskInfo.description = tr("You may select global erasing tasks that are related to all users");

    QTreeWidgetItem *systemTask = addNewTask(tr("System"), QIcon::fromTheme("computer"), taskInfo);

    FileInfo fileInfo;

    taskInfo.taskType = Ram;

    fileInfo.fileName = "/dev/mem";
    taskInfo.filesList << fileInfo;

    taskInfo.description = tr("You may erase free physical memory (RAM)");

    addNewTask(tr("Physical memory"), QIcon(":/ram.png"), taskInfo, systemTask);

    swapList = getSwapFiles();

    if (!swapList.isEmpty()) {
        taskInfo.taskType = Swap;
        taskInfo.description = tr("You may erase swap memory resided on partitions and files");

        fileInfo.deleteAfter = false;

        taskInfo.filesList.clear();

        for (int i = 0; i < swapList.size(); ++i) {
            fileInfo.fileName = swapList.at(i).swapPath;

            taskInfo.filesList << fileInfo;
        }

        addNewTask(tr("Swap memory"), QIcon(":/swap.png"), taskInfo, systemTask);
    }

    taskInfo.taskType = Files;

    taskInfo.filesList.clear();

    fileInfo.deleteAfter = true;

    QStringList oldLogsFilter;
    oldLogsFilter << "*.[0-9]" << "*.gz" << "*.old";

    QDir logDir("/var/log");
    logDir.setFilter(QDir::Files | QDir::NoSymLinks);
    logDir.setNameFilters(oldLogsFilter);

    QFileInfoList rootLogs = logDir.entryInfoList();
    for (int i = 0; i < rootLogs.size(); ++i) {
        fileInfo.fileName = rootLogs.at(i).absoluteFilePath();

        taskInfo.filesList << fileInfo;
    }

    logDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    logDir.setNameFilters(QStringList());

    QFileInfoList rootDirs = logDir.entryInfoList();
    for (int i = 0; i < rootDirs.size(); ++i) {
        QDir rootDir(rootDirs.at(i).absoluteFilePath());
        rootDir.setFilter(QDir::Files | QDir::NoSymLinks);
        rootDir.setNameFilters(oldLogsFilter);

        QFileInfoList dirLogs = rootDir.entryInfoList();
        for (int j = 0; j < dirLogs.size(); ++j) {
            fileInfo.fileName = dirLogs.at(j).absoluteFilePath();

            taskInfo.filesList << fileInfo;
        }
    }

    if (!taskInfo.filesList.isEmpty()) {
        taskInfo.description = tr("You may erase old system log files");
        addNewTask(tr("Old log files"), QIcon(QString("/usr/share/%1/icons/log_file.png").arg(qApp->applicationName())), taskInfo, systemTask);
    }

    QRegExp pidRegExp("/proc/\\d+");
    QString fdPattern("%1/fd");

    QDir procDir("/proc");
    procDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList whiteList;

    QDirIterator procDirIterator(procDir);
    while (procDirIterator.hasNext()) {
        QString fileName = procDirIterator.next();
        if (pidRegExp.exactMatch(fileName)) {
            QDir fdDir(fdPattern.arg(fileName));
            fdDir.setFilter(QDir::Files);

            QFileInfoList fdList = fdDir.entryInfoList();
            for (int i = 0; i < fdList.size(); ++i) {
                whiteList << fdList.at(i).symLinkTarget();
            }
        }
    }

    QList<QRegExp> whiteListRegExp;
    whiteListRegExp << QRegExp("/tmp/.*pid.*")
                    << QRegExp("/tmp/.*lock.*");

    if (checkRunningKDE()) {
        whiteListRegExp << QRegExp("/var/tmp/kdecache-.*");
    }

    taskInfo.filesList.clear();

    addTmpEntries("/tmp", whiteList, whiteListRegExp, taskInfo.filesList);
    addTmpEntries("/var/tmp", whiteList, whiteListRegExp, taskInfo.filesList);

    if (!taskInfo.filesList.isEmpty()) {
        taskInfo.description = tr("You may erase temporary system files");
        addNewTask(tr("Temporary files"), QIcon(":/temp.png"), taskInfo, systemTask);
    }

    whiteList.clear();

    TaskInfo archTask;
    addPkgEntries("/var/cache/pacman/pkg", whiteList, archTask.filesList);

    TaskInfo mandrivaTask;
    addPkgEntries("/var/cache/urpmi/rpms", whiteList, mandrivaTask.filesList);
    addPkgEntries("/var/cache/urpmi/headers", whiteList, mandrivaTask.filesList);
    addPkgEntries("/var/cache/urpmi/partial", whiteList, mandrivaTask.filesList);

    TaskInfo ubuntuTask;

    whiteList << "/var/cache/apt/archives/lock";

    addPkgEntries("/var/cache/apt/archives", whiteList, ubuntuTask.filesList);
    addPkgEntries("/var/cache/apt/archives/partial", whiteList, ubuntuTask.filesList);

    if (!archTask.filesList.isEmpty() || !mandrivaTask.filesList.isEmpty() || !ubuntuTask.filesList.isEmpty()) {
        taskInfo.description = tr("You may erase cache of your favorite package manager");
        QTreeWidgetItem *packageTask = addNewTask(tr("Package managers"), QIcon(":/package_manager.png"), taskInfo, systemTask);

        QString pkgDesc = tr("You may erase cached and partially downloaded packages of '%1' package manager");

        if (!ubuntuTask.filesList.isEmpty()) {
            QString pkgName = tr("APT");

            ubuntuTask.taskType = Files;
            ubuntuTask.description = pkgDesc.arg(pkgName);
            addNewTask(pkgName, QIcon(":/ubuntu.svg"), ubuntuTask, packageTask);
        }

        if (!archTask.filesList.isEmpty()) {
            QString pkgName = tr("Pacman");

            archTask.taskType = Files;
            archTask.description = pkgDesc.arg(pkgName);
            addNewTask(pkgName, QIcon(":/archlinux.svg"), archTask, packageTask);
        }

        if (!mandrivaTask.filesList.isEmpty()) {
            QString pkgName = tr("Urpmi");

            mandrivaTask.taskType = Files;
            mandrivaTask.description = pkgDesc.arg(pkgName);
            addNewTask(pkgName, QIcon(":/mandriva.svg"), mandrivaTask, packageTask);
        }
    }
}

void MainWindow::parseActionTag(const QDomElement &actionTag, const QString &userName, FilesList &filesList)
{
    QList<QRegExp> whiteList;

    QString homeDir = getUserHome(userName);

    for (QDomElement tag = actionTag.firstChildElement(); !tag.isNull(); tag = tag.nextSiblingElement()) {
        if (tag.tagName() == "skip") {
            QString wildCard = tag.attribute("wildCard");
            if (wildCard.isEmpty()) {
                return;
            }

            QRegExp regExp(wildCard, Qt::CaseSensitive, QRegExp::Wildcard);
            if (regExp.isValid()) {
                whiteList << regExp;
            }
        }
    }

    bool deleteAfter = true;
    bool deleteSubDirs = true;
    bool deleteSubFiles = true;

    if (actionTag.attribute("deleteAfter") == "false") {
        deleteAfter = false;

        if (actionTag.attribute("deleteSubDirs") == "false") {
            deleteSubDirs = false;

            if (actionTag.attribute("deleteSubFiles") == "false") {
                deleteSubFiles = false;
            }
        }
    }

    QString wildCard = actionTag.attribute("wildCard");
    if (wildCard.isEmpty()) {
        return;
    }

    QRegExp regExp(wildCard, Qt::CaseSensitive, QRegExp::Wildcard);
    if (!regExp.isValid()) {
        return;
    }

    QDir parentDir;

    QString parentDirPath = actionTag.attribute("parentDir");
    if (parentDirPath.isEmpty()) {
        parentDir.setPath(homeDir);
    }
    else {
        parentDir.setPath(QString("%1/%2").arg(homeDir, parentDirPath));
    }

    if (!parentDir.exists()) {
        return;
    }

    parentDir.setFilter(QDir::Files | QDir::Dirs | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot);
    QStringList parentDirList = parentDir.entryList();

    FileInfo fileInfo;
    QFileInfo fileCheck;

    for (int i = 0; i < parentDirList.size(); ++i) {
        QString fileName = parentDirList.at(i);
        fileCheck.setFile(QString("%1/%2").arg(parentDir.absolutePath(), fileName));

        if (!regExp.exactMatch(fileName)) {
            continue;
        }

        bool skip = false;
        for (int j = 0; j < whiteList.size(); ++j) {
            if (whiteList.at(j).exactMatch(fileName)) {
                skip = true;
                break;
            }
        }
        if (skip) {
            continue;
        }

        if (fileCheck.isDir()) {
            if (deleteAfter) {
                fileInfo.deleteAfter = true;
                fileInfo.fileName = fileCheck.absoluteFilePath();

                filesList << fileInfo;
            }
            else {
                if (deleteSubDirs) {
                    fileInfo.deleteAfter = false;
                    fileInfo.fileName = fileCheck.absoluteFilePath();

                    filesList << fileInfo;
                }
                else {
                    fileInfo.deleteAfter = deleteSubFiles;

                    QDirIterator dirIterator(fileCheck.absoluteFilePath(), QDir::Files | QDir::Dirs | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
                    while (dirIterator.hasNext()) {
                        fileCheck.setFile(dirIterator.next());
                        if (!fileCheck.isDir()) {
                            fileInfo.fileName = fileCheck.absoluteFilePath();

                            filesList << fileInfo;
                        }
                    }
                }
            }
        }
        else {
            fileInfo.deleteAfter = deleteAfter;
            fileInfo.fileName = fileCheck.absoluteFilePath();

            filesList << fileInfo;
        }
    }
}

UserTask MainWindow::parseTaskTag(const QDomElement &taskTag, const QString &userName)
{
    UserTask userTask;
    userTask.taskInfo.taskType = Files;

    QString defaultName;
    QString defaultDescription;

    QString langName = SettingsDialog::getLangName();

    for (QDomElement tag = taskTag.firstChildElement(); !tag.isNull(); tag = tag.nextSiblingElement()) {
        QString tagName = tag.tagName();
        if (tagName == "name") {
            QString tagLang = tag.attribute("lang", "en");

            if (tagLang == "en" && defaultName.isEmpty()) {
                defaultName = tag.text();
            }

            if (tagLang == langName && userTask.taskName.isEmpty()) {
                userTask.taskName = tag.text();
            }
        }
        else if (tagName == "description") {
            QString tagLang = tag.attribute("lang", "en");

            if (tagLang == "en" && defaultDescription.isEmpty()) {
                defaultDescription = tag.text();
            }

            if (tagLang == langName && userTask.taskInfo.description.isEmpty()) {
                userTask.taskInfo.description = tag.text();
            }
        }
        else if (tagName == "icon") {
            if (userTask.taskIcon.isNull()) {
                if (tag.attribute("source") == "theme") {
                    userTask.taskIcon = QIcon::fromTheme(tag.text());
                }
                else {
                    userTask.taskIcon.addFile(QString("/usr/share/%1/icons/%2").arg(qApp->applicationName(), tag.text()));
                }
            }
        }
        else if (tagName == "action") {
            parseActionTag(tag, userName, userTask.taskInfo.filesList);
        }
    }

    return userTask;
}

QList<UserProfile> MainWindow::getProfilesList(const QString &userName)
{    
    QList<UserProfile> profilesList;

    QString langName = SettingsDialog::getLangName();

    QFileInfoList filesList = QDir(QString("/usr/share/%1/profiles").arg(qApp->applicationName())).entryInfoList(QStringList("*.xml"), QDir::Files, QDir::Name);
    for (int i = 0; i < filesList.size(); ++i) {
        QFile xmlFile(filesList.at(i).absoluteFilePath());
        if (!xmlFile.open(QFile::ReadOnly)) {
            continue;
        }

        QDomDocument xmlDoc;
        if (!xmlDoc.setContent(&xmlFile)) {
            continue;
        }

        xmlFile.close();

        QDomElement profileElement = xmlDoc.firstChildElement("profile");
        if (profileElement.isNull()) {
            continue;
        }

        QString defaultName;
        QString defaultDescription;

        UserProfile userProfile;

        for (QDomElement tag = profileElement.firstChildElement(); !tag.isNull(); tag = tag.nextSiblingElement()) {
            QString tagName = tag.tagName();
            if (tagName == "name") {
                QString tagLang = tag.attribute("lang", "en");

                if (tagLang == "en" && defaultName.isEmpty()) {
                    defaultName = tag.text();
                }

                if (tagLang == langName && userProfile.parentTask.taskName.isEmpty()) {
                    userProfile.parentTask.taskName = tag.text();
                }
            }
            else if (tagName == "description") {
                QString tagLang = tag.attribute("lang", "en");

                if (tagLang == "en" && defaultDescription.isEmpty()) {
                    defaultDescription = tag.text();
                }

                if (tagLang == langName && userProfile.parentTask.taskInfo.description.isEmpty()) {
                    userProfile.parentTask.taskInfo.description = tag.text();
                }
            }
            else if (tagName == "icon") {
                if (userProfile.parentTask.taskIcon.isNull()) {
                    if (tag.attribute("source") == "theme") {
                        userProfile.parentTask.taskIcon = QIcon::fromTheme(tag.text());
                    }
                    else {
                        userProfile.parentTask.taskIcon.addFile(QString("/usr/share/%1/icons/%2").arg(qApp->applicationName(), tag.text()));
                    }
                }
            }
            else if (tagName == "task") {
                UserTask userTask = parseTaskTag(tag, userName);

                if (!userTask.taskInfo.filesList.isEmpty() && !userTask.taskName.isEmpty() &&
                    !userTask.taskIcon.isNull() && !userTask.taskInfo.description.isEmpty()) {
                    userProfile.childTaks << userTask;
                }
            }
        }

        if (userProfile.parentTask.taskName.isEmpty()) {
            userProfile.parentTask.taskName = defaultName;
        }

        if (userProfile.parentTask.taskInfo.description.isEmpty()) {
            userProfile.parentTask.taskInfo.description = defaultDescription;
        }

        if (!userProfile.childTaks.isEmpty() && !userProfile.parentTask.taskName.isEmpty() &&
            !userProfile.parentTask.taskIcon.isNull() && !userProfile.parentTask.taskInfo.description.isEmpty()) {
            profilesList << userProfile;
        }
    }

    return profilesList;
}

void MainWindow::addUsersTask()
{
    QMap<QString, QList<UserProfile> > usersMap;

    QStringList usersList = getUsersList();

    bool isEmpty = true;

    for (int i = 0; i < usersList.size(); ++i) {
        QString userName = usersList.at(i);
        usersMap[userName] = getProfilesList(userName);

        if (!usersMap[userName].isEmpty()) {
            isEmpty = false;
        }
    }

    if (!isEmpty) {
        TaskInfo usersTaskInfo;
        usersTaskInfo.description = tr("You may select local erasing tasks that are related to a particular user");

        QTreeWidgetItem *usersTask = addNewTask(tr("Users"), QIcon(":/users.png"), usersTaskInfo);

        for (int i = 0; i < usersList.size(); ++i) {
            QString userName = usersList.at(i);
            QList<UserProfile> profilesList = usersMap[userName];

            if (profilesList.isEmpty()) {
                continue;
            }

            QString homeDir = getUserHome(userName);

            QIcon userIcon;
            if (QFileInfo(QString("%1/.face").arg(homeDir)).exists()) {
                userIcon.addFile(QString("%1/.face").arg(homeDir));
            }
            else if (QFileInfo(QString("%1/.face.icon").arg(homeDir)).exists()) {
                userIcon.addFile(QString("%1/.face.icon").arg(homeDir));
            }
            else {
                if (userName == "root") {
                    userIcon.addFile(":/root.png");
                }
                else {
                    userIcon.addFile(":/user.png");
                }
            }

            TaskInfo userTaskInfo;
            userTaskInfo.description = tr("You may erase confidential application data stored in '%1'").arg(homeDir);

            QTreeWidgetItem *userTask = addNewTask(userName, userIcon, userTaskInfo, usersTask);

            for (int j = 0; j < profilesList.size(); ++j) {
                UserTask profileTask = profilesList.at(j).parentTask;
                QList<UserTask> childTaks = profilesList.at(j).childTaks;

                QTreeWidgetItem *parentTask = addNewTask(profileTask.taskName, profileTask.taskIcon, profileTask.taskInfo, userTask);

                for (int k = 0; k < childTaks.size(); ++k) {
                    UserTask childTask = childTaks.at(k);

                    addNewTask(childTask.taskName, childTask.taskIcon, childTask.taskInfo, parentTask);
                }
            }
        }
    }
}

void MainWindow::rebuildTree()
{
    tasks.clear();
    tasksTree->clear();

    buttonsMap.clear();

    addDisksTask();
    addUnusedTask();
    addSystemTask();
    addUsersTask();
    addCustomTask();

    descriptionWidget->setApplicationWindow();

    createStatusBar();
}

QTreeWidgetItem *MainWindow::findFirstSelectedTask(QTreeWidgetItem *root)
{
    if (!root) {
        for (int i = 0; i < tasksTree->topLevelItemCount(); ++i) {
            QTreeWidgetItem *result = findFirstSelectedTask(tasksTree->topLevelItem(i));
            if (result) {
                return result;
            }
        }
    }
    else {
        if (root->parent() && root->childCount() == 0 && root->data(0, Qt::CheckStateRole) != QVariant() && root->checkState(0) == Qt::Checked) {
            return root;
        }

        for (int i = 0; i < root->childCount(); ++i) {
            QTreeWidgetItem *result = findFirstSelectedTask(root->child(i));
            if (result) {
                return result;
            }
        }
    }

    return 0;
}

void MainWindow::clearFilesList(FilesList *filesList)
{
    QList<int> toRemove;

    for (int i = 0; i < filesList->size(); ++i) {
        if (FileEraseDialog::getFileSize(filesList->at(i).fileName) < 0) {
            toRemove << i - toRemove.size();
        }
    }

    for (int i = 0; i < toRemove.size(); ++i) {
        filesList->removeAt(i);
    }
}

void MainWindow::dialogStopped()
{
    QTreeWidgetItem *stoppedTask = findFirstSelectedTask();
    if (!stoppedTask) {
        return;
    }

    QString errorString;

    QObject *senderDialog = sender();

    if (senderDialog == (QObject *) memEraseDialog)  {
        errorString = memEraseDialog->getErrorString();
    }
    else if (senderDialog == (QObject *) fileEraseDialog) {
        errorString = fileEraseDialog->getErrorString();
    }
    else if (senderDialog == (QObject *) freeSpaceEraseDialog) {
        errorString = freeSpaceEraseDialog->getErrorString();
    }

    TaskType taskType = tasks[stoppedTask].taskType;

    if (taskType == Swap) {
        if (swapDisableError) {
            errorString = tr("Can't disable all swap devices and files!");
        }
        else {
            if (!swapEnableError) {
                numList = restoreSwapFiles(swapList);
                if (!numList.isEmpty()) {
                    swapEnableError = true;
                }
            }

            if (swapEnableError) {
                errorString = tr("Can&#39;t restore swap on following devices and files:");
                for (int i = 0; i < numList.size(); ++i) {
                    QString swapName = swapList.at(numList.at(i)).swapPath;
                    stringToHTML(swapName);

                    errorString += tr("<br>&#8226; %1").arg(swapName);
                }
            }
        }
    }
    else if (taskType == Files || taskType == Custom) {
        clearFilesList(&tasks[stoppedTask].filesList);
    }

    if (!errorString.isEmpty()) {
        reportString += tr("<b><u><font color=\"red\">Fail</font></u></b><br><br>");

        if (!swapEnableError) {
            stringToHTML(errorString);
        }
        reportString += tr("<b><u>Error message:</u></b><br><br>%1").arg(errorString);

        if (settingsDialog->getStopOnError()) {
            if (!settingsDialog->getShowStat()) {
                errorReport->setText(errorString);
                showErrorReport = true;
                if (isVisible()) {
                    errorReport->show();
                }
            }

            if (trayIcon && settingsDialog->getShowTrayMessages()) {
                trayIcon->showMessage(tr("Error!"), tr("Failed task: %1").arg(taskName), QSystemTrayIcon::Critical);
            }

            allTasksFinished();
        }
        else {
            stoppedTask->setCheckState(0, Qt::Unchecked);

            if (trayIcon && settingsDialog->getShowTrayMessages()) {
                trayIcon->showMessage(tr("Warning!"), tr("Skipped task: %1").arg(taskName), QSystemTrayIcon::Warning);
            }

            processTask();
        }
    }
    else {
        reportString += tr("<b><u><font color=\"green\">Cancel</font></u></b>");

        if (trayIcon && settingsDialog->getShowTrayMessages()) {
            trayIcon->showMessage(tr("Cancel!"), tr("Stopped task: %1").arg(taskName), QSystemTrayIcon::Information);
        }

        allTasksFinished();
    }
}

void MainWindow::dialogFinished()
{
    QTreeWidgetItem *finishedTask = findFirstSelectedTask();
    if (!finishedTask) {
        return;
    }

    QString finishString;

    QObject *senderDialog = sender();

    if (senderDialog == (QObject *) memEraseDialog)  {
        finishString = memEraseDialog->getFinishString();
    }
    else if (senderDialog == (QObject *) fileEraseDialog) {
        finishString = fileEraseDialog->getFinishString();
    }
    else if (senderDialog == (QObject *) freeSpaceEraseDialog) {
        finishString = freeSpaceEraseDialog->getFinishString();
    }

    TaskType taskType = tasks[finishedTask].taskType;

    if (taskType == Swap) {
        numList = restoreSwapFiles(swapList);
        if (!numList.isEmpty()) {
            swapEnableError = true;
            dialogStopped();
            return;
        }
    }
    else if (taskType == Files || taskType == Custom) {
        clearFilesList(&tasks[finishedTask].filesList);
    }

    finishedTask->setCheckState(0, Qt::Unchecked);

    reportString += tr("<b><u><font color=\"blue\">Success</font></u></b><br><br>");

    reportString += finishString;

    if (trayIcon && settingsDialog->getShowTrayMessages()) {
        trayIcon->showMessage(tr("Success!"), tr("Finished task: %1").arg(taskName), QSystemTrayIcon::Information);
    }

    processTask();
}

bool MainWindow::disableAllSwaps()
{
    bool retCode = ::disableAllSwaps();
    if (!retCode) {
        swapDisableError = true;
        dialogStopped();
    }

    return retCode;
}

void MainWindow::stringToHTML(QString &string)
{
    string.replace('&', "&amp;");
    string.replace('\"', "&quot;");
    string.replace('\'', "&#39;");
    string.replace('<', "&lt;");
    string.replace('>', "&gt;");
}

void MainWindow::processTask()
{
    numList.clear();

    swapEnableError = false;
    swapDisableError = false;

    QTreeWidgetItem *selectedTask = findFirstSelectedTask();
    if (!selectedTask) {
        allTasksFinished();
        return;
    }

    int owrType = settingsDialog->getOwrType();
    QString owrName = settingsDialog->getOwrName(owrType);

    taskName = tr("\"%1\"").arg(selectedTask->text(0));

    QString taskNameHTML = taskName;
    stringToHTML(taskNameHTML);

    for (QTreeWidgetItem *currentTask = selectedTask->parent(); currentTask; currentTask = currentTask->parent()) {
        QString parentName = tr("\"%1\"").arg(currentTask->text(0));

        QString parentNameHTML = parentName;
        stringToHTML(parentNameHTML);

        taskName.prepend(tr("%1 → ").arg(parentName));
        taskNameHTML.prepend(tr("%1 &rarr; ").arg(parentNameHTML));
    }

    reportString += tr("<HR>Task name: <b>%1</b><br>Result: ").arg(taskNameHTML);

    if (trayIcon) {
        trayIcon->setToolTip(tr("Current task: %1").arg(taskName));
    }

    switch (tasks[selectedTask].taskType) {
        case Ram:
            currentTaskType = 0;
            memEraseDialog->startErasure(false, isVisible(), owrType, owrName);
            break;
        case Swap:
            if (disableAllSwaps()) {
                currentTaskType = 1;
                fileEraseDialog->startErasure(false, isVisible(), owrType, owrName, tasks[selectedTask].filesList);
            }
            break;
        case Disk:
        case Files:
        case Custom:
            currentTaskType = 1;
            fileEraseDialog->startErasure(false, isVisible(), owrType, owrName, tasks[selectedTask].filesList);
            break;
        case Unused:
            currentTaskType = 2;
            freeSpaceEraseDialog->startErasure(false, isVisible(), tasks[selectedTask].doInodes, tasks[selectedTask].doBlocks, tasks[selectedTask].filesList.first().fileName, owrType, owrName);
            break;
    }
}

void MainWindow::saveReport()
{
    QFileDialog saveReportDialog(this, tr("Save report as - %1").arg(qApp->applicationName()), QDir::rootPath(), tr("HTML Files (*.htm *.html)"));
    saveReportDialog.setViewMode(QFileDialog::List);
    saveReportDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveReportDialog.setOption(QFileDialog::DontUseNativeDialog);
    saveReportDialog.setDefaultSuffix(tr("html"));
    saveReportDialog.selectFile(tr("Report-%1.html").arg(QDateTime::currentDateTime().toString(tr("dd-MM-yyyy"))));
    if (saveReportDialog.exec()) {
        QStringList selectedFiles = saveReportDialog.selectedFiles();
        if (selectedFiles.size() == 1) {
            QString reportName = selectedFiles.first();
            QFile reportFile(reportName);
            if (reportFile.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream(&reportFile) << tr("<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /></head>") << reportString;
            }
            else {
                QMessageBox errorMessage;
                errorMessage.setTextFormat(Qt::PlainText);
                errorMessage.setIcon(QMessageBox::Critical);
                errorMessage.addButton(QMessageBox::Ok);
                errorMessage.setWindowTitle(tr("Error!"));
                errorMessage.setText(tr("Can't save report to file '%1'!").arg(reportName));
                errorMessage.exec();
            }
        }
    }
}

void MainWindow::allTasksFinished()
{
    unlockWindow(true);

    currentTaskType = -1;

    setDefaultTrayTip();

    if (settingsDialog->getShowStat()) {
        QString endTime = QDateTime::currentDateTime().toString();
        stringToHTML(endTime);
        reportString += tr("<HR>End time: <b>%1</b><HR>").arg(endTime);

        showNormalReport = true;

        reportDialog->showReport(reportString, isVisible());
    }

    if (!showErrorReport && !showNormalReport) {
        tasksTree->setEnabled(true);
        enableStartAct();
    }

    if (powerOffAct->isChecked()) {
        if (shutdownDialog) {
            delete shutdownDialog;
        }

        shutdownDialog = new ShutdownDialog(this);
        shutdownDialog->startCount(settingsDialog->getShutdownAfter());
    }
}

void MainWindow::startErasure()
{
    currentTaskType = -1;

    showErrorReport = false;
    showNormalReport = false;

    setDefaultTrayTip();

    reportString.clear();

    reportString += tr("<HR><p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                       "<span style=\" font-size:14pt; font-weight:600; text-decoration: underline; color:#0000ff\">REPORT</span></p>");

    QString startTime = QDateTime::currentDateTime().toString();
    stringToHTML(startTime);

    QString owrName = settingsDialog->getOwrName(settingsDialog->getOwrType());
    stringToHTML(owrName);

    reportString += tr("<HR>Start time: <b>%1</b><br>Erasing scheme: <b>%2</b>").arg(startTime, owrName);

    startAct->setEnabled(false);
    tasksTree->setEnabled(false);

    unlockWindow(false);

    processTask();
}

void MainWindow::stopErasure()
{
    switch (currentTaskType) {
        case 0:
            memEraseDialog->close();
            break;
        case 1:
            fileEraseDialog->close();
            break;
        case 2:
            freeSpaceEraseDialog->close();
            break;
    }
}

void MainWindow::selectAllTasks()
{
    for (int i = 0; i < tasksTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *task = tasksTree->topLevelItem(i);

        if (task->data(0, Qt::CheckStateRole) != QVariant()) {
            task->setCheckState(0, Qt::Checked);
        }
    }
}

void MainWindow::unselectAllTasks()
{
    for (int i = 0; i < tasksTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *task = tasksTree->topLevelItem(i);

        if (task->data(0, Qt::CheckStateRole) != QVariant()) {
            task->setCheckState(0, Qt::Unchecked);
        }
    }
}

void MainWindow::unlockWindow(bool state)
{
    stopAct->setDisabled(state);
    reloadAct->setEnabled(state);
    settingsAct->setEnabled(state);
    helpMenu->setEnabled(state);

    if (!state) {
        QList<QTreeWidgetItem *> selectedItems = tasksTree->selectedItems();
        for (int i = 0; i < selectedItems.size(); ++i) {
            selectedItems.at(i)->setSelected(false);
        }

        descriptionWidget->setApplicationWindow();
    }
}

void MainWindow::reportClosed()
{
    showErrorReport = false;
    showNormalReport = false;

    tasksTree->setEnabled(true);
    enableStartAct();
}

void MainWindow::treeContextMenu(const QPoint &pos)
{
    contextMenu->exec(tasksTree->mapToGlobal(pos));
}
