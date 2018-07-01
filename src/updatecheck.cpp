#include "updatecheck.h"
#include <QDebug>
#include "settings.h"
#include <stdlib.h>
UpdateCheck::UpdateCheck(QObject *parent) :
	QObject(parent)
{


	QStringList list = QProcess::systemEnvironment();
	for(int i=list.count()-1; i>=0; i--){
		if(list.at(i).size()>50){
//			qDebug() << "remove " << list.at(i);
			list.takeAt(i);
		}
	}

//	qDebug() << "info: ";
//	for(int i=0; i<list.count(); i++){
//		qDebug() << list.at(i);
//	}


#ifdef Q_OS_WIN
	m_user = getenv("USERNAME");
#endif

#ifdef Q_OS_MAC
	m_user = getenv("USER");
#endif

	m_info += "_APP_VERSION=" + Settings::instance()->version+"\n";
	m_info += "_APP_DIR=" + QApplication::applicationDirPath();
	m_dateString = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");


	if(Settings::instance()->CHECK_UPDATES){
		QString urlVersion;
#ifdef Q_OS_WIN
		urlVersion = "http://circular-mp.sourceforge.net/info.php?method=getWinVersion";
#endif

#ifdef Q_OS_MAC
		urlVersion = "http://circular-mp.sourceforge.net/info.php?method=getMacVersion";
#endif
		QUrl urlRequest;
		QNetworkRequest request;
		QString userAgent = "CircularMediaPlayer/" + Settings::instance()->version + " (Nokia;Qt)";
		request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
		urlRequest.setUrl(urlVersion);
		request.setUrl(urlRequest);
		m_network.get(request);
	}

	connect(&m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(on_replyFinished(QNetworkReply*)));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(logKeepAlive()));

	if(Settings::instance()->SUBMIT_INFO){
		qDebug() << "submit info";
		logStart();
		m_timer.start( 300 *1000); //every x seconds
	}

}

UpdateCheck::~UpdateCheck()
{
//	qDebug() << "update check destrucotr";
	//	logEnd();
	logKeepAlive();
}

void UpdateCheck::on_replyFinished(QNetworkReply *reply)
{
	QByteArray response(reply->readAll());

	reply->deleteLater();
	QString textResponse(response);
	if(textResponse.size()>500)
		textResponse.resize(500);

	//	qDebug() << "server reply path" << reply->url().path();
	//	qDebug() << "server response: " << textResponse;

	if(!(reply->url().path()=="/info.php"))
		return;

	QString curVersion = Settings::instance()->version;
	qDebug() << "remote version: " << textResponse;
	if(textResponse >curVersion ){
		qDebug() << "update available, have older version";

		QString infoText = "<p>Newer version <b>" + textResponse + "</b> is now available! (currently running <b>" + curVersion + "</b>)</p>";
		infoText+= "<p>Go to <a href='http://circular-mp.sourceforge.net/'>circular-mp.sourceforge.net</a> to dowload now!";
		m_dialogUpdate.setText(infoText);
		m_dialogUpdate.exec();

	}
	else if(textResponse < curVersion){
		qDebug() << "have newer version";
	}
	else if(textResponse == curVersion){
		qDebug() << "have same version";
	}

}

void UpdateCheck::logKeepAlive()
{
	qDebug() << "logKeepAlive";
	QByteArray byteRequest = "http://circular-mp.sourceforge.net/log.php?method=logKeepAlive"; //method
	byteRequest += "&info=";
	byteRequest += QUrl::toPercentEncoding(m_info);
	byteRequest += "&date=";
	byteRequest += QUrl::toPercentEncoding(m_dateString);


	QUrl urlRequest;
	urlRequest = QUrl::fromEncoded(byteRequest);
	QNetworkRequest request;
	QString userAgent = "CircularMediaPlayer/" + Settings::instance()->version + " (Nokia;Qt)";
	request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
	request.setUrl(urlRequest);
	m_network.get(request);
}

void UpdateCheck::logStart()
{
	QByteArray byteRequest = "http://circular-mp.sourceforge.net/log.php?method=logStart"; //method
	byteRequest += "&info=";
	byteRequest += QUrl::toPercentEncoding(m_info);
	byteRequest += "&date=";
	byteRequest += QUrl::toPercentEncoding(m_dateString);
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(m_user);
	QUrl urlRequest;

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	QString userAgent = "CircularMediaPlayer/" + Settings::instance()->version + " (Nokia;Qt)";
	request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
	request.setUrl(urlRequest);
	m_network.get(request);
}

void UpdateCheck::logEnd()
{
	QByteArray byteRequest = "http://circular-mp.sourceforge.net/log.php?method=logEnd"; //method
	byteRequest += "&info=";
	byteRequest += QUrl::toPercentEncoding(m_info);
	byteRequest += "&date=";
	byteRequest += QUrl::toPercentEncoding(m_dateString);

	QUrl urlRequest;
	urlRequest = QUrl::fromEncoded(byteRequest);
	QNetworkRequest request;
	QString userAgent = "CircularMediaPlayer/" + Settings::instance()->version + " (Nokia;Qt)";
	request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
	request.setUrl(urlRequest);
	m_network.get(request);
}
