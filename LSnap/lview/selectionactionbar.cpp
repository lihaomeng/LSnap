// lview/selectionactionbar.cpp
#include "selectionactionbar.h"
#include "gifpanel.h"
#include "drawpanel.h"
#include <QHBoxLayout>
#include <QPushButton>

SelectionActionBar::SelectionActionBar(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    rootLayout_ = new QVBoxLayout(this);
    rootLayout_->setContentsMargins(6, 4, 6, 4);
    rootLayout_->setSpacing(4);

    layout_ = new QHBoxLayout();
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(6);

    rootLayout_->addLayout(layout_);
    setStyleSheet("QWidget{background-color:rgba(40,40,40,200);border-radius:6px;}");

    drawPanel_ = new DrawPanel(this);
    rootLayout_->addWidget(drawPanel_);
    drawPanel_->hide();
    connect(drawPanel_, &DrawPanel::lineWidthChanged, this, &SelectionActionBar::lineWidthChanged);
    connect(drawPanel_, &DrawPanel::shapeModeChanged, this, &SelectionActionBar::shapeModeChanged);


    //GIF相关
    gifPanel_ = new GifPanel(this);
    rootLayout_->addWidget(gifPanel_);
    gifPanel_->hide();
    connect(gifPanel_, &GifPanel::startRequested, this, &SelectionActionBar::gifStart);
    connect(gifPanel_, &GifPanel::stopRequested,  this, &SelectionActionBar::gifStop);
    connect(gifPanel_, &GifPanel::cancelRequested,this, &SelectionActionBar::gifCancel);
}

void SelectionActionBar::addDefaultButtons()
{
	addButton(new SaveButton(this));
	addButton(new CopyButton(this));
	addButton(new PasteButton(this));
	addButton(new CancelButton(this));
	addButton(new DrawButton(this));
	addButton(new GifButton(this));
}

void SelectionActionBar::addButton(ActionButton* btn)
{
    if (!btn) return;
    layout_->addWidget(btn);
    const QString t = btn->text();
    if (t.contains(QStringLiteral("保存")))
        connect(btn, &ActionButton::activated, this, &SelectionActionBar::saveClicked);
    else if (t.contains(QStringLiteral("复制")))
        connect(btn, &ActionButton::activated, this, &SelectionActionBar::copyClicked);
    else if (t.contains(QStringLiteral("贴图")))
        connect(btn, &ActionButton::activated, this, &SelectionActionBar::pasteClicked);
    else if (t.contains(QStringLiteral("取消")))
        connect(btn, &ActionButton::activated, this, &SelectionActionBar::cancelClicked);
    else if (t.contains(QStringLiteral("绘图")))
    {
        connect(btn, &ActionButton::activated, this, [this]
        {
            // TODO 先检测其他的，将其他的面板全关闭
            optionsVisible_ = !optionsVisible_;
            if (drawPanel_) drawPanel_->setVisible(optionsVisible_);
            emit drawClicked();
            adjustSize();
            if (!optionsVisible_) emit shapeModeChanged(-1); // 收起时退出绘图模式
        });
    }
    else if (t.contains("GIF", Qt::CaseInsensitive) || t.contains(QStringLiteral("GIF")))
    {
        connect(btn, &ActionButton::activated, this, [this]{
            // 关闭绘图面板
            if (optionsVisible_) {
                optionsVisible_ = false;
                if (drawPanel_) drawPanel_->hide();
                emit shapeModeChanged(-1);
            }

            // 切换 GIF 面板
            const bool newVisible = !gifVisible_;
            gifVisible_ = newVisible;
            if (gifPanel_) {
                gifPanel_->setVisible(newVisible);
                if (!newVisible) emit gifCancel();
            }

            adjustSize();
        });
    }
}

void SelectionActionBar::clearButtons()
{
    while (auto item = layout_->takeAt(0))
    {
        if (auto w = item->widget()) w->deleteLater();
        delete item;
    }
}
