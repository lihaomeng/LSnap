#include "lsnapselectionactionbar.h"
#include "lsnapview/lsnapdrawpanel.h"
//#include "gifpanel.h"
#include <QHBoxLayout>

LSnapSelectionActionBar::LSnapSelectionActionBar(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    m_pVRootLayout = new QVBoxLayout(this);
    m_pVRootLayout->setContentsMargins(6, 4, 6, 4);
    m_pVRootLayout->setSpacing(4);
    m_pHButtonLayout = new QHBoxLayout();
    m_pHButtonLayout->setContentsMargins(0, 0, 0, 0);
    m_pHButtonLayout->setSpacing(6);

    m_pVRootLayout->addLayout(m_pHButtonLayout);
    setStyleSheet("QWidget{background-color:rgba(40,40,40,200);border-radius:6px;}");

    m_pDrawPanel = new LSnapDrawPanel(this);
    //m_pVRootLayout->addWidget(m_pDrawPanel);
    m_pDrawPanel->hide();

    connect(m_pDrawPanel, &LSnapDrawPanel::drawingModeChanged, this, &LSnapSelectionActionBar::onDrawingModeChanged);
    connect(m_pDrawPanel, &LSnapDrawPanel::lineWidthChanged, this, &LSnapSelectionActionBar::onLineWidthChanged);

    ////GIF相关
    //gifPanel_ = new GifPanel(this);
    //m_pVRootLayout_->addWidget(gifPanel_);
    //gifPanel_->hide();
    //connect(gifPanel_, &GifPanel::startRequested, this, &SelectionActionBar::gifStart);
    //connect(gifPanel_, &GifPanel::stopRequested,  this, &SelectionActionBar::gifStop);
    //connect(gifPanel_, &GifPanel::cancelRequested,this, &SelectionActionBar::gifCancel);
}

//TODO color
void LSnapSelectionActionBar::onDrawingModeChanged(int mode)
{
    emit shapeModeChanged(mode);
}

void LSnapSelectionActionBar::onLineWidthChanged(int width)
{
    // 通知覆盖窗口更新线条宽度
    //if (overlayWindow_) {
    //    overlayWindow_->setLineWidth(width);
    //}
}

void LSnapSelectionActionBar::addDefaultButtons()
{
	addButton(new SaveButton(this));
	addButton(new CopyButton(this));
	addButton(new PasteButton(this));
	addButton(new CancelButton(this));
	addButton(new DrawButton(this));
	addButton(new GifButton(this));
}

void LSnapSelectionActionBar::addButton(ActionButton* btn)
{
    if (!btn)
        return;

    m_pHButtonLayout->addWidget(btn);
    const QString t = btn->text();
    if (t.contains(QStringLiteral("save")))
        connect(btn, &ActionButton::activated, this, &LSnapSelectionActionBar::saveClicked);
    else if (t.contains(QStringLiteral("copy")))
        connect(btn, &ActionButton::activated, this, &LSnapSelectionActionBar::copyClicked);
    else if (t.contains(QStringLiteral("paste")))
        connect(btn, &ActionButton::activated, this, &LSnapSelectionActionBar::pasteClicked);
    else if (t.contains(QStringLiteral("cancel")))
        connect(btn, &ActionButton::activated, this, &LSnapSelectionActionBar::cancelClicked);
    else if (t.contains(QStringLiteral("draw")))
    {
        connect(btn, &ActionButton::activated, this, [this, btn]
            {
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
                        m_pVRootLayout->removeWidget(m_pDrawPanel);
                        m_pDrawPanel->hide();
                    }
                }
                emit drawClicked();
                adjustSize();
                if (!optionsVisible_)
                    emit shapeModeChanged(-1);
            });
    }
    //else if (t.contains("GIF", Qt::CaseInsensitive) || t.contains(QStringLiteral("GIF")))
    //{
    //    connect(btn, &ActionButton::activated, this, [this]{
    //        // 关闭绘图面板
    //        if (optionsVisible_) {
    //            optionsVisible_ = false;
    //            if (m_pDrawPanel) m_pDrawPanel->hide();
    //            emit shapeModeChanged(-1);
    //        }

    //        // 切换 GIF 面板
    //        const bool newVisible = !gifVisible_;
    //        gifVisible_ = newVisible;
    //        if (gifPanel_) {
    //            gifPanel_->setVisible(newVisible);
    //            if (!newVisible) emit gifCancel();
    //        }

    //        adjustSize();
    //    });
    //}
}

void LSnapSelectionActionBar::clearButtons()
{
    while (auto item = m_pHButtonLayout->takeAt(0))
    {
        if (auto w = item->widget()) w->deleteLater();
        delete item;
    }
}

void LSnapSelectionActionBar::updateDrawPanelPosition()
{
    if (!m_pDrawPanel)
        return;
    
    m_pVRootLayout->removeWidget(m_pDrawPanel);
    m_pVRootLayout->insertWidget(1, m_pDrawPanel);  // 索引1表示在按钮布局之后
}