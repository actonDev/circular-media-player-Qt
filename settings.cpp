#include "settings.h"
#include <QCryptographicHash>
#include "tools.h"
#include <QXmlStreamReader>
#include <QStringList>
#include <QFile>
#include <QSettings>
#include <QApplication>
#include "graph.h"
#include "objectmanager.h"
#include "lastfm.h"
#include "myview.h"

//###################### END SINGLETON #########################

namespace{
const QString LAST_FM_API_KEY = "your last fm api key";
const QString LAST_FM_API_SECRET = "your last fm api secret";
}
Settings::Settings()
{
	connect(&m_timerSavePlaylist, SIGNAL(timeout()), this, SLOT(savePlaylist()));
	readOptions();
	LastFm::instance();
	LastFm::setSession(LASTFM_USERNAME, SESSION_KEY);
	readThemeOptions();
	PLAYING_SHUFFLE_MODE = false;
	PARTY_MODE = false;
	connect(&m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	version = "0.9.7.6";
//	if(version < QString("0.9.7"))
//		qDebug() << "CHECK VERSION TRUE";
	updateTimerSavePlaylist();
}
Settings::~Settings(){
	qDebug() << "settings desctructor";
	writeOptions();
	savePlaylist();
}

QString Settings::themeFolder()
{
	QString folder = Tools::resourcesLocation() + "/Themes/" +THEME;
	return folder;
}

void Settings::updateThemeFolder()
{
	QString style = Tools::styleSheetFromFile(themeFolder()+"/style.css", themeFolder());
	qApp->setStyleSheet(style);
	Graph::view()->setBackgroundImage(themeFolder() + "/back.png");
	readThemeOptions();
}
void Settings::writeOptions(){
	//clear file
	QFile file(Tools::resourcesLocation() + "/options.ini");
	file.open(QFile::WriteOnly|QFile::Truncate);
	file.close();

	QSettings settings(Tools::resourcesLocation() + "/options.ini", QSettings::IniFormat, this);
	settings.beginGroup("GENERAL");
	if(SUBMIT_INFO == false)
		settings.setValue("SUBMIT_INFO",false);
	settings.setValue("GEOMETRY",GEOMETRY);
	settings.setValue("CHECK_UPDATES",CHECK_UPDATES);
	settings.setValue("DONT_SHOW_HELP",DONT_SHOW_HELP);
	settings.setValue("SET_PLAYING_AS_ROOT",SET_PLAYING_AS_ROOT);
	settings.setValue("INSTANT_SEARCH",INSTANT_SEARCH);
	settings.setValue("ZOOM_ON_HOVER",ZOOM_ON_HOVER);
	settings.setValue("ZOOM_FACTOR",ZOOM_FACTOR);
	settings.setValue("TOOLTIP_ON_HOVER",TOOLTIP_ON_HOVER);
	settings.setValue("TOOLTIP_WHEN_CLIPPED",TOOLTIP_WHEN_CLIPPED);
	settings.setValue("TOOLTIP_VIEW_ZOOM_LESS_THAN",TOOLTIP_VIEW_ZOOM_LESS_THAN);
	settings.setValue("NOW_PLAYING_NOTIFICATION",NOW_PLAYING_NOTIFICATION);
	settings.setValue("MAX_STRING_LENGTH",MAX_STRING_LENGTH);
	settings.setValue("MAX_STRING_LENGTH_HOVER",MAX_STRING_LENGTH_HOVER);
	settings.setValue("AUTO_SAVE_PLAYLIST_MINS",AUTO_SAVE_PLAYLIST_MINS);
	settings.endGroup();

	settings.beginGroup("RELATED");
	settings.setValue("RELATED_TO_ADD",RELATED_TO_ADD);
	settings.setValue("DONT_ADD_SAME_ARTIST",DONT_ADD_SAME_ARTIST);
	settings.setValue("DONT_ADD_HISTORY_LAST_N_HOURS",DONT_ADD_HISTORY_LAST_N_HOURS);
	settings.setValue("GET_RELATED_IF_LESS_THAN",GET_RELATED_IF_LESS_THAN);
	settings.endGroup();

	settings.beginGroup("LAST_FM");
	settings.setValue("LASTFM_USERNAME",LASTFM_USERNAME);
	settings.setValue("SESSION_KEY",SESSION_KEY);
	settings.setValue("SCROBBLE", SCROBBLE);
	settings.setValue("NOW_PLAYING", NOW_PLAYING);
	settings.setValue("SKIP_UNKNOWN_ARTIST", SKIP_UNKNOWN_ARTIST);
	settings.endGroup();

	settings.beginGroup("APPEARANCE");
	settings.setValue("THEME",THEME);

	settings.endGroup();

	qDebug() << "written settings";

}
void Settings::readOptions(){
	QSettings settings(Tools::resourcesLocation() + "/options.ini", QSettings::IniFormat, this);

	settings.beginGroup("GENERAL");
	GEOMETRY = settings.value("GEOMETRY",QRectF()).toRect();
	CHECK_UPDATES = settings.value("CHECK_UPDATES",true).toBool();
	DONT_SHOW_HELP = settings.value("DONT_SHOW_HELP",false).toBool();
	SUBMIT_INFO = settings.value("SUBMIT_INFO",true).toBool();
	SET_PLAYING_AS_ROOT =  settings.value("SET_PLAYING_AS_ROOT",false).toBool();
	INSTANT_SEARCH = settings.value("INSTANT_SEARCH",true).toBool();
	REORDER_ON_MOUSE_RELEASE = settings.value("REORDER_ON_MOUSE_RELEASE",true).toBool();
	ZOOM_ON_HOVER = settings.value("ZOOM_ON_HOVER",true).toBool();
	ZOOM_FACTOR = settings.value("ZOOM_FACTOR",1.2).toDouble();
	TOOLTIP_ON_HOVER = settings.value("TOOLTIP_ON_HOVER",false).toBool();
	TOOLTIP_WHEN_CLIPPED = settings.value("TOOLTIP_WHEN_CLIPPED",true).toBool();
	TOOLTIP_VIEW_ZOOM_LESS_THAN = settings.value("TOOLTIP_VIEW_ZOOM_LESS_THAN",0.5).toReal();
	NOW_PLAYING_NOTIFICATION = settings.value("NOW_PLAYING_NOTIFICATION",true).toBool();
	MAX_STRING_LENGTH = settings.value("MAX_STRING_LENGTH",20).toInt();
	MAX_STRING_LENGTH_HOVER = settings.value("MAX_STRING_LENGTH_HOVER",40).toInt();
	AUTO_SAVE_PLAYLIST_MINS = settings.value("AUTO_SAVE_PLAYLIST_MINS",10).toInt();

	settings.endGroup();

	settings.beginGroup("RELATED");
	RELATED_TO_ADD = settings.value("RELATED_TO_ADD",5).toInt();
	DONT_ADD_SAME_ARTIST = settings.value("DONT_ADD_SAME_ARTIST",true).toBool();
	DONT_ADD_HISTORY_LAST_N_HOURS = settings.value("DONT_ADD_HISTORY_LAST_N_HOURS",24).toInt();
	GET_RELATED_IF_LESS_THAN = settings.value("GET_RELATED_IF_LESS_THAN",1).toInt();
	settings.endGroup();

	settings.beginGroup("LAST_FM");
	LASTFM_USERNAME = settings.value("LASTFM_USERNAME","").toString();
	SESSION_KEY = settings.value("SESSION_KEY", "").toString();
	SCROBBLE = settings.value("SCROBBLE", true).toBool();
	NOW_PLAYING = settings.value("NOW_PLAYING", true).toBool();
	SKIP_UNKNOWN_ARTIST = settings.value("SKIP_UNKNOWN_ARTIST", true).toBool();
	settings.endGroup();

	settings.beginGroup("APPEARANCE");

	THEME = settings.value("THEME","default").toString();
	settings.endGroup();

	qDebug() << "read settings";
}

void Settings::readThemeOptions()
{
	qDebug() << "read theme options: " << themeFolder();
	QSettings settings(themeFolder()+"/theme_options.ini", QSettings::IniFormat, this);
	EDGELINE_NORMAL_WIDTH = settings.value("EDGELINE_NORMAL_WIDTH",2).toInt();
	EDGELINE_NORMAL_COLOR = settings.value("EDGELINE_NORMAL_COLOR","black").toString();
	EDGELINE_ACTIVE_WIDTH = settings.value("EDGELINE_ACTIVE_WIDTH",4).toInt();
	EDGELINE_ACTIVE_COLOR = settings.value("EDGELINE_ACTIVE_COLOR","#00A504").toString();
	CIRCLE_TEXT_COLOR = settings.value("CIRCLE_TEXT_COLOR","white").toString();
	CIRCLE_HOVERED_TEXT_COLOR = settings.value("CIRCLE_HOVERED_TEXT_COLOR","white").toString();
	CIRCLE_HOVERED_TEXT_BACKGROUND_COLOR = settings.value("CIRCLE_HOVERED_TEXT_BACKGROUND_COLOR","black").toString();
	TEXT_BACKGROUND_OPACITY = settings.value("TEXT_BACKGROUND_OPACITY",0).toDouble();
}

void Settings::savePlaylist(bool defaultPlaylist)
{
	qDebug() << "### SAVE PLAYLIST ###";
	if(defaultPlaylist)
		emit infoSignal("Autosaved playlist!");
	QList<Node*> list = ObjectManager::nodeRootList();
	for(int i=0; i<list.count(); i++){
		Node *node = list.at(i);
		qDebug() << "Root: " << node->artist() << " - " << node->title();
	}


	qDebug() << "save slot";
	QDomDocument document;
	QDomElement root = document.createElement("CIRCULAR_MEDIA_PLAYLIST");
	document.appendChild(root);
	for(int i=0; i<list.count(); i++){
		Node *node = list.at(i);
		QDomElement domNode = document.createElement("NODE");
		domNode.setAttribute("ARTIST", node->artist());
		domNode.setAttribute("TITLE", node->title());
		domNode.setAttribute("SOURCE", node->source());
		domNode.setAttribute("PLAYING", node->playing() ? "TRUE":"FALSE");
		domNode.setAttribute("PLAY_NEXT", node->toPlayNext() ? "TRUE":"FALSE");
		domNode.setAttribute("POS_X", node->pos().x());
		domNode.setAttribute("POS_Y", node->pos().y());

		saveChildNode(&document, &domNode, node);
		root.appendChild(domNode);
	}

	QFile file;
	if(defaultPlaylist)
		file.setFileName(Tools::resourcesLocation() + "/playlist.cmpl");
	else{
		QString fileName = QFileDialog::getSaveFileName(0, tr("Save Playlist"), Tools::resourcesLocation()+"/Playlists/new playlist",tr("Circular Media Playlist (*.cmpl)"));
		if(fileName == "")
			return;
		file.setFileName(fileName);
	}

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		qDebug() << "failed to write file!";
	}
	else{
		QTextStream stream(&file);
		stream << document.toString();
		file.close();
		qDebug() << "written file!";
	}
}

