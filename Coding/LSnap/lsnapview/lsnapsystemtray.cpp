#include "lsnapsystemtray.h"
#include <QApplication>
#include <QIcon>

/////////////////TODO 优化界面文件
LSnapSystemTray::LSnapSystemTray(QObject* parent) : QObject(parent)
{
    setupTrayIcon();
    createTrayMenu();
    setupConnections();
}

LSnapSystemTray::~LSnapSystemTray()
{
    if (m_pTrayIcon)
        m_pTrayIcon->hide();
}

void LSnapSystemTray::show()
{
    if (m_pTrayIcon)
        m_pTrayIcon->show();
}

void LSnapSystemTray::hide()
{
    if (m_pTrayIcon)
        m_pTrayIcon->hide();
}

void LSnapSystemTray::setToolTip(const QString& tooltip)
{
    if (m_pTrayIcon)
        m_pTrayIcon->setToolTip(tooltip);
}

void LSnapSystemTray::setIcon(const QIcon& icon)
{
    if (m_pTrayIcon)
        m_pTrayIcon->setIcon(icon);
}

void LSnapSystemTray::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::Trigger:
        emit screenshotRequested();
        break;
    case QSystemTrayIcon::Context:
        break;
    default:
        break;
    }
}

void LSnapSystemTray::onScreenshotAction()
{
    emit screenshotRequested();
}

void LSnapSystemTray::onQuitAction()
{
    emit quitRequested();
}

void LSnapSystemTray::setupTrayIcon()
{
    m_pTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/icons/save.svg");
    if (icon.isNull())
        icon = QIcon::fromTheme("camera-photo");
    m_pTrayIcon->setIcon(icon);
    m_pTrayIcon->setToolTip("LSnap Screenshot Tool");
}

void LSnapSystemTray::createTrayMenu()
{
    m_pTrayMenu = new QMenu();
    m_pScreenshotAction = new QAction("pic", m_pTrayMenu);
    m_pScreenshotAction->setShortcut(QKeySequence("F1"));
    m_pTrayMenu->addAction(m_pScreenshotAction);
    m_pTrayMenu->addSeparator();
    m_pQuitAction = new QAction("exit", m_pTrayMenu);
    m_pTrayMenu->addAction(m_pQuitAction);
}

void LSnapSystemTray::setupConnections()
{
    connect(m_pTrayIcon, &QSystemTrayIcon::activated, this, &LSnapSystemTray::onTrayIconActivated);
    connect(m_pScreenshotAction, &QAction::triggered, this, &LSnapSystemTray::onScreenshotAction);
    connect(m_pQuitAction, &QAction::triggered, this, &LSnapSystemTray::onQuitAction);
    m_pTrayIcon->setContextMenu(m_pTrayMenu);
}