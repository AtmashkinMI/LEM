#include <unistd.h>

#include <QFileInfo>
#include <QGtkStyle>
#include <QTextCodec>
#include <QMessageBox>
#include <QTranslator>
#include <QApplication>
#include <QSharedMemory>

#include "gui/mainwindow.hpp"
#include "gui/promptdialog.hpp"
#include "gui/settingsdialog.hpp"

#include "srm/systeminfo.hpp"
#include "srm/memoryeraser.hpp"

void showErrorMessage(const QString &errorString)
{
    QMessageBox errorMessage;
    errorMessage.setTextFormat(Qt::PlainText);
    errorMessage.setIcon(QMessageBox::Critical);
    errorMessage.addButton(QMessageBox::Ok);
    errorMessage.setWindowTitle(QObject::tr("Error!"));
    errorMessage.setText(errorString);
    errorMessage.exec();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    app.setApplicationName("LEM");

    if (!checkRunningKDE()) {
        QFile iconTheme(QString("/usr/share/%1/gtk/icon_theme").arg(qApp->applicationName()));
        if (iconTheme.open(QFile::ReadOnly)) {
            QIcon::setThemeName(iconTheme.readAll().trimmed());
        }

        QFile fontName(QString("/usr/share/%1/gtk/font_name").arg(qApp->applicationName()));
        if (fontName.open(QFile::ReadOnly)) {
            QFont appFont;
            if (appFont.fromString(fontName.readAll().trimmed())) {
                app.setFont(appFont);
            }
        }

        if (qputenv("GTK2_RC_FILES", QFile::encodeName(QString("/usr/share/%1/gtk/gtkrc").arg(qApp->applicationName())))) {
            app.setStyle(new QGtkStyle);
        }
    }

    QTranslator appTranslator, qtTranslator;
    int langIndex = SettingsDialog::getLangIndex();
    if (langIndex > 0) {
        QString qmPath = SettingsDialog::getQmList().at(langIndex - 1);

        QString qtPath = qmPath.right(qmPath.size() - qmPath.lastIndexOf('_'));
        qtPath.prepend("qt");

        if (appTranslator.load(qmPath, QString("/usr/share/%1/translations").arg(qApp->applicationName()))) {
            app.installTranslator(&appTranslator);
        }

        if (qtTranslator.load(qtPath, "/usr/share/qt/translations")) {
            app.installTranslator(&qtTranslator);
        }
    }

    app.setApplicationVersion(QObject::tr("Version - 0.11.11"));
    app.setOrganizationName(QObject::tr("Linux Erasing Machine"));
    app.setOrganizationDomain(QObject::tr("https://github.com/AtmashkinMI/%1").arg(qApp->applicationName()));
    app.setWindowIcon(QIcon(QString("/usr/share/pixmaps/%1.svg").arg(qApp->applicationName())));

    if (getuid() != 0) {
        showErrorMessage(QObject::tr("This program must have 'root' privileges!"));
        return EXIT_ERROR;
    }

    if (!QFileInfo(URANDOM_FILE).exists()) {
        showErrorMessage(QObject::tr("The random source '%1' doesn't exist!").arg(URANDOM_FILE));
        return EXIT_ERROR;
    }

    QStringList args = app.arguments();

    if (args.size() == 3 && args[1] == "--memory") {
        bool goodType;
        int owrType = args[2].toInt(&goodType);
        if (goodType) {
            MemoryEraser memEraser(owrType);
            return memEraser.erase();
        }
    }

    if (args.size() == 3 && args[1] == "--trash") {
        PromptDialog promptWin(args.mid(2), true);
        promptWin.show();
        return app.exec();
    }

    if (args.size() == 3 && args[1] == "--user") {
        QSharedMemory memLock("lock");
        if (memLock.attach(QSharedMemory::ReadOnly)){
            memLock.detach();
            showErrorMessage(QObject::tr("%1 is allready running!").arg(qApp->applicationName()));
            return EXIT_ERROR;
        }
        else {
            memLock.create(1);
        }

        MainWindow mainWin(args[2]);
        mainWin.show();

        return app.exec();
    }

    if (args.size() >= 3 && args[1] == "--erase") {
        PromptDialog promptWin(args.mid(2), false);
        promptWin.show();
        return app.exec();
    }

    return EXIT_ERROR;
}
