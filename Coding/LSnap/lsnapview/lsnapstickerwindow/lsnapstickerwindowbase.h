#ifndef LSNAPSTICKERWINDOWBASE_H
#define LSNAPSTICKERWINDOWBASE_H

#include <QWidget>
#include <QPoint>
#include <QRect>

class QMouseEvent;
class QKeyEvent;
class QPaintEvent;
class QPainter;

class LSnapStickerWindowBase : public QWidget
{
    Q_OBJECT
public:
    explicit LSnapStickerWindowBase(QWidget* parent = nullptr);
    virtual ~LSnapStickerWindowBase();

    // 公共接口
    void setSelected(bool selected);
    bool isSelected() const { return m_isSelected; }
    
    // 设置窗口大小
    void setStickerSize(const QSize& size);
    QSize getStickerSize() const;

protected:
    // 事件处理
    void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;
    void enterEvent(QEvent* e) override;
    void leaveEvent(QEvent* e) override;
    
    // 纯虚函数，子类必须实现
    virtual void paintEvent(QPaintEvent* event) override = 0;
    
    // 子类可以重写的虚函数
    virtual void onGeometryChanged(const QRect& newGeometry);
    virtual void onSelectionChanged(bool selected);
    void drawResizeHandles(QPainter& painter);
    
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
    
    // 保护成员变量
    bool m_isSelected = false;
    QPoint resizeStartGlobalPos_;
    bool dragging_ = false;
    QPoint dragStartPos_;
    
    ResizeHandle currentHandle_ = None;
    ResizeHandle hoveredHandle_ = None;
    QPoint resizeStartPos_;
    QRect resizeStartRect_;
    int handleSize_ = 10;  // 拖拽控制点大小

private:
    // 私有辅助方法
    ResizeHandle getHandleAt(const QPoint& pos) const;
    QRect getHandleRect(ResizeHandle handle) const;
    void updateCursorForHandle(ResizeHandle handle);
    
    void handleResizeMove(QMouseEvent* e);
    void handleMoveOperation(QMouseEvent* e);
    void handleCornerResize(QMouseEvent* e, ResizeHandle handle);
    void handleEdgeResize(QMouseEvent* e, ResizeHandle handle);
    qreal getAspectRatio() const;
    QRect constrainToScreen(const QRect& rect) const;
    void applyMinimumSize(QRect& rect) const;
};

#endif // LSNAPSTICKERWINDOWBASE_H