void Settings::saveChildNode(QDomDocument *document, QDomElement *domParent, Node *nodeParent)
{
	for(int j=0; j<nodeParent->getChildrenList()->count(); j++){
		Node *node = nodeParent->getChildrenList()->at(j);
		QDomElement domNode = document->createElement("NODE");
		domNode.setAttribute("ARTIST", node->artist());
		domNode.setAttribute("TITLE", node->title());
		domNode.setAttribute("SOURCE", node->source());
		domNode.setAttribute("PLAYING", node->playing() ? "TRUE":"FALSE");
		domNode.setAttribute("PLAY_NEXT", node->toPlayNext() ? "TRUE":"FALSE");
		domNode.setAttribute("POS_X", node->pos().x());
		domNode.setAttribute("POS_Y", node->pos().y());
		saveChildNode(document, &domNode, node);
		domParent->appendChild(domNode);
	}
}

void Settings::loadChildNode(QDomElement *domParent, Node *nodeParent)
{
	QDomNodeList nodeList = domParent->childNodes();
	for(int i=0; i<nodeList.count(); i++){
		QDomElement domNode = nodeList.at(i).toElement();
		QString artist = domNode.attribute("ARTIST");
		QString title = domNode.attribute("TITLE");
		QString source = domNode.attribute("SOURCE");
		QString playing = domNode.attribute("PLAYING");
		QString playNext = domNode.attribute("PLAY_NEXT");
		qreal posX = (qreal)domNode.attribute("POS_X").toDouble();
		qreal posY = (qreal)domNode.attribute("POS_Y").toDouble();
		Node *node = new Node(artist, title, source);
		node->setPos(posX, posY);
		nodeParent->addRelated(node, false, false);
		if(playing == "TRUE")
			node->paintAsPlayingNode();
		else if(playNext == "TRUE")
			node->paintAsNext();
		loadChildNode(&domNode, node);
	}
}

