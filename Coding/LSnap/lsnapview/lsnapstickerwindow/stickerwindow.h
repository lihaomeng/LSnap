#ifndef LSNAPSTICKWINDOW_H
#define LSNAPSTICKWINDOW_H
#include <QWidget>
#include <QPixmap>

class StickerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit StickerWindow(const QPixmap& pixmap, QWidget* parent = nullptr);
    
    void setPixmap(const QPixmap& pixmap);
    QPixmap pixmap() const { return pixmap_; }
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;

private:
    bool m_isSelected = true; //是否处于当前鼠标焦点位置
    QPoint resizeStartGlobalPos_;
    QPixmap pixmap_;
    bool dragging_ = false;
    QPoint dragStartPos_;
    
    // 边界调整相关
    enum ResizeHandle {
        None = 0,
        TopLeft = 1,
        Top = 2, 
        TopRight = 3,
        Right = 4,
        BottomRight = 5,
        Bottom = 6,
        BottomLeft = 7,
        Left = 8,
        Move = 9  // 整体移动
    };
    
    ResizeHandle currentHandle_ = None;
    ResizeHandle hoveredHandle_ = None;

    QPoint resizeStartPos_;
    QRect resizeStartRect_;
    int handleSize_ = 10;  // 拖拽控制点大小
    
    ResizeHandle getHandleAt(const QPoint& pos) const;
    QRect getHandleRect(ResizeHandle handle) const;
    void updateCursorForHandle(ResizeHandle handle);
    void drawResizeHandles(QPainter& painter);
};
#endif // LSNAPSTICKWINDOW_H