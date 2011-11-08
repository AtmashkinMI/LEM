#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <QWidget>

class QLabel;
class QPushButton;

class AboutDialog : public QWidget
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent = 0);

private slots:
    void showCredits();
    void showLicense();

private:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);

    QLabel *logoLabel;
    QLabel *nameLabel;
    QLabel *siteLabel;
    QLabel *authorLabel;
    QLabel *versionLabel;
    QLabel *descriptionLabel;

    QPushButton *closeButton;
    QPushButton *creditsButton;
    QPushButton *licenseButton;
};

#endif // ABOUTDIALOG_HPP
