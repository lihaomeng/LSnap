#include <QApplication>
#include "lsnapapplicationcore.h"
#include "globalhotkey.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    GlobalHotkeyFilter hotkeys;
    app.installNativeEventFilter(&hotkeys);

    LSnapApplicationCore w;
    QObject::connect(&hotkeys, &GlobalHotkeyFilter::hotkeyPressed, &w, &LSnapApplicationCore::onGlobalHotkey);

    bool ok = hotkeys.registerHotkey(1, 0, 0x70); // F1, 0x70 = VK_F1

    return app.exec();
}
