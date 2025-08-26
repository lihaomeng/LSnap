#include "gifsaveworker.h"
#include "thirdparty/gif.h"
#include <QFuture>
#include <QtConcurrent>

GifSaveWorker::GifSaveWorker(QObject* parent) : QObject(parent)
{
	connect(&m_watcher, &QFutureWatcher<bool>::finished, this, [this]() {
		const bool ok = m_watcher.isCanceled() ? false : m_watcher.result();
		emit finished(ok, m_currentOutPath);
	});
}

GifSaveWorker::~GifSaveWorker()
{
	cancel();
	if (m_watcher.isRunning())
		m_watcher.waitForFinished();
}

void GifSaveWorker::saveGif(const QVector<QImage>& frames, const QString& outPath, int fps)
{
	//先关闭正在运行的GIF
	cancel();
	if (m_watcher.isRunning())
		m_watcher.waitForFinished();

	m_cancelRequested = false;
	m_currentOutPath = outPath;

	QFuture<bool> fut = QtConcurrent::run([this, frames, outPath, fps]() {
		return saveGifWithGifH(frames, outPath, fps);
	});
	m_watcher.setFuture(fut);
}

void GifSaveWorker::cancel()
{
	m_cancelRequested = true;
	if (m_watcher.isRunning())
		m_watcher.cancel();
}

bool GifSaveWorker::saveGifWithGifH(const QVector<QImage>& frames, const QString& outGifPath, int fps)
{
	if (frames.isEmpty())
		return false;

	const int w = frames.first().width();
	const int h = frames.first().height();
	const int delayCs = qMax(1, int(100.0 / double(qMax(1, fps))));
	GifWriter writer{};
	if (!GifBegin(&writer, outGifPath.toLocal8Bit().constData(), w, h, delayCs))
		return false;

	QByteArray rgba;
	rgba.resize(w * h * 4);
	for (int i = 0; i < frames.size(); ++i)
	{
		if (m_cancelRequested.load(std::memory_order_relaxed))
		{
			GifEnd(&writer);
			return false;
		}
		const QImage& im0 = frames[i];
		QImage im = (im0.format() == QImage::Format_RGBA8888) ? im0 : im0.convertToFormat(QImage::Format_RGBA8888);
		uint8_t* dst = reinterpret_cast<uint8_t*>(rgba.data());
		for (int y = 0; y < h; ++y)
		{
			const uint8_t* src = im.constScanLine(y);
			memcpy(dst + y * w * 4, src, size_t(w) * 4);
		}
		if (!GifWriteFrame(&writer, reinterpret_cast<uint8_t*>(rgba.data()), w, h, delayCs))
		{
			GifEnd(&writer);
			return false;
		}
		emit progress(i + 1, frames.size());
	}
	GifEnd(&writer);
	return true;
}