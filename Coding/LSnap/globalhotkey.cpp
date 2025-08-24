#include "globalhotkey.h"

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

GlobalHotkeyFilter::GlobalHotkeyFilter(QObject* parent) : QObject(parent) {}

GlobalHotkeyFilter::~GlobalHotkeyFilter()
{
#ifdef Q_OS_WIN
    for (int id : m_ids)
    {
        UnregisterHotKey(nullptr, id);
    }
#endif
}

bool GlobalHotkeyFilter::registerHotkey(int id, unsigned int modifiers, unsigned int vk)
{
#ifdef Q_OS_WIN
    if (RegisterHotKey(nullptr, id, modifiers, vk))
    {
        m_ids.insert(id);
        return true;
    }
#endif
    return false;
}

void GlobalHotkeyFilter::unregisterHotkey(int id)
{
#ifdef Q_OS_WIN
    UnregisterHotKey(nullptr, id);
#endif
    m_ids.remove(id);
}

bool GlobalHotkeyFilter::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY)
        {
            int id = static_cast<int>(msg->wParam);
            emit hotkeyPressed(id);
            return false;
        }
    }
#endif
    Q_UNUSED(result);
    return false;
}
