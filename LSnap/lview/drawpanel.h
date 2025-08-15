// lview/drawpanel.h
#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QPushButton>

class DrawPanel : public QWidget
{
	Q_OBJECT
public:
	explicit DrawPanel(QWidget* parent = nullptr)
		: QWidget(parent)
	{
		auto* lay = new QHBoxLayout(this);
		lay->setContentsMargins(4, 2, 4, 2);
		lay->setSpacing(6);

		lineWidthBtn_ = new QToolButton(this);
		lineWidthBtn_->setText(QStringLiteral("线宽"));
		lineWidthBtn_->setPopupMode(QToolButton::InstantPopup);
		auto* menu = new QMenu(lineWidthBtn_);
		for (int w : {1,2,3,4,5}) {
			auto* act = menu->addAction(QString::number(w) + QStringLiteral(" px"));
			act->setData(w);
			connect(act, &QAction::triggered, this, [this, act]{
				bool ok = false; int w = act->data().toInt(&ok);
				if (ok) emit lineWidthChanged(w);
			});
		}
		lineWidthBtn_->setMenu(menu);

		rectBtn_ = new QPushButton(QStringLiteral("矩形"), this);
		ellipseBtn_ = new QPushButton(QStringLiteral("椭圆"), this);
		connect(rectBtn_,    &QPushButton::clicked, this, [this]{ emit shapeModeChanged(0); });
		connect(ellipseBtn_, &QPushButton::clicked, this, [this]{ emit shapeModeChanged(1); });

		lay->addWidget(lineWidthBtn_);
		lay->addWidget(rectBtn_);
		lay->addWidget(ellipseBtn_);
	}

signals:
	void lineWidthChanged(int w);
	void shapeModeChanged(int mode); // 0=矩形,1=椭圆

private:
	QToolButton* lineWidthBtn_ = nullptr;
	QPushButton* rectBtn_ = nullptr;
	QPushButton* ellipseBtn_ = nullptr;
};