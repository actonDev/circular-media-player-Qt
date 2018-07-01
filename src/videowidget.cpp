#include "videowidget.h"
#include <QDebug>
#include <QSizePolicy>

VideoWidget::VideoWidget(QObject *parent) :
	QObject(parent)
{
	//m_path = Phonon::createPath(&m_mediaObject, &m_audioOutput);
//	Phonon::createPath(&m_mediaObject, &m_videoWidget);
//	connect(&m_mediaObject, SIGNAL(finished()), this, SLOT(onFinish()));
//	m_videoWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//	m_videoWidget.setMinimumHeight(400);
//	m_videoWidget.setMinimumWidth(900);
//	m_mediaObject.set
}

void VideoWidget::setSource(QString source)
{
//	m_mediaObject.setCurrentSource(source);
}

//Phonon::VideoWidget *VideoWidget::widget()
//{
//	return &m_videoWidget;
//}

void VideoWidget::play()
{
//	m_mediaObject.play();
}

void VideoWidget::stop()
{
//	m_mediaObject.stop();
}

void VideoWidget::onFinish()
{
	qDebug() << "video finish";
//	m_mediaObject.stop();
//	m_mediaObject.play();
}
