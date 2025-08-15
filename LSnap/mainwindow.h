#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class OverlayWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onGlobalHotkey(int id);
    void onOverlayCanceled();

private:
    Ui::MainWindow *ui = nullptr;
    OverlayWindow* overlay_ = nullptr;
};
#endif // MAINWINDOW_H
