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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QDomElement>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include "taskinfo.hpp"

#include "../srm/systeminfo.hpp"

class QSplitter;
class AboutDialog;
class QMessageBox;
class QScrollArea;
class QToolButton;
class QTreeWidget;
class ReportDialog;
class MemEraseDialog;
class SettingsDialog;
class ShutdownDialog;
class FileEraseDialog;
class DescriptionWidget;
class FreeSpaceEraseDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &currentUserName);

private:
    void addDisksTask();
    void addUsersTask();
    void addCustomTask();
    void addUnusedTask();
    void addSystemTask();

    void createMenus();
    void createActions();
    void createToolBars();
    void createTrayIcon();
    void createStatusBar();

    void processTask();
    void enableStartAct();
    bool disableAllSwaps();
    void allTasksFinished();

    void setDefaultTrayTip();

    QString genListName();

    QMenu *createPopupMenu();

    void unlockWindow(bool state);

    void stringToHTML(QString &string);

    void closeEvent(QCloseEvent *event);

    bool openPdf(const QString &filePath);

    void clearFilesList(FilesList *filesList);

    QList<UserProfile> getProfilesList(const QString &userName);

    QTreeWidgetItem *findFirstSelectedTask(QTreeWidgetItem *root = 0);

    UserTask parseTaskTag(const QDomElement &taskTag, const QString &userName);

    void addPkgEntries(const QString &dirPath, const QStringList &whiteList, FilesList &filesList);

    void parseActionTag(const QDomElement &actionTag, const QString &userName, FilesList &filesList);

    bool addTmpEntries(const QString &dirPath, const QStringList &whiteList, const QList<QRegExp> &whiteListRegExp, FilesList &filesList);

    QTreeWidgetItem *addNewTask(const QString &taskName, const QIcon &taskIcon, const TaskInfo &taskInfo, QTreeWidgetItem *parentTask = 0);

    TaskMap tasks;

    QList<int> numList;
    QList<SwapInfo> swapList;

    qint64 freedCount;
    qint64 freedCurrent;

    int currentTaskType;

    bool showErrorReport;
    bool showNormalReport;

    bool swapEnableError;
    bool swapDisableError;

    QString taskName;
    QString reportString;
    QString currentUserName;

    QMenu *fileMenu;
    QMenu *helpMenu;
    QMenu *toolsMenu;
    QMenu *contextMenu;

    QAction *exitAct;
    QAction *stopAct;
    QAction *aboutAct;
    QAction *startAct;
    QAction *reloadAct;
    QAction *aboutQtAct;
    QAction *contentsAct;
    QAction *powerOffAct;
    QAction *settingsAct;
    QAction *expandAllAct;
    QAction *selectAllAct;
    QAction *collapseAllAct;
    QAction *unselectAllAct;

    QSplitter *splitter;

    QToolBar *mainToolBar;

    QTreeWidget *tasksTree;

    AboutDialog *aboutDialog;

    QMessageBox *errorReport;

    QSystemTrayIcon *trayIcon;

    QToolButton *addTaskButton;

    ReportDialog *reportDialog;

    QTreeWidgetItem *customTask;

    QScrollArea *descriptionScroll;

    MemEraseDialog *memEraseDialog;

    SettingsDialog *settingsDialog;

    ShutdownDialog *shutdownDialog;

    FileEraseDialog *fileEraseDialog;

    DescriptionWidget *descriptionWidget;

    FreeSpaceEraseDialog *freeSpaceEraseDialog;

    QMap<QToolButton *, QTreeWidgetItem *> buttonsMap;

private slots:
    void saveReport();
    void rebuildTree();
    void stopErasure();
    void reportClosed();
    void showContents();
    void startErasure();
    void dialogStopped();
    void dialogFinished();
    void selectAllTasks();
    void createCustomTask();
    void deleteCustomTask();
    void unselectAllTasks();
    void treeSelectionChanged();
    void treeContextMenu(const QPoint &pos);
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void itemCheckStateChanged(QTreeWidgetItem *item, int column);
};

#endif // MAINWINDOW_HPP
