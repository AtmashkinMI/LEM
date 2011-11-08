#include <QtGui>

#include "aboutdialog.hpp"

AboutDialog::AboutDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("About - %1").arg(qApp->applicationName()));

    logoLabel = new QLabel(this);
    logoLabel->setPixmap(qApp->windowIcon().pixmap(128, 128));
    logoLabel->setAlignment(Qt::AlignCenter);

    QFont nameFont;
    nameFont.setBold(true);
    nameFont.setPointSize(20);

    nameLabel = new QLabel(qApp->organizationName(), this);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);

    QFont versionFont;
    versionFont.setBold(true);
    versionFont.setPointSize(13);

    versionLabel = new QLabel(qApp->applicationVersion(), this);
    versionLabel->setFont(versionFont);
    versionLabel->setAlignment(Qt::AlignCenter);

    descriptionLabel = new QLabel(tr("Securely erases sensitive information in Linux"), this);
    descriptionLabel->setAlignment(Qt::AlignCenter);

    QFont authorFont;
    authorFont.setPointSize(9);

    authorLabel = new QLabel(tr("© 2011 Atmashkin M.I."), this);
    authorLabel->setFont(authorFont);
    authorLabel->setAlignment(Qt::AlignCenter);

    siteLabel = new QLabel(QString("<a href=\"%1\">%1</a>").arg(qApp->organizationDomain()), this);
    siteLabel->setOpenExternalLinks(true);
    siteLabel->setAlignment(Qt::AlignCenter);

    creditsButton = new QPushButton(QIcon::fromTheme("help-about"), tr("C&redits"), this);
    licenseButton = new QPushButton(QIcon::fromTheme("text-x-generic"), tr("&License"), this);
    closeButton = new QPushButton(QIcon::fromTheme("window-close"), tr("&Close"), this);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(creditsButton);
    buttonsLayout->addWidget(licenseButton);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(logoLabel);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(authorLabel);
    mainLayout->addWidget(siteLabel);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    setWindowModality(Qt::ApplicationModal);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(creditsButton, SIGNAL(clicked()), this, SLOT(showCredits()));
    connect(licenseButton, SIGNAL(clicked()), this, SLOT(showLicense()));
}

void AboutDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        closeButton->setFocus();
    }
}

void AboutDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

void AboutDialog::showCredits()
{
    QDialog creditsDialog(this);
    creditsDialog.setWindowFlags(creditsDialog.windowFlags() ^ Qt::WindowContextHelpButtonHint);
    QTabWidget creditsTabs(&creditsDialog);
    QDialogButtonBox closeBox(QDialogButtonBox::Close, Qt::Horizontal, &creditsDialog);

    QLabel tabLabel(tr("Atmashkin M.I. &lt;<a href=\"mailto:AtmashkinMI@gmail.com\">AtmashkinMI@gmail.com</a>&gt;"));
    tabLabel.setOpenExternalLinks(true);
    creditsTabs.addTab(&tabLabel, tr("Written by"));

    connect(&closeBox, SIGNAL(rejected()), &creditsDialog, SLOT(close()));

    QVBoxLayout mainLayout;
    mainLayout.addWidget(&creditsTabs);
    mainLayout.addWidget(&closeBox);
    mainLayout.setSizeConstraint(QLayout::SetFixedSize);

    creditsDialog.setLayout(&mainLayout);
    creditsDialog.setWindowTitle(tr("Credits"));
    creditsDialog.exec();
}

void AboutDialog::showLicense()
{
    QDialog licenseDialog(this);
    licenseDialog.setWindowFlags(licenseDialog.windowFlags() ^ Qt::WindowContextHelpButtonHint);
    QVBoxLayout mainLayout;
    QTextEdit licenseEdit(&licenseDialog);
    QDialogButtonBox closeBox(QDialogButtonBox::Close, Qt::Horizontal, &licenseDialog);

    QFile licenseFile(":/gpl-3.0-standalone.html");
    licenseFile.open(QIODevice::ReadOnly);

    QTextStream license(&licenseFile);
    licenseEdit.setHtml(license.readAll());
    licenseEdit.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    licenseEdit.setReadOnly(true);

    connect(&closeBox, SIGNAL(rejected()), &licenseDialog, SLOT(close()));

    mainLayout.addWidget(&licenseEdit);
    mainLayout.addWidget(&closeBox);

    licenseDialog.setLayout(&mainLayout);
    licenseDialog.setWindowTitle(tr("License"));
    licenseDialog.resize(500, 350);
    licenseDialog.exec();
}
