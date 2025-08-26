#ifndef LSNAPSELECTIONACTIONBAR_H
#define LSNAPSELECTIONACTIONBAR_H

#include <QWidget>

class LSnapGifPanel;
class LSnapDrawPanel;

namespace Ui {
	class LSnapSelectionActionBar;
}

class LSnapSelectionActionBar : public QWidget
{
	Q_OBJECT
public:
    explicit LSnapSelectionActionBar(QWidget* parent);

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
    void gifStartForOverlayWindow();
    void gifStopForOverlayWindow();
    void gifCancel();

    //二级控件
	void lineWidthChanged(int w);
    void shapeModeChanged(int mode);   // 0 = 截图  1 = 矩形，2 = 椭圆

private:
	LSnapDrawPanel* m_pDrawPanel = nullptr;   // 绘图面板
    bool optionsVisible_ = false;
	Ui::LSnapSelectionActionBar* m_ui = nullptr;
    //GIF相关
    LSnapGifPanel* m_pGifPanel = nullptr;
    bool m_gifVisible = false;

	void updateDrawPanelPosition();
	void updateGifPanelPosition();
};
#endif //LSNAPSELECTIONACTIONBAR_H