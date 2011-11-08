#ifndef REPORTDIALOG_HPP
#define REPORTDIALOG_HPP

#include <QWidget>

class QTextEdit;
class QDialogButtonBox;

class ReportDialog : public QWidget
{
    Q_OBJECT

public:
    ReportDialog(QWidget *parent = 0);

    void showReport(const QString &reportString, bool showDialog);

signals:
    void save();
    void closed();

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QTextEdit *reportEdit;
    QDialogButtonBox *buttonBox;
};

#endif // REPORTDIALOG_HPP
