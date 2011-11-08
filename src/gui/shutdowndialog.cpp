#include <QtGui>

#include "shutdowndialog.hpp"

ShutdownDialog::ShutdownDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags((windowFlags() | Qt::WindowStaysOnTopHint) ^ Qt::WindowContextHelpButtonHint);

    shutdownTimer = new QTimer(this);

    setWindowTitle(tr("Power off - %1").arg(qApp->applicationName()));

    textLabel = new QLabel(this);

    imageLabel = new QLabel(this);
    imageLabel->setPixmap(QIcon::fromTheme("system-shutdown").pixmap(64, 64));
    imageLabel->setAlignment(Qt::AlignCenter);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(textLabel);
    rightLayout->addWidget(buttonBox);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(imageLabel);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(shutdownSystem()));
    connect(shutdownTimer, SIGNAL(timeout()), this, SLOT(secondElapsed()));
}

void ShutdownDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        okButton->setFocus();

        move(QApplication::desktop()->screen()->rect().center() - rect().center());
    }
}

void ShutdownDialog::closeEvent(QCloseEvent *event)
{
    shutdownTimer->stop();

    event->accept();
}

void ShutdownDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void ShutdownDialog::secondElapsed()
{
    if (++shutdownCount >= shutdownAfter) {
        shutdownSystem();
    }
    else {
        updateCount();
    }
}

void ShutdownDialog::shutdownSystem()
{
    close();

    QProcess::startDetached("shutdown -h now");
}

void ShutdownDialog::updateCount()
{
    textLabel->setText(tr("Your system will be powered off in %1 seconds...").arg(shutdownAfter - shutdownCount));
    repaint();
}

void ShutdownDialog::startCount(int shutdownAfter)
{
    this->shutdownAfter = shutdownAfter;

    shutdownCount = 0;

    updateCount();

    shutdownTimer->start(1000);

    show();
}
