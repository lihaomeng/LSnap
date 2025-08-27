#include "lsnapselectionactionbar.h"
#include "lsnapview/lsnapdrawpanel.h"
#include "lsnapgifpanel.h"
#include "ui_lsnapselectionactionbar.h"

LSnapSelectionActionBar::LSnapSelectionActionBar(QWidget* parent)
    : QWidget(parent), m_ui(new Ui::LSnapSelectionActionBar)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);
    m_ui->btnSave->setIcon(QIcon(":/icons/save.svg"));
    m_ui->btnSave->setIconSize(QSize(40, 40));
    connect(m_ui->btnSave, &QPushButton::clicked, this, &LSnapSelectionActionBar::saveClicked);

    m_ui->btnCopy->setIcon(QIcon(":/icons/copy.svg"));
    m_ui->btnCopy->setIconSize(QSize(40, 40));
    connect(m_ui->btnCopy, &QPushButton::clicked, this, &LSnapSelectionActionBar::copyClicked);

    m_ui->btnPaste->setIcon(QIcon(":/icons/paste.svg"));
    m_ui->btnPaste->setIconSize(QSize(40, 40));
    connect(m_ui->btnPaste, &QPushButton::clicked, this, &LSnapSelectionActionBar::pasteClicked);

    m_ui->btnCancel->setIcon(QIcon(":/icons/cancel.svg"));
    m_ui->btnCancel->setIconSize(QSize(40, 40));
    connect(m_ui->btnCancel, &QPushButton::clicked, this, &LSnapSelectionActionBar::cancelClicked);

    m_ui->btnDraw->setIcon(QIcon(":/icons/rect.svg"));
    m_ui->btnDraw->setIconSize(QSize(40, 40));
    connect(m_ui->btnDraw, &QPushButton::clicked, this, [this] {
        optionsVisible_ = !optionsVisible_;
        if (m_pDrawPanel)
        {
            if (optionsVisible_)
            {
                updateDrawPanelPosition();
                m_pDrawPanel->show();
            }
            else
            {
                m_ui->verticalLayout->removeWidget(m_pDrawPanel);
                m_pDrawPanel->hide();
            }
        }
        adjustSize();
        if (!optionsVisible_)
            emit shapeModeChanged(-1);
    });

    m_ui->btnGif->setIcon(QIcon(":/icons/stu-revoke-lined.svg"));
    m_ui->btnGif->setIconSize(QSize(40, 40));
    connect(m_ui->btnGif, &QPushButton::clicked, this, [this] {
        if (optionsVisible_)
            optionsVisible_ = false;
        
        const bool newVisible = !m_gifVisible;
        m_gifVisible = newVisible;
        if (m_pGifPanel)
        {
            if (newVisible)
            {
                updateGifPanelPosition();
                m_pGifPanel->show();
                m_pGifPanel->raise();
            }
            else
            {
                m_ui->verticalLayout->removeWidget(m_pGifPanel);
                m_pGifPanel->hide();
                emit gifCancel();
            }
        }
        adjustSize();
        if (!newVisible)
            emit shapeModeChanged(-1);
    });

    QString buttonStyle = "QPushButton{background-color:transparent;border:none;"
    "border-radius:4px;color:black;padding:6px 12px;font-size:11px;min-width:16px;min-height:12px;}"
    "QPushButton:hover{background-color:rgba(0,0,0,30);}"
    "QPushButton:pressed{background-color:rgba(0,0,0,50);}";
    m_ui->btnSave->setStyleSheet(buttonStyle);
    m_ui->btnCopy->setStyleSheet(buttonStyle);
    m_ui->btnPaste->setStyleSheet(buttonStyle);
    m_ui->btnCancel->setStyleSheet(buttonStyle);
    m_ui->btnDraw->setStyleSheet(buttonStyle);
    m_ui->btnGif->setStyleSheet(buttonStyle);
    setStyleSheet("LSnapSelectionActionBar{background-color:white;}");

    m_pDrawPanel = new LSnapDrawPanel(this);
    m_pDrawPanel->hide();
    connect(m_pDrawPanel, &LSnapDrawPanel::drawingModeChanged, this, &LSnapSelectionActionBar::onDrawingModeChanged);
    connect(m_pDrawPanel, &LSnapDrawPanel::lineWidthChanged, this, &LSnapSelectionActionBar::onLineWidthChanged);

    m_pGifPanel = new LSnapGifPanel(this);
    m_pGifPanel->hide();
    connect(m_pGifPanel, &LSnapGifPanel::gifCaptureStart, this, &LSnapSelectionActionBar::gifStartForOverlayWindow);
    connect(m_pGifPanel, &LSnapGifPanel::gifCaptureStop, this, &LSnapSelectionActionBar::gifStopForOverlayWindow);
}

void LSnapSelectionActionBar::onDrawingModeChanged(int mode)
{
    emit shapeModeChanged(mode);
}

void LSnapSelectionActionBar::onLineWidthChanged(int width)
{

}

void LSnapSelectionActionBar::updateDrawPanelPosition()
{
    if (!m_pDrawPanel)
        return;

    //m_ui->verticalLayout->removeWidget(m_pDrawPanel);
    //m_ui->verticalLayout->insertWidget(1, m_pDrawPanel);
    //m_pDrawPanel->setParent(this);
    m_pDrawPanel->move(6, height() - m_pDrawPanel->height() - 4 + 40 + 6 + 5 + 5);
    //m_pDrawPanel->raise();
}

void LSnapSelectionActionBar::updateGifPanelPosition()
{
    if (!m_pGifPanel)
        return;

    // m_pVRootLayout->removeWidget(m_pGifPanel);
    // m_pVRootLayout->insertWidget(1, m_pGifPanel);
 /*   m_ui->verticalLayout->removeWidget(m_pGifPanel);
    m_ui->verticalLayout->insertWidget(1, m_pGifPanel);*/
    //m_pGifPanel->setParent(this);
    m_pGifPanel->move(6, height() - m_pGifPanel->height() - 4 + 40 + 6);
    //m_pGifPanel->raise();
}