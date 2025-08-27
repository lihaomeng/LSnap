#include "lsnapapplicationcore.h"
#include "lsnapview/lsnapsystemtray.h"
#include "lsnapview/lsnapoverlaywindow.h"
#include <QGuiApplication>
#include <QScreen>

LSnapApplicationCore::LSnapApplicationCore(QObject* parent) : QObject(parent)
{
    initSystemtrayMenu();
}

LSnapApplicationCore::~LSnapApplicationCore()
{
}

void LSnapApplicationCore::onGlobalHotkey(int id)
{
    Q_UNUSED(id);
    if (!m_pOverlay)
    {
        m_pOverlay = new LSnapOverlayWindow();
        connect(m_pOverlay, &LSnapOverlayWindow::canceled, this, &LSnapApplicationCore::onOverlayCanceled);
        connect(m_pOverlay, &QObject::destroyed, this, [this]() { m_pOverlay = nullptr; });
    }
    m_pOverlay->setGeometry(QGuiApplication::primaryScreen()->virtualGeometry());
    m_pOverlay->show();
    m_pOverlay->raise();
    m_pOverlay->activateWindow();
}

void LSnapApplicationCore::onOverlayCanceled()
{
    if (m_pOverlay)
        m_pOverlay->close();
}

void LSnapApplicationCore::initSystemtrayMenu()
{
    m_pSystemTray = new LSnapSystemTray(this);
    m_pSystemTray->show();
}