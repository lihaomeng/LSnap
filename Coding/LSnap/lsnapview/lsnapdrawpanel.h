#ifndef LSnapDrawPanel_H
#define LSnapDrawPanel_H

#include <QFrame>

namespace Ui {
class LSnapDrawPanel;
}

class LSnapDrawPanel : public QFrame
{
    Q_OBJECT
public:
    explicit LSnapDrawPanel(QWidget *parent = nullptr);
    ~LSnapDrawPanel();
    void setDrawingMode(int mode);
    int getDrawingMode() const;
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
    int m_currentDrawingMode = 0; // 0: no, 1: rect, 2: Ellipse
    int m_currentLineWidth = 2;
};
#endif // LSnapDrawPanel_H