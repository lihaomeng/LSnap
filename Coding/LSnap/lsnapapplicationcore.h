#ifndef LSNAPAPPLICATIONCORE_H
#define LSNAPAPPLICATIONCORE_H

#include <QObject>

class LSnapSystemTray;
class LSnapOverlayWindow;
class LSnapApplicationCore : public QObject
{
    Q_OBJECT
public:
    LSnapApplicationCore(QObject* parent = nullptr);
    ~LSnapApplicationCore();

public slots:
    void onGlobalHotkey(int id);
    void onOverlayCanceled();

private:
    void initSystemtrayMenu();

private:
    LSnapSystemTray* m_pSystemTray = nullptr;
    LSnapOverlayWindow* m_pOverlay = nullptr;
};
#endif // LSNAPAPPLICATIONCORE_H