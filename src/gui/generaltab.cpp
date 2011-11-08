#include <QtGui>
#include <QDomDocument>

#include <limits.h>

#include "generaltab.hpp"
#include "settingsdialog.hpp"

GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{
    stopOnErrorBox = new QCheckBox(this);
    QLabel *stopOnErrorLabel = new QLabel(tr("&Break all tasks on first error"), this);
    stopOnErrorLabel->setBuddy(stopOnErrorBox);

    showStatBox = new QCheckBox(this);
    QLabel *showStatLabel = new QLabel(tr("&Show report window with statistics"), this);
    showStatLabel->setBuddy(showStatBox);

    showTrayMessagesBox = new QCheckBox(this);
    QLabel *showTrayMessagesLabel = new QLabel(tr("&Display notifications in system tray"), this);
    showTrayMessagesLabel->setBuddy(showTrayMessagesBox);

    shutdownAfterBox = new QSpinBox(this);
    shutdownAfterBox->setSuffix(tr(" sec"));
    shutdownAfterBox->setMaximum(INT_MAX);

    langBox = new QComboBox(this);

    QStringList langs;
    langs << "English";

    QStringList qmList = SettingsDialog::getQmList();
    for (int i = 0; i < qmList.size(); ++i) {
        langs << languageName(qmList.at(i));
    }

    for (int i = 0; i < langs.size(); ++i) {
        langBox->insertItem(i, QIcon(QString("/usr/share/%1/icons/flags/flag_%2").arg(qApp->applicationName(), SettingsDialog::getLangName(i))), langs.at(i));
    }

    QLabel *langLabel = new QLabel(tr("&Language (need restart):"), this);
    langLabel->setBuddy(langBox);

    QLabel *shutdownAfterLabel = new QLabel(tr("&Power off system after:"), this);
    shutdownAfterLabel->setBuddy(shutdownAfterBox);

    kdeAddButton = new QToolButton(this);
    kdeAddButton->setIcon(QIcon::fromTheme("list-add"));
    connect(kdeAddButton, SIGNAL(clicked()), this, SLOT(kdeAddSlot()));

    kdeDelButton = new QToolButton(this);
    kdeDelButton->setIcon(QIcon::fromTheme("list-remove"));
    connect(kdeDelButton, SIGNAL(clicked()), this, SLOT(kdeDelSlot()));

    xfceAddButton = new QToolButton(this);
    xfceAddButton->setIcon(QIcon::fromTheme("list-add"));
    connect(xfceAddButton, SIGNAL(clicked()), this, SLOT(xfceAddSlot()));

    xfceDelButton = new QToolButton(this);
    xfceDelButton->setIcon(QIcon::fromTheme("list-remove"));
    connect(xfceDelButton, SIGNAL(clicked()), this, SLOT(xfceDelSlot()));

    gnomeAddButton = new QToolButton(this);
    gnomeAddButton->setIcon(QIcon::fromTheme("list-add"));
    connect(gnomeAddButton, SIGNAL(clicked()), this, SLOT(gnomeAddSlot()));

    gnomeDelButton = new QToolButton(this);
    gnomeDelButton->setIcon(QIcon::fromTheme("list-remove"));
    connect(gnomeDelButton, SIGNAL(clicked()), this, SLOT(gnomeDelSlot()));

    QLabel *kdeLabel = new QLabel(tr("Konqueror & Dolphin (KDE)"), this);
    QLabel *xfceLabel = new QLabel(tr("Thunar (Xfce)"), this);
    QLabel *gnomeLabel = new QLabel(tr("Nautilus (GNOME)"), this);

    QHBoxLayout *kdeLayout = new QHBoxLayout;
    kdeLayout->addWidget(kdeLabel, 1);
    kdeLayout->addWidget(kdeAddButton);
    kdeLayout->addWidget(kdeDelButton);

    QHBoxLayout *xfceLayout = new QHBoxLayout;
    xfceLayout->addWidget(xfceLabel, 1);
    xfceLayout->addWidget(xfceAddButton);
    xfceLayout->addWidget(xfceDelButton);

    QHBoxLayout *gnomeLayout = new QHBoxLayout;
    gnomeLayout->addWidget(gnomeLabel, 1);
    gnomeLayout->addWidget(gnomeAddButton);
    gnomeLayout->addWidget(gnomeDelButton);

    QGroupBox *intergrationBox = new QGroupBox(tr("&Integration with file managers"), this);

    QVBoxLayout *intergrationLayout = new QVBoxLayout;
    intergrationLayout->addLayout(xfceLayout);
    intergrationLayout->addLayout(gnomeLayout);
    intergrationLayout->addLayout(kdeLayout);
    intergrationLayout->addStretch();

    intergrationBox->setLayout(intergrationLayout);

    QHBoxLayout *stopOnErrorLayout = new QHBoxLayout;
    stopOnErrorLayout->addWidget(stopOnErrorLabel, 1);
    stopOnErrorLayout->addWidget(stopOnErrorBox);

    QHBoxLayout *showStatLayout = new QHBoxLayout;
    showStatLayout->addWidget(showStatLabel, 1);
    showStatLayout->addWidget(showStatBox);

    QHBoxLayout *showTrayMessagesLayout = new QHBoxLayout;
    showTrayMessagesLayout->addWidget(showTrayMessagesLabel, 1);
    showTrayMessagesLayout->addWidget(showTrayMessagesBox);

    QHBoxLayout *langLayout = new QHBoxLayout;
    langLayout->addWidget(langLabel, 1);
    langLayout->addWidget(langBox);

    QHBoxLayout *shutdownAfterLayout = new QHBoxLayout;
    shutdownAfterLayout->addWidget(shutdownAfterLabel, 1);
    shutdownAfterLayout->addWidget(shutdownAfterBox);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(stopOnErrorLayout);
    mainLayout->addLayout(showStatLayout);
    mainLayout->addLayout(showTrayMessagesLayout);
    mainLayout->addLayout(langLayout);
    mainLayout->addLayout(shutdownAfterLayout);
    mainLayout->addWidget(intergrationBox);

    setLayout(mainLayout);

    connect(showStatBox, SIGNAL(stateChanged(int)), this, SIGNAL(tabChanged()));
    connect(stopOnErrorBox, SIGNAL(stateChanged(int)), this, SIGNAL(tabChanged()));
    connect(showTrayMessagesBox, SIGNAL(stateChanged(int)), this, SIGNAL(tabChanged()));
    connect(langBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(tabChanged()));
    connect(shutdownAfterBox, SIGNAL(valueChanged(int)), this, SIGNAL(tabChanged()));
}

