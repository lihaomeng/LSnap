#include "lsnappicstickerwindow.h"
#include <QPainter>

LSnapPicStickerWindow::LSnapPicStickerWindow(const QPixmap& pixmap, QWidget* parent)
    : LSnapStickerWindowBase(parent)
    , pixmap_(pixmap)
{
    updateSizeFromPixmap();
}

void LSnapPicStickerWindow::setPixmap(const QPixmap& pixmap)
{
    pixmap_ = pixmap;
    updateSizeFromPixmap();
    update();
}

void LSnapPicStickerWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(240, 240, 240, 200));
    if (!pixmap_.isNull())
    {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        QSizeF logical = QSizeF(pixmap_.size()) / pixmap_.devicePixelRatio();
        QSize widgetSz = size();
        QSizeF scaled = logical.scaled(widgetSz, Qt::KeepAspectRatio);
        int x = (widgetSz.width() - int(scaled.width())) / 2;
        int y = (widgetSz.height() - int(scaled.height())) / 2;
        QRect target(x, y, int(scaled.width()), int(scaled.height()));
        p.drawPixmap(target, pixmap_, pixmap_.rect());
    }
    else
    {
        p.drawText(rect(), Qt::AlignCenter, "No Image");
    }
    
    p.setPen(QPen(QColor(0, 120, 200), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(rect().adjusted(0, 0, -1, -1));
    
    drawResizeHandles(p);
}

void LSnapPicStickerWindow::onGeometryChanged(const QRect& newGeometry)
{
    //Q_UNUSED(newGeometry);
}

qreal LSnapPicStickerWindow::getAspectRatio() const
{
    if (pixmap_.isNull())
        return 1.0;
    
    QSizeF logical = QSizeF(pixmap_.size()) / pixmap_.devicePixelRatio();
    return logical.width() / logical.height();
}

void LSnapPicStickerWindow::updateSizeFromPixmap()
{
    if (!pixmap_.isNull())
    {
        QSizeF logical = QSizeF(pixmap_.size()) / pixmap_.devicePixelRatio();
        resize(logical.toSize());
    }
    else
    {
        resize(200, 150);
    }
}