void Settings::updateTimerSavePlaylist()
{
	if(AUTO_SAVE_PLAYLIST_MINS>0){
		m_timerSavePlaylist.setInterval(AUTO_SAVE_PLAYLIST_MINS * 1000 * 60);
		m_timerSavePlaylist.setSingleShot(false);
		m_timerSavePlaylist.start();
	}
	else
		m_timerSavePlaylist.stop();
}

void Settings::loadPlaylist(bool defaultPlaylist, bool clear)
{
	QString fileName;
	if(defaultPlaylist)
		fileName = Tools::resourcesLocation() + "/playlist.cmpl";
	else{
		fileName = QFileDialog::getOpenFileName(0, "Open Playlist",	Tools::resourcesLocation()+"/Playlists/", "Circular Media Playlist (*.cmpl)");

	}
	if(fileName == "")
		return;
	loadPlaylist(fileName, clear);
}

void Settings::loadPlaylist(QString fileName, bool clear)
{
	QDomDocument document;
	QFile file;
	file.setFileName(fileName);

	if(clear)
		Node::clearAll();

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		qDebug() << "Failed to open file!";
	}
	else{
		if(!document.setContent(&file)){
			qDebug() << "Failed to laod document!";
			return;
		}
		file.close();
	}

	QDomElement root = document.firstChildElement();
	Node *node = 0;
	QDomNodeList nodeList = root.childNodes();
	for(int i=0; i<nodeList.count(); i++){
		QDomElement domNode = nodeList.at(i).toElement();
		QString artist = domNode.attribute("ARTIST");
		QString title = domNode.attribute("TITLE");
		QString source = domNode.attribute("SOURCE");
		QString playing = domNode.attribute("PLAYING");
		QString playNext = domNode.attribute("PLAY_NEXT");
		qreal posX = (qreal)domNode.attribute("POS_X").toDouble();
		qreal posY = (qreal)domNode.attribute("POS_Y").toDouble();
		node = new Node(artist, title, source);
		node->setPos(posX, posY);
		if(playing == "TRUE")
			node->paintAsPlayingNode();
		else if(playNext == "TRUE")
			node->paintAsNext();
		loadChildNode(&domNode, node);
	}
	qDebug() << "playing node: " << Node::playingNode();
	if(Node::playingNode()!=0)
		Node::playingNode()->centerOnThis();
	else if(node!=0)
		node->centerOnThis();
}

