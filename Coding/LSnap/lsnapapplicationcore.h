#ifndef LSNAPAPPLICATIONCORE_H
#define LSNAPAPPLICATIONCORE_H

#include <QObject>

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
    LSnapOverlayWindow* m_pOverlay = nullptr;
};
#endif // LSNAPAPPLICATIONCORE_H