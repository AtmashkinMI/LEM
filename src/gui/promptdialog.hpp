#ifndef PROMPTDIALOG_HPP
#define PROMPTDIALOG_HPP

#include <QWidget>

class QLabel;
class QCheckBox;
class QPushButton;
class SettingsDialog;
class FileEraseDialog;

class PromptDialog : public QWidget
{
    Q_OBJECT

public:
    PromptDialog(const QStringList &filesList, bool isTrash, QWidget *parent = 0);

private:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

    bool isTrash;

    QLabel *imageLabel;
    QLabel *promptLabel;

    QCheckBox *deleteBox;

    QPushButton *eraseButton;
    QPushButton *cancelButton;
    QPushButton *settingsButton;

    SettingsDialog *settingsDialog;
    FileEraseDialog *fileEraseDialog;

    QStringList filesList;

private slots:
    void startErasure();
    void erasureFinished();
};

#endif // PROMPTDIALOG_HPP
