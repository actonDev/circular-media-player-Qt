#include "lastfm.h"
#include "settings.h"
#include "Tools.h"
#include <QCryptographicHash>
#include <QUrlQuery>

namespace{
const QString API_KEY = "d84665f06309064914d4ae07488ea6e2";
const QString API_SECRET = "a75d7463d502eae07d8b1b435f966a07";
}

LastFm* LastFm::p_instance = 0;
QNetworkAccessManager LastFm::m_network;

QString LastFm::m_sessionKey;
QString LastFm::m_user;
QString LastFm::m_apiKey;
QString LastFm::m_apiSecret;

LastFm::LastFm(QObject *parent) :
	QObject(parent)
{
//	m_network = QNetworkAccessManager();
//	connect(&m_network, SIGNAL(finished(QNetworkReply*)), /*p_instance*/, SLOT(replyFinished(QNetworkReply*)));
}

LastFm* LastFm::instance()
{
	if(p_instance == 0 ){
		p_instance = new LastFm();
		connect(&m_network, SIGNAL(finished(QNetworkReply*)), p_instance, SLOT(replyFinished(QNetworkReply*)));
	}
	return p_instance;
}

void LastFm::setSession(QString user, QString sessionKey)
{
	instance();
	m_user = user;
	m_sessionKey = sessionKey;
}

void LastFm::authorize(QString user, QString pass)
{
	instance();
	qDebug() << "authorize";
	m_user = user;
	//	emit lastFmAuthorizationInfo("Authorizing");
	QString passMd5 = QString(QCryptographicHash::hash(pass.toLocal8Bit(),QCryptographicHash::Md5).toHex());
	QString userPassMd5 = user + passMd5;
	QString authToken = QString(QCryptographicHash::hash(userPassMd5.toLocal8Bit(),QCryptographicHash::Md5).toHex());

	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "authToken";
	pair.second = authToken;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "auth.getMobileSession";
	arguments.append(pair);
	pair.first = "username";
	pair.second = user;
	arguments.append(pair);

	QString signature = Tools::lastFmSignature(arguments);


	QUrl urlRequest;

	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+=arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + arguments.at(i).second.toLocal8Bit();
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFm::updateNowPlaying(QString artist, QString title, int duration)
{
	qDebug() << "update now playing, duration " << duration;
	if(m_sessionKey == "")
		return;
	if(artist == "Unknown Artist" && Settings::instance()->SKIP_UNKNOWN_ARTIST == true)
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	if(duration!=0){
		pair.first = "duration";
		pair.second = QString::number(duration);
		arguments.append(pair);
	}
	pair.first = "method";
	pair.second = "track.updateNowPlaying";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "track";
	pair.second = title;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);
}

void LastFm::scrobble(QString artist, QString title, int secSince1970)
{
	if(m_sessionKey == "")
		return;
	if(artist == "Unknown Artist" && Settings::instance()->SKIP_UNKNOWN_ARTIST == true)
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.scrobble";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "timestamp";
	pair.second = QString::number(secSince1970);
	arguments.append(pair);
	pair.first = "track";
	pair.second = title;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
//	qDebug() << urlRequest;
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);
}

void LastFm::trackAddTags(QString artist, QString track, QString tags)
{
	qDebug() << "track add tags";
	if(m_sessionKey == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.addTags";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "tags";
	pair.second = tags;
	arguments.append(pair);
	pair.first = "track";
	pair.second = track;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);

}

void LastFm::trackRemoveTag(QString artist, QString track, QString tag)
{
	qDebug() << "track remove tag";
	if(m_sessionKey == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.removeTag";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "tag";
	pair.second = tag;
	arguments.append(pair);
	pair.first = "track";
	pair.second = track;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);
}

void LastFm::loveTrack(QString artist, QString track)
{
	qDebug() << "love track";
	if(m_sessionKey == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.love";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "track";
	pair.second = track;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);

}

void LastFm::unloveTrack(QString artist, QString track)
{
	qDebug() << "unlove track";
	if(m_sessionKey == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.unlove";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = m_sessionKey;
	arguments.append(pair);
	pair.first = "track";
	pair.second = track;
	arguments.append(pair);
	QString signature = Tools::lastFmSignature(arguments);

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/";
	for(int i=0; i<arguments.count(); i++){
		if(i==0)
			byteRequest += "?";
		else
			byteRequest += "&";
		byteRequest+= arguments.at(i).first.toLocal8Bit();
		byteRequest+="=" + QUrl::toPercentEncoding( arguments.at(i).second.toLocal8Bit() );
	}
	byteRequest += "&api_sig=" + signature.toLocal8Bit();

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	request.setHeader( QNetworkRequest::ContentTypeHeader, "application/octet-stream" );
	m_network.post(request, byteRequest);
}

bool LastFm::authorized()
{
	if(m_sessionKey!="")
		return true;
	else
		return false;
}

void LastFm::replyFinished(QNetworkReply *reply)
{
		qDebug() << "REPLY FROM: " << reply->url();
	QByteArray response(reply->readAll());
	QString textReply(response);
	textReply.resize(500);
		qDebug() << textReply;

	QUrlQuery query(reply->url());
	QString method = query.queryItemValue("method");
	if(method == ""){
		reply->deleteLater();
		return;
	}
	reply->deleteLater();

	// Get lfm status frist of all, and key (if it exists in the answer)!
	QXmlStreamReader xml(response);
	QString status;
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "lfm"){
				QXmlStreamAttributes attributes = xml.attributes();
				status = attributes.value("status").toString();
			}
		}
		if(xml.name().toString() == "key"){
			QString key = xml.readElementText();
			m_sessionKey = key;
			//			qDebug() << "READ KEY: " << key;
		}
	}

	//### AUTHORIZATION REPLY
	if(method == "auth.getMobileSession"){
		if(status == "failed"){
			m_sessionKey = "";
		}
		//		qDebug() << "STATUS " << status;
		emit lastFmAuthorizationInfo(status);
	}
	//### SCROBLE REPLY
	if(method == "track.scrobble"){
		if(status == "ok"){
			QString artist = query.queryItemValue("artist");
			QString track = query.queryItemValue("track");
			emit infoSignal("Scrobbled: " + artist + " - " + track + ".");
		}
		else
			emit infoSignal("Scrobble failed!");
	}

	Settings::instance()->SESSION_KEY = m_sessionKey;
}
