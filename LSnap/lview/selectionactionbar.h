#pragma once
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QToolButton>
#include <QMenu>

class QVBoxLayout;
class GifPanel;
class DrawPanel;
class ActionButton : public QPushButton
{
	Q_OBJECT
public:
	explicit ActionButton(const QString& text, QWidget* parent = nullptr)
		: QPushButton(text, parent)
	{
		setStyleSheet(
			"QPushButton{background-color:rgba(60,60,60,180);border:1px solid rgba(100,100,100,150);"
            "border-radius:4px;color:white;padding:6px 12px;font-size:11px;min-width:16px;min-height:12px;}"
			"QPushButton:hover{background-color:rgba(80,80,80,200);}"
			"QPushButton:pressed{background-color:rgba(100,100,100,200);}"
		);
		connect(this, &QPushButton::clicked, this, &ActionButton::activated);
	}

signals:
	void activated();
};

///////////////////////////////////////

class DrawButton : public ActionButton
{
	Q_OBJECT
public:
	explicit DrawButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("绘图"), parent) {}
};

///////////////////////////////////////

class SaveButton : public ActionButton
{
	Q_OBJECT
public:
	explicit SaveButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("保存"), parent) {}
};

///////////////////////////////////////

class CopyButton : public ActionButton
{
	Q_OBJECT
public:
	explicit CopyButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("复制"), parent) {}
};

///////////////////////////////////////

class PasteButton : public ActionButton
{
	Q_OBJECT
public:
	explicit PasteButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("贴图"), parent) {}
};

///////////////////////////////////////

class CancelButton : public ActionButton
{
	Q_OBJECT
public:
	explicit CancelButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("取消"), parent) {}
};

///////////////////////////////////////

class GifButton : public ActionButton
{
	Q_OBJECT
public:
	explicit GifButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("GIF"), parent) {}
};

///////////////////////////////////////

class SelectionActionBar : public QWidget
{
	Q_OBJECT
public:
    explicit SelectionActionBar(QWidget* parent);
    void addButton(ActionButton* btn);
    void clearButtons();
    void addDefaultButtons();

signals:
	void saveClicked();
	void copyClicked();
	void pasteClicked();
	void cancelClicked();
    void drawClicked();

    //GIF相关
    void gifStart(int fps, bool loop);
    void gifStop();
    void gifCancel();

    //二级控件
	void lineWidthChanged(int w);      // 新增：线宽变化
    void shapeModeChanged(int mode);   // 新增：形状模式变化  // 0 = 矩形，1 = 椭圆

private:
    QHBoxLayout* layout_ = nullptr;
    QVBoxLayout* rootLayout_ = nullptr;

    DrawPanel* drawPanel_ = nullptr;   // 绘图面板
    bool optionsVisible_ = false;

    //GIF相关
    GifPanel* gifPanel_ = nullptr;
    bool gifVisible_ = false;

    void setupOptionsPanel();

};
