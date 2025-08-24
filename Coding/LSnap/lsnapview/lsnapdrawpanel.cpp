#include "lsnapdrawpanel.h"
#include "ui_lsnapdrawpanel.h"
#include <QDebug>

LSnapDrawPanel::LSnapDrawPanel(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::LSnapDrawPanel)
{
    ui->setupUi(this);
    setupConnections();
    updateButtonStates();
    
    // init
    setDrawingMode(0);
    setLineWidth(2);
}

LSnapDrawPanel::~LSnapDrawPanel()
{
    delete ui;
}

QPushButton* LSnapDrawPanel::getLineWidthBtn() const
{
    return ui->lineWidthBtn;
}

QPushButton* LSnapDrawPanel::getRectBtn() const
{
    return ui->rectBtn;
}

QPushButton* LSnapDrawPanel::getEllipseBtn() const
{
    return ui->ellipseBtn;
}

void LSnapDrawPanel::setDrawingMode(int mode)
{
    if (currentDrawingMode_ != mode)
    {
        currentDrawingMode_ = mode;
        updateButtonStates();
        emit drawingModeChanged(mode);
    }
}

int LSnapDrawPanel::getDrawingMode() const
{
    return currentDrawingMode_;
}

void LSnapDrawPanel::setLineWidth(int width)
{
    if (currentLineWidth_ != width)
    {
        currentLineWidth_ = qMax(1, qMin(width, 20)); // 限制在1-20之间
        updateButtonStates();
        emit lineWidthChanged(currentLineWidth_);
    }
}

int LSnapDrawPanel::getLineWidth() const
{
    return currentLineWidth_;
}

void LSnapDrawPanel::setupConnections()
{
    connect(ui->rectBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onRectBtnClicked);
    connect(ui->ellipseBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onEllipseBtnClicked);
    connect(ui->lineWidthBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onLineWidthBtnClicked);
}

void LSnapDrawPanel::updateButtonStates()
{
    ui->rectBtn->setDown(currentDrawingMode_ == 1);
    ui->ellipseBtn->setDown(currentDrawingMode_ == 2);
    ui->lineWidthBtn->setText(QString("线宽: %1").arg(currentLineWidth_));
    QString pressedStyle = "QPushButton { background-color: #4CAF50; color: white; border: 1px solid #45a049; }";
    QString normalStyle = "QPushButton { background-color: #f0f0f0; border: 1px solid #ccc; }";
    ui->rectBtn->setStyleSheet(currentDrawingMode_ == 1 ? pressedStyle : normalStyle);
    ui->ellipseBtn->setStyleSheet(currentDrawingMode_ == 2 ? pressedStyle : normalStyle);
}

void LSnapDrawPanel::onRectBtnClicked()
{
    if (currentDrawingMode_ == 1)
        setDrawingMode(0);
    else
        setDrawingMode(1);
    emit rectButtonClicked();
}

void LSnapDrawPanel::onEllipseBtnClicked()
{
    if (currentDrawingMode_ == 2)
        setDrawingMode(0);
    else 
        setDrawingMode(2);
    emit ellipseButtonClicked();
}

void LSnapDrawPanel::onLineWidthBtnClicked()
{
    int newWidth = currentLineWidth_;
    switch (currentLineWidth_)
    {
    case 2: newWidth = 4; break;
    case 4: newWidth = 6; break;
    case 6: newWidth = 8; break;
    case 8: newWidth = 2; break;
    default: newWidth = 2; break;
    }
    setLineWidth(newWidth);
    emit lineWidthButtonClicked();
}
