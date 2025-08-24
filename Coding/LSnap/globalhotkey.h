#ifndef GLOBALHOTKEYFILTER_H   
#define GLOBALHOTKEYFILTER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QSet>

namespace WinHotkey
{
enum Modifier : unsigned int
{
    Alt = 0x0001,
    Control = 0x0002,
    Shift = 0x0004,
    Win = 0x0008,
    NoRepeat = 0x4000,
};
}

class GlobalHotkeyFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit GlobalHotkeyFilter(QObject* parent = nullptr);
    ~GlobalHotkeyFilter() override;

    bool registerHotkey(int id, unsigned int modifiers, unsigned int vk);
    void unregisterHotkey(int id);

    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

signals:
    void hotkeyPressed(int id);

private:
    QSet<int> m_ids;
};
#endif // GLOBALHOTKEYFILTER_H