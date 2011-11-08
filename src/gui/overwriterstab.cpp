#include <QtGui>

#include "overwriterstab.hpp"

OverwritersTab::OverwritersTab(QWidget *parent)
    : QWidget(parent)
{
    owrTable = new QTableWidget(20, 3, this);

    for (int i = 0; i < 20; ++i) {
        QTableWidgetItem *rowItem = new QTableWidgetItem(QString::number(i + 1));
        rowItem->setTextAlignment(Qt::AlignCenter);
        owrTable->setVerticalHeaderItem(i, rowItem);
    }

    owrTable->setTextElideMode(Qt::ElideNone);

    owrTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Passes") << tr("Verification"));
    owrTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    initTableRow(0,  QStringList() << tr("Overwrite with zeros")                      << tr("1")  << tr("Without verification"));
    initTableRow(1,  QStringList() << tr("Overwrite with ones")                       << tr("1")  << tr("Without verification"));
    initTableRow(2,  QStringList() << tr("Overwrite with random bytes")               << tr("1")  << tr("Without verification"));
    initTableRow(3,  QStringList() << tr("British HMG IS5 (Baseline)")                << tr("1")  << tr("Last step"));
    initTableRow(4,  QStringList() << tr("Australian & New Zealand ISM - 2010")       << tr("1")  << tr("Last step"));
    initTableRow(5,  QStringList() << tr("Russian State Technical Commission - 1992") << tr("2")  << tr("Without verification"));
    initTableRow(6,  QStringList() << tr("British HMG IS5 (Enhanced)")                << tr("3")  << tr("Last step"));
    initTableRow(7,  QStringList() << tr("US Army AR380-19")                          << tr("3")  << tr("Last step"));
    initTableRow(8,  QStringList() << tr("US Department of Defense 5220.22-M")        << tr("3")  << tr("All steps"));
    initTableRow(9,  QStringList() << tr("US Department of Defense 5220.22-M (E)")    << tr("3")  << tr("Without verification"));
    initTableRow(10, QStringList() << tr("US NAVSO P-5239-26 (RLL)")                  << tr("3")  << tr("Last step"));
    initTableRow(11, QStringList() << tr("US NAVSO P-5239-26 (MFM)")                  << tr("3")  << tr("Last step"));
    initTableRow(12, QStringList() << tr("US Air Force AFSSI-5020")                   << tr("3")  << tr("Last step"));
    initTableRow(13, QStringList() << tr("German BSI IT Baseline Protection Manual")  << tr("6")  << tr("Without verification"));
    initTableRow(14, QStringList() << tr("US Department of Defense 5220.22-M (ECE)")  << tr("7")  << tr("Without verification"));
    initTableRow(15, QStringList() << tr("Canadian RCMP TSSIT OPS-II")                << tr("7")  << tr("Last step"));
    initTableRow(16, QStringList() << tr("German VSITR")                              << tr("7")  << tr("Without verification"));
    initTableRow(17, QStringList() << tr("Bruce Schneier's method")                   << tr("7")  << tr("Without verification"));
    initTableRow(18, QStringList() << tr("Roy Pfitzner's method")                     << tr("33") << tr("Without verification"));
    initTableRow(19, QStringList() << tr("Peter Gutmann's method")                    << tr("35") << tr("Without verification"));

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
    switch (owrTable->selectedItems().first()->row()) {
        case 0:
            infoLabel->setText("Your data is overwritten with '0x00' bytes through 1 pass without verification.");
            break;
        case 1:
            infoLabel->setText("Your data is overwritten with '0xFF' bytes through 1 pass without verification.");
            break;
        case 2:
            infoLabel->setText("Your data is overwritten with random bytes through 1 pass without verification.");
            break;
        case 3:
            infoLabel->setText("Your data is overwritten with '0x00' bytes through 1 pass with verification.");
            break;
        case 4:
            infoLabel->setText("Your data is overwritten with random bytes through 1 pass with verification.");
            break;
        case 5:
            infoLabel->setText("Your data is overwritten through 2 passes without verification: first pass - with '0x00' bytes and the last pass - with random bytes.");
            break;
        case 6:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0x00' bytes, second pass - with '0xFF' bytes and the last pass - with random bytes.");
            break;
        case 7:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of last pass: first pass - with random bytes, then 2 passes - with certain byte and with its complement.");
            break;
        case 8:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of all passes: first pass - with '0x00' bytes, second pass - with '0xFF' bytes and the last pass - with random bytes.");
            break;
        case 9:
            infoLabel->setText("Your data is overwritten through 3 passes without verification: 2 passes - with certain byte and with its complement and the last pass - with random bytes.");
            break;
        case 10:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0xFF' bytes, second pass - with special RLL pattern and the last pass - with random bytes.");
            break;
        case 11:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0xFF' bytes, second pass - with special MFM pattern and the last pass - with random bytes.");
            break;
        case 12:
            infoLabel->setText("Your data is overwritten through 3 passes with verification of last pass: first pass - with '0x00' bytes, second pass - with '0xFF' bytes and the last pass - with certain byte.");
            break;
        case 13:
            infoLabel->setText("Your data is overwritten with certain bytes and with their complements through 6 passes without verification.");
            break;
        case 14:
            infoLabel->setText("Your data is overwritten through 7 passes without verification: 2 passes - with certain byte and with its complement, then 2 passes - with random bytes, then 2 passes - with certain byte and with its complement and the last pass - with random bytes.");
            break;
        case 15:
            infoLabel->setText("Your data is overwritten through 7 passes with verification of last pass: 6 alternating passes - with '0x00' bytes and '0xFF' bytes and the last pass - with random bytes.");
            break;
        case 16:
            infoLabel->setText("Your data is overwritten through 7 passes without verification: 6 alternating passes - with '0x00' bytes and '0xFF' bytes and the last pass - with '0xAA' bytes.");
            break;
        case 17:
            infoLabel->setText("Your data is overwritten through 7 passes without verification: first pass - with '0xFF' bytes, second pass - with '0x00' bytes, then 5 passes - with random bytes.");
            break;
        case 18:
            infoLabel->setText("Your data is overwritten with random bytes through 33 passes without verification.");
            break;
        case 19:
            infoLabel->setText("Your data is overwritten through 35 passes without verification: 4 passes - with random bytes, then 27 passes - with special Gutmann's patterns, then 4 passes - with random bytes.");
            break;
    }

    emit tabChanged();
}
