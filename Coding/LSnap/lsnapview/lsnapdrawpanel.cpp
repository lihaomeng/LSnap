#include "lsnapdrawpanel.h"
#include "ui_lsnapdrawpanel.h"

LSnapDrawPanel::LSnapDrawPanel(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::LSnapDrawPanel)
{
    ui->setupUi(this);
    setupConnections();
    updateButtonStates();
    setDrawingMode(0);
    setLineWidth(2);

    ui->rectBtn->setIcon(QIcon(":/icons/save.svg"));
    ui->rectBtn->setIconSize(QSize(40, 40));
    ui->ellipseBtn->setIcon(QIcon(":/icons/save.svg"));
    ui->ellipseBtn->setIconSize(QSize(40, 40));
}

void LSnapDrawPanel::setupConnections()
{
    connect(ui->rectBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onRectBtnClicked);
    connect(ui->ellipseBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onEllipseBtnClicked);
    connect(ui->lineWidthBtn, &QPushButton::clicked, this, &LSnapDrawPanel::onLineWidthBtnClicked);
}

void LSnapDrawPanel::updateButtonStates()
{
    ui->lineWidthBtn->setText(QString("width: %1").arg(m_currentLineWidth));
    const QString& normalStyle = "QPushButton{background-color:transparent;border:none;"
        "border-radius:4px;color:black;padding:6px 12px;font-size:11px;min-width:16px;min-height:12px;}"
        "QPushButton:hover{background-color:rgba(0,0,0,30);}"
        "QPushButton:pressed{background-color:rgba(0,0,0,50);}";
        
    const QString& pressedStyle = "QPushButton{background-color:rgba(76,175,80,50);border:none;"
    "border-radius:4px;color:#4CAF50;padding:6px 12px;font-size:11px;min-width:16px;min-height:12px;}"
    "QPushButton:hover{background-color:rgba(76,175,80,70);}"
    "QPushButton:pressed{background-color:rgba(76,175,80,90);}";
    ui->rectBtn->setStyleSheet(m_currentDrawingMode == 1 ? pressedStyle : normalStyle);
    ui->ellipseBtn->setStyleSheet(m_currentDrawingMode == 2 ? pressedStyle : normalStyle);
}

LSnapDrawPanel::~LSnapDrawPanel()
{
    delete ui;
}

void LSnapDrawPanel::setDrawingMode(int mode)
{
    if (m_currentDrawingMode != mode)
    {
        m_currentDrawingMode = mode;
        updateButtonStates();
        emit drawingModeChanged(mode);
    }
}

int LSnapDrawPanel::getDrawingMode() const
{
    return m_currentDrawingMode;
}

void LSnapDrawPanel::setLineWidth(int width)
{
    if (m_currentLineWidth != width)
    {
        m_currentLineWidth = qMax(1, qMin(width, 20));
        updateButtonStates();
        emit lineWidthChanged(m_currentLineWidth);
    }
}

int LSnapDrawPanel::getLineWidth() const
{
    return m_currentLineWidth;
}

void LSnapDrawPanel::onRectBtnClicked()
{
    if (m_currentDrawingMode == 1)
        setDrawingMode(0);
    else
        setDrawingMode(1);
    emit rectButtonClicked();
}

void LSnapDrawPanel::onEllipseBtnClicked()
{
    if (m_currentDrawingMode == 2)
        setDrawingMode(0);
    else 
        setDrawingMode(2);
    emit ellipseButtonClicked();
}

void LSnapDrawPanel::onLineWidthBtnClicked()
{
    int newWidth = m_currentLineWidth;
    switch (m_currentLineWidth)
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