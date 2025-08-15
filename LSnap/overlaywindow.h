// overlaywindow.h
#pragma once
#include <QWidget>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QPainter>

class QKeyEvent;
class QShowEvent;
class QHideEvent;
class QMouseEvent;
class DrawingLayer;
class GifRecorder;

class DrawingLayer
{
public:
    enum class ShapeType { None, Rectangle, Ellipse };
    struct Item {
        ShapeType type;
        QRect rect;
        int penWidth;
        QColor color;
    };

    void setMode(ShapeType m) { mode_ = m; drawing_ = false; }
    void setModeFromInt(int mode) { mode_ = (mode == 0 ? ShapeType::Rectangle : mode == 1 ? ShapeType::Ellipse : ShapeType::None); drawing_ = false; }
    void setPenWidth(int w) { penWidth_ = qMax(1, qMin(w, 20)); }
    bool isActive() const { return mode_ != ShapeType::None; }
    bool isDrawing() const { return drawing_; }

    bool press(const QPoint& pos, Qt::MouseButton button)
    {
        if (!isActive() || button != Qt::LeftButton) return false;
        drawing_ = true; start_ = end_ = pos; return true;
    }

    bool move(const QPoint& pos)
    {
        if (!drawing_) return false;
        end_ = pos; return true;
    }

    bool release(const QPoint& pos, Qt::MouseButton button)
    {
        if (!drawing_ || button != Qt::LeftButton) return false;
        drawing_ = false;
        QRect r = QRect(start_, pos).normalized();
        if (!r.isEmpty() && mode_ != ShapeType::None) {
            items_.append(Item{ mode_, r, penWidth_, color_ });
        }
        return true;
    }

    void paint(QPainter& p) const
    {
        for (const auto& it : items_)
        {
            p.setPen(QPen(it.color, it.penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            if (it.type == ShapeType::Rectangle) p.drawRect(it.rect);
            else if (it.type == ShapeType::Ellipse) p.drawEllipse(it.rect);
        }
        if (drawing_ && mode_ != ShapeType::None)
        {
            QRect pv = QRect(start_, end_).normalized();
            p.setPen(QPen(color_, penWidth_, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));
            if (mode_ == ShapeType::Rectangle) p.drawRect(pv);
            else p.drawEllipse(pv);
        }
    }

    void paintCommitted(QPainter& p) const
    {
        for (const auto& it : items_) {
            p.setPen(QPen(it.color, it.penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            if (it.type == ShapeType::Rectangle) p.drawRect(it.rect);
            else if (it.type == ShapeType::Ellipse) p.drawEllipse(it.rect);
            else /* None */ {}
        }
    }

    void paintCommittedMapped(QPainter& p, const QPointF& offsetLogical, qreal dpr) const
    {
        for (const auto& it : items_)
        {
            const int wpx = qMax(1, int(std::lround(it.penWidth * dpr)));
            p.setPen(QPen(it.color, wpx, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
            QRectF rf = it.rect;
            rf.translate(offsetLogical);
            rf = QRectF(rf.left()  , rf.top() , rf.width() , rf.height() );
            if (it.type == ShapeType::Rectangle) p.drawRect(rf);
            else if (it.type == ShapeType::Ellipse) p.drawEllipse(rf);
        }
    }
        
private:
    ShapeType mode_ = ShapeType::None;
    int penWidth_ = 2;
    QColor color_ = QColor(0, 180, 255);
    bool drawing_ = false;
    QPoint start_, end_;
    QVector<Item> items_;
};

/////////////////////////////////////////////////////

class OverlayWindow : public QWidget
{
    Q_OBJECT
public:
    explicit OverlayWindow(QWidget* parent = nullptr);
    QRect selection() const { return selection_; }

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
    qreal m_captureDpr = 1.0; // 1.5
    QRect m_captureVirtualGeom; // (0,0 1707x1067)
    QWidget* m_pActionBar = nullptr;
    int m_snapStatus = 0;
    void createButtonBar();

    bool dragging_ = false;
    QPoint pressPos_;
    QPoint currPos_;
    QRect selection_;
    QPixmap m_screenShot;
    
    // 边界拖动控制
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
    int handleSize_ = 8;  // 拖拽控制点大小
    

    void captureScreen();
    void copySelectionToClipboard();
    QPixmap getSelectionPixmap();
    //QPixmap getSelectionPixmap(bool recapture = false);
    bool recordHollow_ = false;
    QPixmap getSelectionPixmap1(bool live = false);


    // 边界控制相关方法
    ResizeHandle getHandleAt(const QPoint& pos) const;
    QRect getHandleRect(ResizeHandle handle) const;
    void updateCursorForHandle(ResizeHandle handle);
    void drawResizeHandles(QPainter& painter);
    

    void updateAndShowActionBar();
    void hideActionBar();

    //绘图相关
	DrawingLayer drawing_;

    //GIF相关
    GifRecorder* m_pGifRecorder = nullptr;

private slots:
        void onShapeModeChanged(int mode); // 0=矩形 1=椭圆
        void onLineWidthChanged(int w);
};
