// lview/gifpanel.h
#pragma once
#include <QWidget>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>

// TODO 先实现开始和结束按钮，点开始时开始录制，结束停止录制，并生成GIF

class GifPanel : public QWidget
{
	Q_OBJECT
public:
    explicit GifPanel(QWidget* parent);

	// 录制过程中外部可更新帧数与尺寸（仅显示）
	void setFrameInfo(int frames, const QSize& sz)
	{
		infoLabel_->setText(QStringLiteral("%1 帧 %2x%3").arg(frames).arg(sz.width()).arg(sz.height()));
	}

signals:
	void startRequested(int fps, bool loop);
	void stopRequested();
	void cancelRequested();

private slots:
    void updateUi();

private:
	QComboBox* fpsBox_ = nullptr;
	QCheckBox* loopBox_ = nullptr;
	QLabel* timeLabel_ = nullptr;
	QLabel* infoLabel_ = nullptr;
	QPushButton* startBtn_ = nullptr;
	QPushButton* stopBtn_ = nullptr;
	QPushButton* cancelBtn_ = nullptr;
	QTimer* timer_ = nullptr;
	QElapsedTimer elapsed_;
	bool running_ = false;
};
