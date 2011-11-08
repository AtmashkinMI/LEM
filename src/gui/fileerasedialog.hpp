#ifndef FILEERASEDIALOG_HPP
#define FILEERASEDIALOG_HPP

#include <QTime>
#include <QWidget>

#include "../srm/fileinfo.hpp"

class QLabel;
class FileEraser;
class QPushButton;
class QProgressBar;

class FileEraseDialog : public QWidget
{
    Q_OBJECT

public:
    FileEraseDialog(QWidget *parent = 0);

    QString getErrorString();
    QString getFinishString();

    void startErasure(bool showError, bool showDialog, int owrType, const QString &owrName, const FilesList &filesList);

    static qint64 getFileSize(const QString &fileName);
    static QIcon fileTypeIcon(const QString &fileName);
    static QString fileTypeStr(const QString &fileName);
    static FilesList getTrashList(const QString &userName);

private:
    QString fromDouble(double num);

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QString owrName;

    QString errorString;
    QString finishString;

    QLabel *statLabel;
    QLabel *imageLabel;

    QPushButton *cancelButton;

    QProgressBar *totalProgress;
    QProgressBar *currentProgress;

    FileEraser *eraseThread;

    int leftPart;
    int roundsNumber;

    bool canceled;
    bool exitLock;

    bool showError;

    QTime totalClock;

signals:
    void stopped();
    void finished();

private slots:
    void stopErasure();
    void threadFinished();
    void finishErasePart();
    void renamingRound(int round);
    void changeStatLabel(double speed, double time, int round);
    void changeFormType(const QString &fileName, int type, bool firstTime);
};

#endif // FILEERASEDIALOG_HPP
