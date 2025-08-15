#include "gifpanel.h"

GifPanel::GifPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(4, 2, 4, 2);
    lay->setSpacing(6);

    fpsBox_ = new QComboBox(this);
    fpsBox_->addItems({ "5", "10", "15", "20", "25", "30" });
    fpsBox_->setCurrentText("15");

    loopBox_ = new QCheckBox(QStringLiteral("循环"), this);
    loopBox_->setChecked(true);

    timeLabel_ = new QLabel(QStringLiteral("00:00.0"), this);
    infoLabel_ = new QLabel(QStringLiteral("0 帧 0x0"), this);

    startBtn_ = new QPushButton(QStringLiteral("开始"), this);
    stopBtn_ = new QPushButton(QStringLiteral("停止"), this);
    cancelBtn_ = new QPushButton(QStringLiteral("取消"), this);

    lay->addWidget(new QLabel(QStringLiteral("FPS"), this));
    lay->addWidget(fpsBox_);
    lay->addWidget(loopBox_);
    lay->addSpacing(6);
    lay->addWidget(timeLabel_);
    lay->addWidget(infoLabel_);
    lay->addStretch();
    lay->addWidget(startBtn_);
    lay->addWidget(stopBtn_);
    lay->addWidget(cancelBtn_);

    timer_ = new QTimer(this);
    timer_->setInterval(100);
    connect(timer_, &QTimer::timeout, this, &GifPanel::updateUi);

    connect(startBtn_, &QPushButton::clicked, this, [this]{
        if (running_) return;
        running_ = true;
        elapsed_.restart();
        timer_->start();
        emit startRequested(fpsBox_->currentText().toInt(), loopBox_->isChecked());
    });
    connect(stopBtn_, &QPushButton::clicked, this, [this]{
        if (!running_) return;
        running_ = false;
        timer_->stop();
        emit stopRequested();
    });
    connect(cancelBtn_, &QPushButton::clicked, this, [this]{
        if (running_) { running_ = false; timer_->stop(); }
        emit cancelRequested();
    });
}

void GifPanel::updateUi()
{
    if (!running_) return;
    const qint64 ms = elapsed_.elapsed();
    const int sec = int(ms / 1000);
    const int min = sec / 60;
    const int rem = sec % 60;
    const int deci = int((ms % 1000) / 100);
    timeLabel_->setText(QStringLiteral("%1:%2.%3")
                            .arg(min, 2, 10, QLatin1Char('0'))
                            .arg(rem, 2, 10, QLatin1Char('0'))
                            .arg(deci));
}
