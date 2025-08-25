#include "lsnapgifpanel.h"
#include "ui_lsnapgifpanel.h"

LSnapGifPanel::LSnapGifPanel(QWidget* parent)
    : QWidget(parent)
    , m_ui(new Ui::LSnapGifPanel)
{
    m_ui->setupUi(this);
    initConnect();
}

void LSnapGifPanel::initConnect()
{
    connect(m_ui->startBtn, &QPushButton::clicked, this, &LSnapGifPanel::gifCaptureStart);
    connect(m_ui->stopBtn, &QPushButton::clicked, this, &LSnapGifPanel::gifCaptureStop);
    //connect(this, &LSnapGifPanel::gifCaptureStart, m_pGifRecoder, &GifRecorder::startCapture);
    //connect(this, &LSnapGifPanel::gifCaptureStop, m_pGifRecoder, &GifRecorder::stopCapture);
}