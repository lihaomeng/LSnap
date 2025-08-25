#include "gifsaveworker.h"
//#include "thirdparty/gif.h"

GifSaveWorker::GifSaveWorker(QObject* parent) : QObject(parent)
{
}

//GifSaveWorker::~GifSaveWorker()
//{
//    cancel();
//}
//
//void GifSaveWorker::saveGif(const QVector<QImage>& frames, const QString& outPath, int fps, bool loop)
//{
//    // 重置取消标志
//    {
//        QMutexLocker locker(&cancelMutex_);
//        cancelRequested_ = false;
//    }
//    
//    bool success = saveGifWithGifH(frames, outPath, fps, loop);
//    emit finished(success, outPath);
//}
//
//void GifSaveWorker::cancel()
//{
//    QMutexLocker locker(&cancelMutex_);
//    cancelRequested_ = true;
//}
//
//bool GifSaveWorker::saveGifWithGifH(const QVector<QImage>& frames, const QString& outGifPath, int fps, bool loop)
//{
//    if (frames.isEmpty()) return false;
//    
//    const int w = frames.first().width();
//    const int h = frames.first().height();
//    const int delayCs = qMax(1, int(100.0 / double(qMax(1, fps)))); // 1/100秒
//
//    GifWriter writer{};
//    if (!GifBegin(&writer, outGifPath.toLocal8Bit().constData(), w, h, delayCs)) {
//        return false;
//    }
//
//    QByteArray rgba;
//    rgba.resize(w * h * 4);
//
//    for (int i = 0; i < frames.size(); ++i) {
//        // 检查是否被取消
//        {
//            QMutexLocker locker(&cancelMutex_);
//            if (cancelRequested_) {
//                GifEnd(&writer);
//                return false;
//            }
//        }
//        
//        const QImage& im0 = frames[i];
//        QImage im = (im0.format() == QImage::Format_RGBA8888)
//        ? im0
//        : im0.convertToFormat(QImage::Format_RGBA8888);
//
//        // 拷贝成无 stride 的连续 RGBA 缓冲
//        uint8_t* dst = reinterpret_cast<uint8_t*>(rgba.data());
//        for (int y = 0; y < h; ++y) {
//            const uint8_t* src = im.constScanLine(y);
//            memcpy(dst + y * w * 4, src, size_t(w) * 4);
//        }
//        
//        if (!GifWriteFrame(&writer, reinterpret_cast<uint8_t*>(rgba.data()), w, h, delayCs)) {
//            GifEnd(&writer);
//            return false;
//        }
//        
//        // 发送进度信号
//        emit progress(i + 1, frames.size());
//    }
//    
//    GifEnd(&writer);
//    return true;
//}