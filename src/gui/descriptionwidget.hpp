#ifndef DESCRIPTIONWIDGET_HPP
#define DESCRIPTIONWIDGET_HPP

#include <QWidget>

#include "taskinfo.hpp"

class QLabel;
class QPushButton;
class QToolButton;
class QTreeWidget;
class QVBoxLayout;

class DescriptionWidget : public QWidget
{
    Q_OBJECT

public:
    DescriptionWidget(TaskMap &tasks, QWidget *parent = 0);

    void setApplicationWindow();
    void setChildWindow(QTreeWidgetItem *task);
    void setParentWindow(QTreeWidgetItem *task);

private:
    void clearWidget();
    void clearLayout(QLayout *layout);

    QString fromDouble(double num);
    QString fileSizeToStr(qint64 fileSize);

    void createRamTree();
    void createFilesTree();
    void createUnusedTree();

    void fillCustomTree();
    void createCustomTree(QVBoxLayout *&buttonsLayout);

    TaskMap &tasks;

    QFont bigFont;
    QFont littleFont;
    QFont middleFont;

    QLabel *totalSizeLabel;

    QTreeWidget *filesTree;
    QTreeWidgetItem *currentTask;
    QTreeWidgetItem *unusedSpacetask;

    QToolButton *delEntriesButton;

    QMap<QPushButton *, QTreeWidgetItem *> buttonsMap;

private slots:
    void selectItem();
    void addDirsToCustomTree();
    void addFilesToCustomTree();
    void treeSelectionChanged();
    void delEntriesFromCustomTree();
    void checkUnusedSpace(QTreeWidgetItem *item, int column);
    void treeCheckStateChanged(QTreeWidgetItem *item, int column);
};

#endif // DESCRIPTIONWIDGET_HPP
