#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QObject>
//#include <phonon>

class VideoWidget : public QObject
{
	Q_OBJECT
private:
//	Phonon::AudioOutput m_audioOutput;
//	Phonon::MediaObject m_mediaObject;
//	Phonon::Path m_path;
//	Phonon::VideoWidget m_videoWidget;
public:
	explicit VideoWidget(QObject *parent = 0);
	void setSource(QString source);
//	Phonon::VideoWidget *widget();
	void play();
	void stop();
	
signals:

private slots:
	void onFinish();
	
public slots:
	
};

#endif // VIDEOWIDGET_H
