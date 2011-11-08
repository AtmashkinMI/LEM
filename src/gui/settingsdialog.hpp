#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QWidget>

class QLabel;
class QSettings;
class GeneralTab;
class QTabWidget;
class QPushButton;
class OverwritersTab;
class QDialogButtonBox;

class SettingsDialog : public QWidget
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);

    int getOwrType();
    bool getShowStat();
    bool getStopOnError();
    int getShutdownAfter();
    bool getShowTrayMessages();
    QString getOwrName(int owrType);

    static int getLangIndex();
    static QString getLangName(int langIndex = -1);
    static QStringList getQmList();

private:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void readSettings();
    void writeSettings();

    QTabWidget *tabs;

    QSettings *settings;

    GeneralTab *generalTab;

    OverwritersTab *owrTab;

    QPushButton *okButton;
    QPushButton *applyButton;
    QPushButton *resetButton;
    QPushButton *cancelButton;

    QDialogButtonBox *buttonBox;

private slots:
    void okClicked();
    void enableApply();
    void applyClicked();
    void resetSettings();
};

#endif // SETTINGSDIALOG_HPP
