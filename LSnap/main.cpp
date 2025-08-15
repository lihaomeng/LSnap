#include "mainwindow.h"
#include "globalhotkey.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    GlobalHotkeyFilter hotkeys;
    a.installNativeEventFilter(&hotkeys);

    MainWindow w;
    QObject::connect(&hotkeys, &GlobalHotkeyFilter::hotkeyPressed, &w, &MainWindow::onGlobalHotkey);
    bool ok = hotkeys.registerHotkey(1, 0, 0x70); // F1, 0x70 = VK_F1
    //qDebug() << ok;
    //TODO F3是将复制的内容变成贴图
    //w.show();
    return a.exec();
}
