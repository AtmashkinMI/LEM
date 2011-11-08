#ifndef GENERALTAB_HPP
#define GENERALTAB_HPP

#include <QWidget>

class QSpinBox;
class QCheckBox;
class QComboBox;
class QToolButton;

class GeneralTab : public QWidget
{
    Q_OBJECT

public:
    GeneralTab(QWidget *parent = 0);

    bool getShowStat();
    void setShowStat(bool showStat);

    bool getStopOnError();
    void setStopOnError(bool stopOnError);

    bool getShowTrayMessages();
    void setShowTrayMessages(bool showTrayMessages);

    int getLangIndex();
    void setLangIndex(int langIndex);

    int getShutdownAfter();
    void setShutdownAfter(int shutdownAfter);

signals:
    void tabChanged();

private:
    QString languageName(const QString &qmFile);

    QComboBox *langBox;

    QCheckBox *showStatBox;
    QCheckBox *stopOnErrorBox;
    QCheckBox *showTrayMessagesBox;

    QToolButton *kdeAddButton;
    QToolButton *kdeDelButton;

    QToolButton *xfceAddButton;
    QToolButton *xfceDelButton;

    QToolButton *gnomeAddButton;
    QToolButton *gnomeDelButton;

    QSpinBox *shutdownAfterBox;

public slots:
    void kdeDelSlot();
    void xfceDelSlot();
    void gnomeDelSlot();

private slots:
    void kdeAddSlot();
    void xfceAddSlot();
    void gnomeAddSlot();
};

#endif // GENERALTAB_HPP
