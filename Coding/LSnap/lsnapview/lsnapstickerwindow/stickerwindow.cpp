#include "stickerwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>

StickerWindow::StickerWindow(const QPixmap& pixmap, QWidget* parent) 
    : QWidget(parent), pixmap_(pixmap)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool); // 设置窗口标志：无边框、置顶
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    if (!pixmap.isNull())
        resize(pixmap.size() / pixmap.devicePixelRatio());
    else
        resize(200, 150);
}

StickerWindow::~StickerWindow()
{
    pixmap_ = QPixmap();
}

void StickerWindow::setPixmap(const QPixmap& pixmap)
{
    pixmap_ = pixmap;
    if (!pixmap.isNull())
        resize(pixmap.size() / pixmap.devicePixelRatio());
    update();
}

void StickerWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(240, 240, 240, 200));
    if (!pixmap_.isNull())
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        QSizeF logical = QSizeF(pixmap_.size()) / pixmap_.devicePixelRatio();
        QSize widgetSz = size();
        QSizeF scaled = logical.scaled(widgetSz, Qt::KeepAspectRatio);
        int x = (widgetSz.width()  - int(scaled.width()))  / 2;
        int y = (widgetSz.height() - int(scaled.height())) / 2;
        QRect target(x, y, int(scaled.width()), int(scaled.height()));
        p.drawPixmap(target, pixmap_, pixmap_.rect());
    }
    else
    {
        p.drawText(rect(), Qt::AlignCenter, "no");
    }
    p.setPen(QPen(QColor(0, 120, 200), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
    if(m_isSelected)
        drawResizeHandles(p);
}

void StickerWindow::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        currentHandle_ = getHandleAt(e->pos());
        if (currentHandle_ != None)
        {
            resizeStartPos_ = e->pos();
            resizeStartRect_ = geometry();
            resizeStartGlobalPos_ = e->globalPos();
            e->accept();
            return;
        }
    }
    QWidget::mousePressEvent(e);
}

void StickerWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (currentHandle_ != None)
    {
        QPoint delta = e->globalPos() - resizeStartGlobalPos_; // 统一全局差值
        QRect newRect = resizeStartRect_;
        QRect screenBounds = QGuiApplication::primaryScreen()->virtualGeometry();

        // 计算贴图逻辑尺寸的纵横比
        qreal aspect = 1.0;
        if (!pixmap_.isNull()) {
            QSizeF logical = QSizeF(pixmap_.size()) / pixmap_.devicePixelRatio();
            if (logical.height() > 0.0) aspect = logical.width() / logical.height();
        }

        switch (currentHandle_)
        {
        case TopLeft:
        {
            QPoint anchor = resizeStartRect_.bottomRight();
            QPoint tl = resizeStartRect_.topLeft() + delta;
            // 从锚点到新角的向量
            int w = anchor.x() - tl.x();
            int h = anchor.y() - tl.y();
            // 按较主导的方向约束比例
            if (qAbs(w) >= qAbs(h * aspect)) {
                h = qRound(w / aspect);
            } else {
                w = qRound(h * aspect);
            }
            tl.setX(anchor.x() - w);
            tl.setY(anchor.y() - h);
            newRect = QRect(tl, anchor).normalized();
            break;
        }
        case TopRight:
        {
            QPoint anchor = resizeStartRect_.bottomLeft();
            QPoint tr = resizeStartRect_.topRight() + delta;
            int w = tr.x() - anchor.x();
            int h = anchor.y() - tr.y();
            if (qAbs(w) >= qAbs(h * aspect)) {
                h = qRound(w / aspect);
            } else {
                w = qRound(h * aspect);
            }
            tr.setX(anchor.x() + w);
            tr.setY(anchor.y() - h);
            newRect = QRect(anchor, tr).normalized();
            break;
        }
        case BottomRight:
        {
            QPoint anchor = resizeStartRect_.topLeft();
            QPoint br = resizeStartRect_.bottomRight() + delta;
            int w = br.x() - anchor.x();
            int h = br.y() - anchor.y();
            if (qAbs(w) >= qAbs(h * aspect)) {
                h = qRound(w / aspect);
            } else {
                w = qRound(h * aspect);
            }
            br.setX(anchor.x() + w);
            br.setY(anchor.y() + h);
            newRect = QRect(anchor, br).normalized();
            break;
        }
        case BottomLeft:
        {
            QPoint anchor = resizeStartRect_.topRight();
            QPoint bl = resizeStartRect_.bottomLeft() + delta;
            int w = anchor.x() - bl.x();
            int h = bl.y() - anchor.y();
            if (qAbs(w) >= qAbs(h * aspect)) {
                h = qRound(w / aspect);
            } else {
                w = qRound(h * aspect);
            }
            bl.setX(anchor.x() - w);
            bl.setY(anchor.y() + h);
            newRect = QRect(bl, anchor).normalized();
            break;
        }
        case Right:
            newRect.setRight(resizeStartRect_.right() + delta.x());
            break;
        case Left:
            newRect.setLeft(resizeStartRect_.left() + delta.x());
            break;
        case Top:
            newRect.setTop(resizeStartRect_.top() + delta.y());
            break;
        case Bottom:
            newRect.setBottom(resizeStartRect_.bottom() + delta.y());
            break;
        case Move:
            newRect = resizeStartRect_.translated(delta);
            // 边界限制
            if (newRect.left() < screenBounds.left()) newRect.moveLeft(screenBounds.left());
            else if (newRect.right() > screenBounds.right()) newRect.moveRight(screenBounds.right());
            if (newRect.top() < screenBounds.top()) newRect.moveTop(screenBounds.top());
            else if (newRect.bottom() > screenBounds.bottom()) newRect.moveBottom(screenBounds.bottom());
            setGeometry(newRect);
            e->accept();
            return;
        }

        if (newRect.width() < 20) newRect.setWidth(20);
        if (newRect.height() < 20) newRect.setHeight(20);
        newRect = newRect.intersected(screenBounds);

        setGeometry(newRect);
        e->accept();
        return;
    }
    
    ResizeHandle newHovered = getHandleAt(e->pos());
    if (newHovered != hoveredHandle_)
    {
        hoveredHandle_ = newHovered;
        updateCursorForHandle(hoveredHandle_);
    }
    
    QWidget::mouseMoveEvent(e);
}

void StickerWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (currentHandle_ != None)
        {
            currentHandle_ = None;
            e->accept();
            return;
        }
    }
    QWidget::mouseReleaseEvent(e);
}

void StickerWindow::drawResizeHandles(QPainter& painter)
{
    painter.save();
    
    QColor handleColor(0, 160, 255, 220);
    QColor handleBorder(255, 255, 255, 240);
    
    painter.setPen(QPen(handleBorder, 1));
    painter.setBrush(QBrush(handleColor));
    
    const ResizeHandle corners[] = { TopLeft, TopRight, BottomRight, BottomLeft };
    for (ResizeHandle handle : corners)
    {
        QRect handleRect = getHandleRect(handle);
        if (hoveredHandle_ == handle)
            painter.setBrush(QBrush(handleColor.lighter(130)));
        else
            painter.setBrush(QBrush(handleColor));
        painter.drawRect(handleRect);
    }
    painter.restore();
}

QRect StickerWindow::getHandleRect(ResizeHandle handle) const
{
    QRect r = rect();
    int half = handleSize_ / 2;
    int size = handleSize_ + 2;
    
    switch (handle)
    {
    case TopLeft:     return QRect(r.left() - half, r.top() - half, size, size);
    case Top:         return QRect(r.center().x() - half, r.top() - half, size, size);
    case TopRight:    return QRect(r.right() - half, r.top() - half, size, size);
    case Right:       return QRect(r.right() - half, r.center().y() - half, size, size);
    case BottomRight: return QRect(r.right() - half, r.bottom() - half, size, size);
    case Bottom:      return QRect(r.center().x() - half, r.bottom() - half, size, size);
    case BottomLeft:  return QRect(r.left() - half, r.bottom() - half, size, size);
    case Left:        return QRect(r.left() - half, r.center().y() - half, size, size);
    default:          return QRect();
    }
}

StickerWindow::ResizeHandle StickerWindow::getHandleAt(const QPoint& pos) const
{
	const ResizeHandle corners[] = { TopLeft, TopRight, BottomRight, BottomLeft };
	for (ResizeHandle handle : corners)
	{
		QRect r = getHandleRect(handle);
        r.adjust(-2, -2, 2, 2);
		if (r.contains(pos))
			return handle;
	}
	// 选区内部：整体移动
	if (rect().contains(pos))
		return Move;

	return None;
}

void StickerWindow::updateCursorForHandle(ResizeHandle handle)
{
    switch (handle)
    {
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Top:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Move:
        setCursor(Qt::SizeAllCursor);
        break;
    default:
        unsetCursor();
        break;
    }
}

void StickerWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}

void StickerWindow::enterEvent(QEvent* e)
{
    m_isSelected = true;
    update();
    QWidget::enterEvent(e);
}

void StickerWindow::leaveEvent(QEvent* e)
{
    m_isSelected = false;
    update();
    QWidget::leaveEvent(e);
}
