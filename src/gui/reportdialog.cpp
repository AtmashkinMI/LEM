#include <QtGui>

#include "reportdialog.hpp"

ReportDialog::ReportDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    reportEdit = new QTextEdit(this);
    reportEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    reportEdit->setReadOnly(true);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close, Qt::Horizontal, this);
    connect(buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(close()));
    connect(buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SIGNAL(save()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(reportEdit);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    setWindowTitle(tr("Finished!"));
}

void ReportDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        buttonBox->button(QDialogButtonBox::Close)->setFocus();
    }
}

void ReportDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void ReportDialog::closeEvent(QCloseEvent *event)
{
    emit closed();

    event->accept();
}

void ReportDialog::showReport(const QString &reportString, bool showDialog)
{
    reportEdit->setHtml(reportString);
    resize(400, 350);

    if (showDialog) {
        show();
    }
}
