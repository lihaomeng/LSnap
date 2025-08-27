#ifndef LSNAPOVERLAYWINDOW_H
#define LSNAPOVERLAYWINDOW_H

#include "lsnapdrawinglayer.h"
#include "lsnaphistory.h"
#include <QWidget>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QPainter>

class LSnapDrawingLayer;
class GifRecorder;

class LSnapOverlayWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LSnapOverlayWindow(QWidget* parent = nullptr);
    QRect selection() const { return m_selection; }

public slots:
    void onGifProgress(int currentFrame, int totalFrames);

signals:
    void canceled();
    void selectionFinished(const QRect& rect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    //press
    bool handleDrawingPress(QMouseEvent* e);
    bool handleResizePress(QMouseEvent* e);
    bool handleDraggingPress(QMouseEvent* e);
    bool handleExpandSelection(QMouseEvent* e);

    //move
    bool handleDrawingMove(QMouseEvent* e);
    bool handleResizeMove(QMouseEvent* e);
    bool handleDraggingMove(QMouseEvent* e);
    void handleHoverUpdate(const QPoint& pos);

    //release
    bool handleDrawingRelease(QMouseEvent* e);
    bool handleResizeRelease(QMouseEvent* e);
    bool handleDraggingRelease(QMouseEvent* e);

    enum ResizeHandle
    {
        None = 0,
        TopLeft = 1,
        Top = 2,
        TopRight = 3,
        Right = 4,
        BottomRight = 5,
        Bottom = 6,
        BottomLeft = 7,
        Left = 8,
        Move = 9
    };

    // init in captureScreen()
    qreal m_captureDpr = 1.0; // 1.5 my device
    QRect m_captureVirtualGeom; // (0,0 1707x1067)
    QPixmap m_screenShot;

    // button bar
    QWidget* m_pActionBar = nullptr;
    void createButtonBar();
    int m_snapStatus = 0; // 0 未选择  1 已经选择

    // about drag
    bool m_bDragging = false;
    QPoint m_pressPos;
    QPoint m_currrentPos;
    QRect m_selection;  //select region after press F1
    
    bool m_recordHollow = false; // 是否开启GIF录制

    ResizeHandle m_currentHandle = None;
    ResizeHandle m_hoveredHandle = None;

    //adjust size after capture
    int m_handleSize = 8;
    QPoint m_resizeStartPos;
    QRect m_resizeStartRect;

    LSnapDrawingLayer m_drawing; //绘图相关

    //History
    LSnapHistory* m_pHistory = nullptr;

    //GIF
    GifRecorder* m_pGifRecorder = nullptr;
    void captureScreen();
    void copySelectionToClipboard();
    QPixmap getSelectionPixmap();
    QPixmap getSelectionPixmap1(bool live = false);

    // 边界控制相关方法
    ResizeHandle getHandleAt(const QPoint& pos) const;
    QRect getHandleRect(ResizeHandle handle) const;
    void updateCursorForHandle(ResizeHandle handle);
    void drawResizeHandles(QPainter& painter);
    void updateAndShowActionBar();
    void hideActionBar();
 
private slots:
        void onShapeModeChanged(int mode); // 0=矩形 1=椭圆
        void onLineWidthChanged(int w);
};
#endif //LSNAPOVERLAYWINDOW_H