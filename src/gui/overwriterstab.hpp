#ifndef OVERWRITERSTAB_HPP
#define OVERWRITERSTAB_HPP

#include <QWidget>

class QLabel;
class QTableWidget;
class QTableWidgetItem;

class OverwritersTab : public QWidget
{
    Q_OBJECT

public:
    OverwritersTab(QWidget *parent = 0);

    int getOwrType();
    void setOwrType(int owrType);

    QString getOwrName(int owrType);

signals:
    void tabChanged();

private:
    void initTableRow(int row, const QStringList &cells);

    QLabel *descLabel;
    QLabel *infoLabel;

    QTableWidget *owrTable;

private slots:
    void setInfoLabel();
};

#endif // OVERWRITERSTAB_HPP