QString GeneralTab::languageName(const QString &qmFile)
{
    QTranslator translator;
    translator.load(qmFile, QString("/usr/share/%1/translations").arg(qApp->applicationName()));

    return translator.translate("GeneralTab", "English");
}

bool GeneralTab::getShowStat()
{
    return showStatBox->isChecked();
}

bool GeneralTab::getStopOnError()
{
    return stopOnErrorBox->isChecked();
}

bool GeneralTab::getShowTrayMessages()
{
    return showTrayMessagesBox->isChecked();
}

void GeneralTab::setShowStat(bool showStat)
{
    showStatBox->setChecked(showStat);
}

void GeneralTab::setStopOnError(bool stopOnError)
{
    stopOnErrorBox->setChecked(stopOnError);
}

void GeneralTab::setShowTrayMessages(bool showTrayMessages)
{
    showTrayMessagesBox->setChecked(showTrayMessages);
}

int GeneralTab::getLangIndex()
{
    return langBox->currentIndex();
}

void GeneralTab::setLangIndex(int langIndex)
{
    langBox->setCurrentIndex(langIndex);
}

int GeneralTab::getShutdownAfter()
{
    return shutdownAfterBox->value();
}

void GeneralTab::setShutdownAfter(int shutdownAfter)
{
    shutdownAfterBox->setValue(shutdownAfter);
}

