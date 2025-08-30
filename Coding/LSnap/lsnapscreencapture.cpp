#include "lsnapscreencapture.h"
#include <QGuiApplication>
#include <QScreen>

LSnapScreenCapture::LSnapScreenCapture(QObject* parent)
	: QObject(parent)
{
}

void LSnapScreenCapture::captureScreen()
{
	QScreen* scr = QGuiApplication::primaryScreen();
	if (!scr)
	{
		m_dpr = 1.0;
		m_virtualGeom = QRect();
		m_snapshot = QPixmap();
		return;
	}
	m_virtualGeom = scr->virtualGeometry();
	m_dpr = scr->devicePixelRatio();
	const QRect v = m_virtualGeom;
	m_snapshot = scr->grabWindow(0, v.x(), v.y(), v.width(), v.height()); //need to change stragery,use grabWindow cause singele pic is big
	m_snapshot.setDevicePixelRatio(m_dpr);
}

QPixmap LSnapScreenCapture::grabRegion(const QRect& virtualRect) const
{
	if (m_snapshot.isNull() || !virtualRect.isValid())
		return QPixmap();

	const QRect localRect = virtualRect.translated(-m_virtualGeom.topLeft());
	const QRect bounds(QPoint(0, 0), m_snapshot.size());
	const QRect valid = localRect.intersected(bounds);
	if (valid.isEmpty())
		return QPixmap();

	QPixmap out = m_snapshot.copy(valid);
	out.setDevicePixelRatio(m_dpr);
	return out;
}

QPixmap LSnapScreenCapture::grabRegionLive(const QRect& virtualRect) const
{
	QScreen* scr = QGuiApplication::primaryScreen();
	if (!scr || !virtualRect.isValid())
		return QPixmap();

	QPixmap p = scr->grabWindow(0, virtualRect.x(), virtualRect.y(), virtualRect.width(), virtualRect.height());
	p.setDevicePixelRatio(scr->devicePixelRatio());
	return p;
}

QRect LSnapScreenCapture::mapOverlayToVirtual(const QRect& overlayRect, const QPoint& overlayGlobalTopLeft) const
{
	return overlayRect.translated(overlayGlobalTopLeft - m_virtualGeom.topLeft());
}

QPixmap LSnapScreenCapture::cropFromImageVirtual(const QImage& composed, const QRect& virtualRect) const
{
	if (composed.isNull() || !virtualRect.isValid())
		return QPixmap();

	QRect physicalSel(
		qRound(virtualRect.x() * m_dpr),
		qRound(virtualRect.y() * m_dpr),
		qRound(virtualRect.width() * m_dpr),
		qRound(virtualRect.height() * m_dpr)
	);
	QRect valid = physicalSel.intersected(QRect(QPoint(0,0), composed.size()));
	if (valid.isEmpty())
		return QPixmap();

	QPixmap out = QPixmap::fromImage(composed.copy(valid));
	out.setDevicePixelRatio(m_dpr);
	return out;
}