void Settings::setAutoSavePlaylistMins(int mins)
{
	AUTO_SAVE_PLAYLIST_MINS = mins;
	updateTimerSavePlaylist();
}

void Settings::lastFmAuthorize(QString user, QString pass)
{
	LASTFM_USERNAME = user;
	emit lastFmAuthorizationInfo("Authorizing");
	QString passMd5 = QString(QCryptographicHash::hash(pass.toLocal8Bit(),QCryptographicHash::Md5).toHex());
	QString userPassMd5 = user + passMd5;
	QString authToken = QString(QCryptographicHash::hash(userPassMd5.toLocal8Bit(),QCryptographicHash::Md5).toHex());

	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = LAST_FM_API_KEY;
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

void Settings::lastFmNowPlaying(QString artist, QString title, int duration)
{
	if(SESSION_KEY == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = LAST_FM_API_KEY;
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
	pair.second = SESSION_KEY;
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

void Settings::lastFmScrobble(QString artist, QString title, int secSince1970)
{
	if(SESSION_KEY == "")
		return;
	QList< QPair<QString,QString> > arguments;
	QPair<QString, QString> pair;
	pair.first = "api_key";
	pair.second = LAST_FM_API_KEY;
	arguments.append(pair);
	pair.first = "artist";
	pair.second = artist;
	arguments.append(pair);
	pair.first = "method";
	pair.second = "track.scrobble";
	arguments.append(pair);
	pair.first = "sk";
	pair.second = SESSION_KEY;
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

void Settings::setThemeFolder(QString folder)
{
	QString style = Tools::styleSheetFromFile(folder + "/style.css", folder);
	//	QApplication::setStyleSheet(StyleSheet);
	qApp->setStyleSheet(style);
	Graph::view()->setBackgroundImage(folder + "/back.png");
	readThemeOptions();
}

void Settings::replyFinished(QNetworkReply *reply)
{
//	qDebug() << "REPLY FROM: " << reply->url();
	QByteArray response(reply->readAll());

	QString textReply(response);
	textReply.resize(500);
//	qDebug() << textReply;


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
			SESSION_KEY = key;
//			qDebug() << "READ KEY: " << key;
		}
	}

	//### AUTHORIZATION REPLY
	if(method == "auth.getMobileSession"){
		if(status == "failed"){
			SESSION_KEY = "";
		}
//		qDebug() << "STATUS " << status;
		emit lastFmAuthorizationInfo(status);
	}
	//### SCROBLE REPLY
	if(method == "track.scrobble"){
		if(status == "ok")
			emit infoSignal("Scrobbled!");
		else
			emit infoSignal("Scrobble failed!");
	}
}
