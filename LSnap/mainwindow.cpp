#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "overlaywindow.h"

#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::onGlobalHotkey(int id)
{
    Q_UNUSED(id);
    if (!overlay_)
    {
        overlay_ = new OverlayWindow();
        connect(overlay_, &OverlayWindow::canceled, this, &MainWindow::onOverlayCanceled);
        connect(overlay_, &QObject::destroyed, this, [this](){ overlay_ = nullptr; });
    }
    overlay_->setGeometry(QGuiApplication::primaryScreen()->virtualGeometry());
    overlay_->show();
    overlay_->raise();
    overlay_->activateWindow();
}

void MainWindow::onOverlayCanceled()
{
    if (overlay_)
    {
        overlay_->close();
    }
}
