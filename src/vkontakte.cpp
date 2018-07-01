#include "vkontakte.h"
#include <QDebug>
#include <QToolTip>
#include "objectmanager.h"
#include "tools.h"
#include "settings.h"

namespace{
const QString VKONTAKTE_ACCESS_TOKEN = "your vkontakte access token";
}

Vkontakte *Vkontakte::p_instance = 0;
QNetworkAccessManager Vkontakte::m_network;
QNetworkReply *Vkontakte::p_searchReply;
QList<Node *> Vkontakte::m_requestList;
QTimer Vkontakte::m_timer;

Vkontakte::Vkontakte(QObject *parent) :
	QObject(parent)
{
	qDebug() << "Vkontakte constructor!";
}

void Vkontakte::search(Node *sender)
{
	instance();
	qDebug() << "vkontakte search!";
	m_requestList.append(sender);
	restartTimer();
}

void Vkontakte::search(QString term)
{
	QByteArray byteRequest = "https://api.vk.com/method/audio.search.xml?q=";
	QUrl urlRequest;
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&access_token=";
	byteRequest += VKONTAKTE_ACCESS_TOKEN;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	qDebug() << request.rawHeaderList();
	p_searchReply = m_network.get(request);
	connect(p_searchReply, SIGNAL(finished()), instance(), SLOT(on_replyFinished()));

}

Vkontakte *Vkontakte::instance()
{
	if(p_instance == 0){
		p_instance = new Vkontakte();
		connect(&m_timer, SIGNAL(timeout()), p_instance, SLOT(on_timerSearch()));
		m_timer.setSingleShot(true);
	}
	return p_instance;
}

void Vkontakte::on_replyFinished(Node *sender, QNetworkReply *reply)
{
	restartTimer();

	QByteArray response = reply->readAll();
	QString responseText(response);
	responseText.resize(100);
	QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	qDebug() << "code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	qDebug() << "Location: " << reply->rawHeader("Location");
	qDebug() << "location header: " <<reply->header(QNetworkRequest::LocationHeader);
	qDebug() << "reply:  " << response;
	QString host = reply->url().host();
	reply->deleteLater();

	if(!ObjectManager::objectDynamicExists(sender))
		return;
	qDebug() << "reply url " << reply->url();
	qDebug() << "reply from host " << reply->url().host();
	if(host == "circular-mp.sourceforge.net" ){
		QUrl redirectUrl;
		//		QString url
		if(possibleRedirectUrl.isValid()){
			qDebug() << "redirection " << possibleRedirectUrl;

			redirectUrl = possibleRedirectUrl.toUrl();

			if(redirectUrl.isRelative()){
				qDebug() << "relative url: " << redirectUrl;
			}
		}
		else if(!response.isEmpty()){
			redirectUrl = QUrl(QString(response));
			qDebug() << "got echoed url " << redirectUrl;
		}
		if(!redirectUrl.isEmpty()) {
			QNetworkRequest request;
			request.setUrl(redirectUrl);

			MyNetworkReply *myReply = new MyNetworkReply;
			myReply->setSender(sender);

			myReply->setReply( m_network.get(request) );
			connect(myReply, SIGNAL(myReplyFinished(Node*,QNetworkReply*)), instance(), SLOT(on_replyFinished(Node*,QNetworkReply*)));
		}
		return;
	}



	// #######################
	// #
	// # actual response from vkontakte
	// #
	// #######################


	QString artist = "";
	QString title = "";
	QString url = "";
	bool flagSource = false;

	int count = 0;

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "artist"){
				artist = xml.readElementText();
				artist.replace("&amp;", "&");
			}
			if(xml.name().toString() == "title"){
				title = xml.readElementText();
				title.replace("&amp;", "&");
			}

			if(xml.name().toString() == "url"){
				int match = Tools::artistTitleMatch(sender->artist(), sender->title(), artist, title);
				if(match == 0)
					continue;

				count++;
				if(count==1)
					sender->addSource(":::VKONTAKTE:::","");
				if(count>10){
					//ENDING AFTER 10 SOURCES!!!
					if(flagSource==false && !sender->hasSource())
						sender->setSource(sender->sourceListAt(1));
					//at 1, cause at 0 is the pseudo source seperator :::VKONTAKTE:::
					return;
				}
				url = xml.readElementText();
				sender->addSource(artist + " - " + title, url);

				if(!flagSource && match==3 && sender->source()==""){
					// if exact math artist - title found, set it as source
					flagSource = true;
					sender->setSource(url);
				}
			}
		}
	}

	//if less than 10 sources found
	if(!sender->hasSource())
		sender->setSource(sender->sourceListAt(1));
	//at 1, cause at 0 is the pseudo source seperator :::VKONTAKTE:::

	//if no source found and node has source as 'null'
	if(sender->source()=="null")
		sender->setSource("");
}

