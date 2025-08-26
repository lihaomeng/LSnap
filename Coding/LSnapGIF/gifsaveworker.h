#ifndef GIFSAVEWORKER_H
#define GIFSAVEWORKER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QFutureWatcher>
#include "lsnapgif_export.h"

class LSNAPGIF_EXPORT GifSaveWorker : public QObject
{
    Q_OBJECT
public:
    explicit GifSaveWorker(QObject* parent = nullptr);
    ~GifSaveWorker();
    void saveGif(const QVector<QImage>& frames, const QString& outPath, int fps);
    void cancel();
    
signals:
    void progress(int currentFrame, int totalFrames);
    void finished(bool success, const QString& path);
    
private:
	QFutureWatcher<bool> m_watcher;
	std::atomic<bool> m_cancelRequested = false;
	QString m_currentOutPath;
    bool saveGifWithGifH(const QVector<QImage>& frames, const QString& outGifPath, int fps);
};
#endif // GIFSAVEWORKER_H