void GeneralTab::kdeDelSlot()
{
    QFile::remove(QString("/usr/share/kde4/services/ServiceMenus/%1_KDE.desktop").arg(qApp->applicationName()));
}

void GeneralTab::xfceDelSlot()
{
    QFile ucaXml("/etc/xdg/Thunar/uca.xml");
    if (ucaXml.open(QFile::ReadOnly)) {
        QDomDocument ucaDoc;
        if (ucaDoc.setContent(&ucaXml)) {
            if (ucaXml.remove() && QFile::copy(":/uca_head.xml", "/etc/xdg/Thunar/uca.xml") && ucaXml.open(QFile::Append)) {
                QTextStream ucaText(&ucaXml);

                ucaText << "<actions>\n";

                QDomNodeList actionList = ucaDoc.elementsByTagName("action");
                for (int i = 0; i < actionList.size(); ++i) {
                    QDomElement nameTag = actionList.at(i).firstChildElement("name");
                    if (!nameTag.isNull()) {
                        QString nameText = nameTag.text();
                        if (nameText == "Securely Erase..." || nameText == "Securely Empty Trash...") {
                            continue;
                        }
                    }

                    ucaText << '\n';
                    actionList.at(i).save(ucaText, 2);
                }

                ucaText << "\n</actions>";

                ucaXml.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
            }
        }
    }
}

void GeneralTab::gnomeDelSlot()
{
    QFile::remove("/usr/share/file-manager/actions/Erase_action.desktop");
    QFile::remove("/usr/share/file-manager/actions/Trash_action.desktop");
}

void GeneralTab::kdeAddSlot()
{
    kdeDelSlot();

    QFile::copy(QString(":/%1_KDE.desktop").arg(qApp->applicationName()), QString("/usr/share/kde4/services/ServiceMenus/%1_KDE.desktop").arg(qApp->applicationName()));

    QFile(QString("/usr/share/kde4/services/ServiceMenus/%1_KDE.desktop").arg(qApp->applicationName())).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
}

void GeneralTab::xfceAddSlot()
{
    QFile ucaTail(":/uca_tail.xml");
    QFile ucaXml("/etc/xdg/Thunar/uca.xml");

    if (ucaTail.open(QFile::ReadOnly) && ucaXml.open(QFile::ReadOnly)) {
        QDomDocument ucaDoc;
        if (ucaDoc.setContent(&ucaXml)) {
            if (ucaXml.remove() && QFile::copy(":/uca_head.xml", "/etc/xdg/Thunar/uca.xml") && ucaXml.open(QFile::Append)) {
                QTextStream ucaText(&ucaXml);

                ucaText << "<actions>\n";

                QDomNodeList actionList = ucaDoc.elementsByTagName("action");
                for (int i = 0; i < actionList.size(); ++i) {
                    QDomElement nameTag = actionList.at(i).firstChildElement("name");
                    if (!nameTag.isNull()) {
                        QString nameText = nameTag.text();
                        if (nameText == "Securely Erase..." || nameText == "Securely Empty Trash...") {
                            continue;
                        }
                    }

                    ucaText << '\n';
                    actionList.at(i).save(ucaText, 2);
                }

                ucaText << '\n';
                ucaText << QTextStream(&ucaTail).readAll();
                ucaText << "\n</actions>";

                ucaXml.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
            }
        }
    }
}

void GeneralTab::gnomeAddSlot()
{
    gnomeDelSlot();

    QDir actionsDir("/usr/share/file-manager/actions");
    bool dirExists = actionsDir.exists();
    if (!dirExists) {
        dirExists = actionsDir.mkpath(actionsDir.absolutePath());
    }

    if (dirExists) {
        QFile::copy(":/Erase_action.desktop", "/usr/share/file-manager/actions/Erase_action.desktop");
        QFile::copy(":/Trash_action.desktop", "/usr/share/file-manager/actions/Trash_action.desktop");

        QFile("/usr/share/file-manager/actions/Erase_action.desktop").setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
        QFile("/usr/share/file-manager/actions/Trash_action.desktop").setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
    }
}
