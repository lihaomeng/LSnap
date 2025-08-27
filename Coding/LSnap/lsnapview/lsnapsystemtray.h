#ifndef LSNAPSYSTEMTRAY_H
#define LSNAPSYSTEMTRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>

class LSnapSystemTray : public QObject
{
    Q_OBJECT
public:
    explicit LSnapSystemTray(QObject* parent = nullptr);
    ~LSnapSystemTray();
    void show();
    void hide();
    void setToolTip(const QString& tooltip);
    void setIcon(const QIcon& icon);

signals:
    void screenshotRequested();
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onScreenshotAction();
    void onQuitAction();

private:
    void setupTrayIcon();
    void createTrayMenu();
    void setupConnections();

private:
    QSystemTrayIcon* m_pTrayIcon = nullptr;
    QMenu* m_pTrayMenu = nullptr;
    QAction* m_pScreenshotAction = nullptr;
    QAction* m_pQuitAction = nullptr;
};

#endif // LSNAPSYSTEMTRAY_H
