#ifndef LSNAPSELECTIONACTIONBAR_H
#define LSNAPSELECTIONACTIONBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QToolButton>

class QVBoxLayout;
class GifPanel;
class LSnapDrawPanel;

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
	explicit DrawButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("draw"), parent) {}
};

///////////////////////////////////////

class SaveButton : public ActionButton
{
	Q_OBJECT
public:
	explicit SaveButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("save"), parent) {}
};

///////////////////////////////////////

class CopyButton : public ActionButton
{
	Q_OBJECT
public:
	explicit CopyButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("copy"), parent) {}
};

///////////////////////////////////////

class PasteButton : public ActionButton
{
	Q_OBJECT
public:
	explicit PasteButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("paste"), parent) {}
};

///////////////////////////////////////

class CancelButton : public ActionButton
{
	Q_OBJECT
public:
	explicit CancelButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("cancel"), parent) {}
};

///////////////////////////////////////

class GifButton : public ActionButton
{
	Q_OBJECT
public:
	explicit GifButton(QWidget* parent = nullptr) : ActionButton(QStringLiteral("GIF"), parent) {}
};

///////////////////////////////////////

class LSnapSelectionActionBar : public QWidget
{
	Q_OBJECT
public:
    explicit LSnapSelectionActionBar(QWidget* parent);
    void addButton(ActionButton* btn);
    void clearButtons();
    void addDefaultButtons();

public slots:
	void onDrawingModeChanged(int mode);
	void onLineWidthChanged(int width);

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
	void lineWidthChanged(int w);
    void shapeModeChanged(int mode);   // 0 = 截图  1 = 矩形，2 = 椭圆

private:
    QVBoxLayout* m_pVRootLayout = nullptr;
	QHBoxLayout* m_pHButtonLayout = nullptr;
	LSnapDrawPanel* m_pDrawPanel = nullptr;   // 绘图面板
    bool optionsVisible_ = false;

    //GIF相关
    GifPanel* gifPanel_ = nullptr;
    bool gifVisible_ = false;

    void setupOptionsPanel();
	void updateDrawPanelPosition();
};
#endif //LSNAPSELECTIONACTIONBAR_H