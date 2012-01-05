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

#include "generaltab.hpp"
#include "overwriterstab.hpp"
#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QWidget(parent, Qt::Dialog)
{
    setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("Settings - %1").arg(qApp->applicationName()));

    settings = new QSettings(QString("/etc/%1.ini").arg(qApp->applicationName()), QSettings::IniFormat, this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok    |
                                     QDialogButtonBox::Apply |
                                     QDialogButtonBox::Reset |
                                     QDialogButtonBox::Cancel, Qt::Horizontal, this);

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    applyButton = buttonBox->button(QDialogButtonBox::Apply);
    resetButton = buttonBox->button(QDialogButtonBox::Reset);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    tabs = new QTabWidget(this);

    generalTab = new GeneralTab(this);
    tabs->addTab(generalTab, QIcon::fromTheme("preferences-system"), tr("&General"));

    owrTab = new OverwritersTab(this);
    tabs->addTab(owrTab, QIcon::fromTheme("preferences-other"), tr("&Erasing schemes"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabs);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    setWindowModality(Qt::ApplicationModal);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(okClicked()));
    connect(owrTab, SIGNAL(tabChanged()), this, SLOT(enableApply()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(applyClicked()));
    connect(generalTab, SIGNAL(tabChanged()), this, SLOT(enableApply()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetSettings()));
}

void SettingsDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous()) {
        readSettings();

        tabs->setCurrentIndex(0);

        applyButton->setEnabled(false);
        cancelButton->setFocus();

        resize(650, 500);
    }
}

void SettingsDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

QStringList SettingsDialog::getQmList()
{
    return QDir(QString("/usr/share/%1/translations").arg(qApp->applicationName())).entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
}

bool SettingsDialog::getShowStat()
{
    settings->sync();

    bool showStat = settings->value("general_settings/show_stat_box", true).toBool();

    return showStat;
}

bool SettingsDialog::getStopOnError()
{
    settings->sync();

    bool stopOnError = settings->value("general_settings/stop_on_error", true).toBool();

    return stopOnError;
}

bool SettingsDialog::getShowTrayMessages()
{
    settings->sync();

    bool showTrayMessages = settings->value("general_settings/show_tray_messages", true).toBool();

    return showTrayMessages;
}

QString SettingsDialog::getLangName(int langIndex)
{
    QString langName = "en";

    QStringList qmList = SettingsDialog::getQmList();

    if (langIndex < 0 || langIndex > qmList.size()) {
        langIndex = SettingsDialog::getLangIndex();
    }

    if (langIndex > 0) {
        QString qmPath = qmList.at(langIndex - 1);

        int startIndex = qmPath.lastIndexOf('_') + 1;
        int endIndex = qmPath.lastIndexOf('.');

        langName = qmPath.mid(startIndex, endIndex - startIndex);
    }

    return langName;
}

int SettingsDialog::getLangIndex()
{
    QSettings tmpSettings(QString("/etc/%1.ini").arg(qApp->applicationName()), QSettings::IniFormat);
    tmpSettings.sync();

    int langIndex = tmpSettings.value("general_settings/language_index", 0).toInt();

    if (langIndex < 0 || langIndex > getQmList().size()) {
        langIndex = 0;
    }

    return langIndex;
}

int SettingsDialog::getShutdownAfter()
{
    settings->sync();

    int shutdownAfter = settings->value("general_settings/power_off_after", 30).toInt();

    if (shutdownAfter < 0) {
        shutdownAfter = 30;
    }

    return shutdownAfter;
}

int SettingsDialog::getOwrType()
{
    settings->sync();

    int owrType = settings->value("erasing_schemes/overwriter_type", 0).toInt();

    if (owrType < 0 || owrType >= OVERWRITERS_COUNT) {
        owrType = 0;
    }

    return owrType;
}

QString SettingsDialog::getOwrName(int owrType)
{
    return owrTab->getOwrName(owrType);
}

void SettingsDialog::readSettings()
{
    generalTab->setShowStat(getShowStat());
    generalTab->setStopOnError(getStopOnError());
    generalTab->setShowTrayMessages(getShowTrayMessages());
    generalTab->setLangIndex(getLangIndex());
    generalTab->setShutdownAfter(getShutdownAfter());

    owrTab->setOwrType(getOwrType());
}

void SettingsDialog::writeSettings()
{
    settings->clear();

    settings->setValue("general_settings/show_stat_box", generalTab->getShowStat());
    settings->setValue("general_settings/stop_on_error", generalTab->getStopOnError());
    settings->setValue("general_settings/show_tray_messages", generalTab->getShowTrayMessages());
    settings->setValue("general_settings/language_index", generalTab->getLangIndex());
    settings->setValue("general_settings/power_off_after", generalTab->getShutdownAfter());

    settings->setValue("erasing_schemes/overwriter_type", owrTab->getOwrType());

    settings->sync();
}

void SettingsDialog::enableApply()
{
    applyButton->setEnabled(true);
}

void SettingsDialog::applyClicked()
{
    writeSettings();

    applyButton->setEnabled(false);
}

void SettingsDialog::okClicked()
{
    applyClicked();

    if (!applyButton->isEnabled()) {
        close();
    }
}

void SettingsDialog::resetSettings()
{
    generalTab->setShowStat(true);
    generalTab->setStopOnError(true);
    generalTab->setShowTrayMessages(true);
    generalTab->setLangIndex(0);
    generalTab->setShutdownAfter(30);

    generalTab->kdeDelSlot();
    generalTab->xfceDelSlot();
    generalTab->gnomeDelSlot();

    owrTab->setOwrType(0);
}
