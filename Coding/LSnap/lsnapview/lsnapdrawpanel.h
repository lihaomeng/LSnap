#ifndef LSnapDrawPanel_H
#define LSnapDrawPanel_H

#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class LSnapDrawPanel;
}
QT_END_NAMESPACE

class LSnapDrawPanel : public QFrame
{
    Q_OBJECT

public:
    explicit LSnapDrawPanel(QWidget *parent = nullptr);
    ~LSnapDrawPanel();

    // 获取UI控件的方法
    QPushButton* getLineWidthBtn() const;
    QPushButton* getRectBtn() const;
    QPushButton* getEllipseBtn() const;

    // 设置绘图模式
    void setDrawingMode(int mode); // 0: 无, 1: 矩形, 2: 椭圆
    int getDrawingMode() const;

    // 设置线条宽度
    void setLineWidth(int width);
    int getLineWidth() const;

signals:
    void drawingModeChanged(int mode);
    void lineWidthChanged(int width);
    void rectButtonClicked();
    void ellipseButtonClicked();
    void lineWidthButtonClicked();

private slots:
    void onRectBtnClicked();
    void onEllipseBtnClicked();
    void onLineWidthBtnClicked();

private:
    void setupConnections();
    void updateButtonStates();

private:
    Ui::LSnapDrawPanel *ui;

    int currentDrawingMode_ = 0; // 0: 无, 1: 矩形, 2: 椭圆
    int currentLineWidth_ = 2;
};

#endif // LSnapDrawPanel_H
