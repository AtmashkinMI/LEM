#ifndef SHUTDOWNDIALOG_HPP
#define SHUTDOWNDIALOG_HPP

#include <QWidget>

class QLabel;
class QTimer;
class QPushButton;
class QDialogButtonBox;

class ShutdownDialog : public QWidget
{
    Q_OBJECT

public:
    ShutdownDialog(QWidget *parent = 0);

    void startCount(int shutdownAfter);

private:
    void updateCount();

    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    int shutdownCount;
    int shutdownAfter;

    QLabel *textLabel;
    QLabel *imageLabel;

    QTimer *shutdownTimer;

    QPushButton *okButton;
    QPushButton *cancelButton;

    QDialogButtonBox *buttonBox;

private slots:
    void secondElapsed();
    void shutdownSystem();
};

#endif // SHUTDOWNDIALOG_HPP
