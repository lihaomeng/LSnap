#ifndef LSNAP_SCREEN_CAPTURE_H
#define LSNAP_SCREEN_CAPTURE_H

#include <QObject>
#include <QPixmap>
#include <QRect>

class LSnapScreenCapture : public QObject
{
	Q_OBJECT
public:
	explicit LSnapScreenCapture(QObject* parent = nullptr);
	void captureScreen();
	qreal dpr() const { return m_dpr; }
	QRect virtualGeometry() const { return m_virtualGeom; }
	const QPixmap& snapshot() const { return m_snapshot; }

    QRect mapOverlayToVirtual(const QRect& overlayRect, const QPoint& overlayGlobalTopLeft) const;
	QPixmap cropFromImageVirtual(const QImage& composed, const QRect& virtualRect) const;
	QPixmap grabRegion(const QRect& virtualRect) const;
	QPixmap grabRegionLive(const QRect& virtualRect) const;
private:
	qreal m_dpr = 1.0;
	QRect m_virtualGeom;
	QPixmap m_snapshot;
};

#endif // LSNAP_SCREEN_CAPTURE_H