void Vkontakte::on_replyFinished()
{
	qDebug() << "VK ON REPLY FINISHED";
	QByteArray response = p_searchReply->readAll();



	p_searchReply->deleteLater();
	QList< QList<QString> > resultsList;

	QXmlStreamReader xml(response);
	QString artist;
	QString title;
	QString url;
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "artist"){
				artist = xml.readElementText();
				artist.replace("&amp;", "&");
			}
			if(xml.name().toString() == "title"){
				title = xml.readElementText();
				title.replace("&amp;", "&");
			}
			if(xml.name().toString() == "url"){
				url = xml.readElementText();
				QList<QString> entry;
				entry.append(artist);
				entry.append(title);
				entry.append(url);
				resultsList.append(entry);
			}
		}
	}
	emit gotResults(resultsList);
}

void Vkontakte::doSearch(Node *sender)
{
	if(!ObjectManager::objectDynamicExists(sender))
		return;

	restartTimer();

	QString artist = sender->artist();
	QString title = sender->title();
	QUrl urlRequest;

	QByteArray byteRequest = "http://circular-mp.sourceforge.net/vkontakteurl.php?query=";

	byteRequest += QUrl::toPercentEncoding(artist + " " +title);
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
//	request.setRawHeader("HOST", "circular-mp.sourceforge.net");
	QString userAgent = "CircularMediaPlayer/" + Settings::instance()->version + " (Nokia;Qt)";
	request.setRawHeader("User-Agent", userAgent.toLocal8Bit());
//	request.setRawHeader("Accept", "text/html, */*; q=0.01");
//	request.setRawHeader("Accept-Language", "en-US,en;q=0.5");
//	request.setRawHeader("Accept-Encoding", "gzip, deflate");

	request.setUrl(urlRequest);
//	request.setRawHeader(QByteArray('Accept-Encoding'), QByteArray('gzip, deflate'));
//	request.setRawHeader(QByteArray('Origin'), QByteArray('http://localhost'));

//	QSslConfiguration config = QSslConfiguration::defaultConfiguration();
//	config.setProtocol(QSsl::TlsV1_2);
//	request.setSslConfiguration(config);

	qDebug() <<"request raw header: " <<  request.rawHeaderList();

	const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
	  foreach (QByteArray rawHeader, rawHeaderList) {
		qDebug() << request.rawHeader(rawHeader);
	  }
//	  qDebug() << data;

	MyNetworkReply *myReply = new MyNetworkReply;
	myReply->setSender(sender);

	myReply->setReply( m_network.get(request) );
	connect(myReply, SIGNAL(myReplyFinished(Node*,QNetworkReply*)), instance(), SLOT(on_replyFinished(Node*,QNetworkReply*)));
}

void Vkontakte::restartTimer()
{
	if(!m_timer.isActive())
		m_timer.start(500);
}

void Vkontakte::on_timerSearch()
{
	//	qDebug() << "vkontakte timer search!";
	if(m_requestList.count() == 0)
		return;
	else
		doSearch(m_requestList.takeFirst());
}
