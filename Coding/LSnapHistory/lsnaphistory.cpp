#include "lsnaphistory.h"
#include <QtConcurrent>
#include <QStandardPaths>

LSnapHistory::LSnapHistory(QObject *parent)
    : QObject(parent)
    , m_saveWatcher(new QFutureWatcher<bool>(this))
{
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (defaultDir.isEmpty())
        defaultDir = QDir::homePath() + "/Pictures";
    setSaveDirectory(defaultDir + "/LSnap");
    
    connect(m_saveWatcher, &QFutureWatcher<bool>::finished, this, &LSnapHistory::onSaveCompleted);
}

LSnapHistory::~LSnapHistory()
{
    if (m_saveWatcher->isRunning())
        m_saveWatcher->waitForFinished();
}

void LSnapHistory::setSaveDirectory(const QString &directory)
{
    m_saveDirectory = directory;
    ensureDirectoryExists(directory);
}

QString LSnapHistory::getSaveDirectory() const
{
    return m_saveDirectory;
}

QFuture<bool> LSnapHistory::saveImageAsync(const QPixmap &image, const QString &customName)
{
    if (m_saveWatcher->isRunning())
    {
        QFuture<bool> future;
        return future;
    }
    QString fileName = generateFileName(customName);
    m_currentSavePath = QDir(m_saveDirectory).absoluteFilePath(fileName);
    if (!ensureDirectoryExists(m_saveDirectory))
    {
        QFuture<bool> future;
        return future;
    }
    QFuture<bool> future = QtConcurrent::run(saveImageToFile, image, m_currentSavePath);
    m_saveWatcher->setFuture(future);
    return future;
}

bool LSnapHistory::isSaving() const
{
    return m_saveWatcher->isRunning();
}

void LSnapHistory::onSaveCompleted()
{
    bool success = m_saveWatcher->result();
    emit saveCompleted(success, m_currentSavePath);
}

QString LSnapHistory::generateFileName(const QString &customName) const
{
    QString fileName;
    if (!customName.isEmpty())
    {
        fileName = customName;
        QDateTime now = QDateTime::currentDateTime();
        fileName += QString("LSnap_%1.png").arg(now.toString("yyyy-MM-dd_hh-mm-ss"));
        if (!fileName.endsWith(".png"))
            fileName += ".png";
    }
    return fileName;
}

bool LSnapHistory::ensureDirectoryExists(const QString &directory) const
{
    QDir dir(directory);
    if (!dir.exists())
        return dir.mkpath(".");
    return true;
}

bool LSnapHistory::saveImageToFile(const QPixmap &image, const QString &filePath)
{
    if (image.isNull())
        return false;
    bool success = image.save(filePath, "PNG", 100);
    return success;
}