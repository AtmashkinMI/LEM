#ifndef MEMERASEDIALOG_HPP
#define MEMERASEDIALOG_HPP

#include <QMap>
#include <QTime>
#include <QWidget>

class QLabel;
class QProcess;
class QPushButton;
class QProgressBar;

class MemEraseDialog : public QWidget
{
    Q_OBJECT

public:
    MemEraseDialog(QWidget *parent = 0);

    QString getErrorString();
    QString getFinishString();

    void startErasure(bool showError, bool showDialog, int owrType, const QString &owrName);

    static qint64 getMemTotal();

private:
    void saveOvercommitState();
    void changeOvercommitState(char newState);

    void changeOOMScores();
    void restoreOOMScores();

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QString fromDouble(double num);

    bool canceled;
    bool exitLock;

    bool showError;

    int owrType;

    QString owrName;

    QString errorString;
    QString finishString;

    qint64 bytesCount;
    qint64 totalMemSize;

    char overcommitState;

    QLabel *statLabel;
    QLabel *imageLabel;

    QTime totalClock;
    QTime progressClock;

    QPushButton *cancelButton;

    QProgressBar *progressBar;

    QProcess *eraseProc;

    QByteArray procBuffer;

    QMap<QString, QByteArray> oldScores;

signals:
    void stopped();
    void finished();

private slots:
    void processOut();
    void processExit();
    void processStart();

    void stopErasure();
};

#endif // MEMERASEDIALOG_HPP
