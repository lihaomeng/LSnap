#ifndef LSNAPPICSTICKERWINDOW_H
#define LSNAPPICSTICKERWINDOW_H

#include "lsnapstickerwindowbase.h"
#include <QPixmap>

class LSnapPicStickerWindow : public LSnapStickerWindowBase
{
    Q_OBJECT
public:
    explicit LSnapPicStickerWindow(const QPixmap& pixmap, QWidget* parent = nullptr);
    
    void setPixmap(const QPixmap& pixmap);
    QPixmap pixmap() const { return pixmap_; }
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void onGeometryChanged(const QRect& newGeometry) override;
    qreal getAspectRatio() const;

private:
    QPixmap pixmap_;
    void updateSizeFromPixmap();
};

#endif // LSNAPPICSTICKERWINDOW_H
