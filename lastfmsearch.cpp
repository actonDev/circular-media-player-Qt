#include "lastfmsearch.h"
#include "node.h"
#include "objectmanager.h"
#include "menualbummenu.h"
#include "menutagaction.h"
#include "menualbumaction.h"

namespace{
const QString API_KEY = "d84665f06309064914d4ae07488ea6e2";
}

LastFmSearch::LastFmSearch(QObject *parent) :
	QObject(parent)
{
	connect(&m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(on_replyFinished(QNetworkReply*)));
}

void LastFmSearch::searchForTrack(QString term)
{
	qDebug() << "search for track " << term;
	//sample
	//http://ws.audioscrobbler.com/2.0/?method=track.search&track=stigma&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=track.search&track=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&limit=100";
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::searchTagTopTracks(QString term)
{
	//sample
	//http://ws.audioscrobbler.com/2.0/?method=tag.gettoptracks&tag=disco&api_key=b25b959554ed76058ac220b7b2e0a026
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=tag.gettoptracks&tag=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
	qDebug() << request.url();
}

void LastFmSearch::searchForTag(QString term)
{
	// http://ws.audioscrobbler.com/2.0/?method=tag.search&tag=disco&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=tag.search&tag=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
	qDebug() << request.url();
}

void LastFmSearch::searchForTagTopAlbums(QString term)
{
	//sample:
	//http://ws.audioscrobbler.com/2.0/?method=tag.getTopAlbums&tag=neurofunk&api_key=b25b959554ed76058ac220b7b2e0a026
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=tag.getTopAlbums&tag=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
	qDebug() << request.url();
}

void LastFmSearch::searchForArtistTopAlbums(QString term)
{
	//sample:
	//http://ws.audioscrobbler.com/2.0/?method=artist.gettopalbums&artist=noisia&api_key=b25b959554ed76058ac220b7b2e0a026
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=artist.getTopAlbums&artist=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
	qDebug() << request.url();
}

void LastFmSearch::searchForArtist(QString term)
{

	// sample: http://ws.audioscrobbler.com/2.0/?method=artist.gettoptracks&artist=cher&api_key=b25b959554ed76058ac220b7b2e0a026
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=artist.gettoptracks&artist=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
	qDebug() << request.url();
}

void LastFmSearch::searchForAlbums(QString artist, QString title)
{
	//	p_albumNode = sender;
	//	p_albumMenu = menuAlbums;
	//	p_tagsMenu = menuTags;
	//	QString artist = sender->artist();
	//	QString title = sender->title();
	//	QUrl urlRequest;
	//	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=track.getInfo&artist=";
	//	byteRequest += QUrl::toPercentEncoding(artist);
	//	byteRequest += "&track=";
	//	byteRequest += QUrl::toPercentEncoding(title);
	//	byteRequest += "&api_key=d84665f06309064914d4ae07488ea6e2";
	//	urlRequest.setEncodedUrl(byteRequest);

	//	QNetworkRequest request;
	//	request.setUrl(urlRequest);
	//	m_network.get(request);

}

void LastFmSearch::searchForSimilar(QString artist, QString track)
{
	qDebug() << "search for similar!!";
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=track.getsimilar&artist=";
	byteRequest += QUrl::toPercentEncoding(artist);
	byteRequest += "&track=";
	byteRequest += QUrl::toPercentEncoding(track);
	byteRequest += "&api_key=" + API_KEY;
	byteRequest += "&autocorrect=1";
	QUrl urlRequest;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::searchInfo(Node *sender, MenuActionLoading *menuAlbums, MenuActionLoading *menuTags)
{
	p_albumNode = sender;
	p_albumMenu = menuAlbums;
	p_tagsMenu = menuTags;
	QString artist = sender->artist();
	QString title = sender->title();
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=track.getInfo&artist=";
	byteRequest += QUrl::toPercentEncoding(artist);
	byteRequest += "&track=";
	byteRequest += QUrl::toPercentEncoding(title);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::searchAlbumTracks(QString artist, QString album)
{
	// sample http://ws.audioscrobbler.com/2.0/?method=album.getinfo&artist=noisia&album=split%20the%20atom&api_key=d84665f06309064914d4ae07488ea6e2

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=album.getinfo"; //method
	byteRequest += "&artist=";
	byteRequest += QUrl::toPercentEncoding(artist);
	byteRequest += "&album=";
	byteRequest += QUrl::toPercentEncoding(album);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);

}

void LastFmSearch::searchUserLovedTracks(QString term)
{
	// sample http://ws.audioscrobbler.com/2.0/?method=user.getlovedtracks&user=act1v1st&api_key=d84665f06309064914d4ae07488ea6e2

	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.getlovedtracks";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::searchUsertTopTags(QString term)
{
	// http://ws.audioscrobbler.com/2.0/?method=user.gettoptags&user=act1v1st&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.gettoptags";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(term);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::searchUserTaggedTracks(QString user, QString tag)
{
	// http://ws.audioscrobbler.com/2.0/?method=user.getpersonaltags&user=act1v1st&tag=neurofunk&taggingtype=track&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.getpersonaltags";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(user);
	byteRequest += "&tag=";
	byteRequest += QUrl::toPercentEncoding(tag);
	byteRequest += "&taggingtype=track";
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::getweeklytrackchart(QString user)
{
	// http://ws.audioscrobbler.com/2.0/?method=user.getweeklytrackchart&user=act1v1st&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.getweeklytrackchart";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(user);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::getRecentTracks(QString user)
{
	// http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=act1v1st&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(user);
	byteRequest += "&limit=100";
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::userGetPlaylists(QString user)
{
	qDebug() << "get playlists";
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=user.getplaylists";
	byteRequest += "&user=";
	byteRequest += QUrl::toPercentEncoding(user);
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::playlistFetch(QString playlistId)
{
	// http://ws.audioscrobbler.com/2.0/?method=playlist.fetch&playlistURL=lastfm://playlist/2026126&api_key=d84665f06309064914d4ae07488ea6e2
	QUrl urlRequest;
	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=playlist.fetch&playlistURL=lastfm://playlist/";
	byteRequest += playlistId.toLocal8Bit();
	byteRequest += "&api_key=" + API_KEY;
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void LastFmSearch::on_replyFinished(QNetworkReply *reply)
{
	qDebug() << "lastfmsearch reply";
	QByteArray response(reply->readAll());
	QUrlQuery query(reply->url());
	QString method = query.queryItemValue("method");
	qDebug() << "method: " <<method;
	reply->deleteLater();

	if(method == "track.search")
		replySearchTrack(response);
	else if(method == "tag.gettoptracks" || method == "artist.gettoptracks")
		replyTagTopTracks(response);
	else if(method == "tag.search")
		replyTagSearch(response);
	else if(method == "track.getInfo")
		replyTrackInfo(response);
	else if(method == "album.getinfo")
		replyAlbumInfo(response);
	else if(method == "track.getsimilar")
		replySimilar(response);
	else if(method == "tag.getTopAlbums")
		replyTagTopAlbums(response);
	else if(method == "artist.getTopAlbums")
		replyArtistTopAlbums(response);
	else if(method == "artist.getTopAlbums")
		replyArtistTopAlbums(response);
	else if(method == "user.getlovedtracks")
		replyTagTopTracks(response); //same response structure!
	else if(method == "user.gettoptags")
		replyUserTopTags(response);
	else if(method == "user.getpersonaltags")
		replyTagTopTracks(response); //same response structure!
	else if(method == "user.getweeklytrackchart")
		replySearchTrack(response); //same response structure!
	else if(method == "user.getrecenttracks")
		replyGetRecentTracks(response);
	else if(method == "user.getplaylists")
		replyUserPlaylists(response);
	else if(method == "playlist.fetch")
		replyPlaylistFetch(response);
	//		replySearchTrack(response); //same response structure!

	//comment: tag.gettoptracks and artist.gettoptracks have ideantical response structure
	//	so for the artist.gettoptracks response the same function made for tag response is used!
}

void LastFmSearch::replySearchTrack(QByteArray response)
{
	//sample: http://ws.audioscrobbler.com/2.0/?method=track.search&track=stigma&api_key=d84665f06309064914d4ae07488ea6e2
	qDebug() << "replySearchTrack";
	//	QString text(response);
	//	text.resize(100);
	//	qDebug() << text;
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	entry.append("Artist");
	entry.append("Title");
	resultsList.append(entry);
	entry.clear();

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "artist")
				artist = xml.readElementText();
			if(xml.name().toString() == "name")
				title = xml.readElementText();

			if( artist!="" && title!=""){
				//got both
				entry.append(artist);
				entry.append(title);
				resultsList.append(entry);
				entry.clear();
				artist = "";
				title = "";
			}

		}
		//		if(xml.isEndElement()){
		//			qDebug() << "end: " <<  xml.name();
		//		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyTagTopTracks(QByteArray response)
{
	//sample: http://ws.audioscrobbler.com/2.0/?method=tag.gettoptracks&tag=jazz&api_key=d84665f06309064914d4ae07488ea6e2

	//	QString replyText(response);
	//	replyText.resize(100);
	//	qDebug() << replyText;
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	entry.append("Artist");
	entry.append("Title");
	resultsList.append(entry);
	entry.clear();

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "track"){
				xml.readNextStartElement();
				title = xml.readElementText();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();

				// got both
				entry.append(artist);
				entry.append(title);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyTagSearch(QByteArray response)
{
	QList< QStringList > resultsList;
	QStringList entry;
	QString tag;
	QString count;
	entry.append("Tag");
	entry.append("Times used");
	resultsList.append(entry);
	entry.clear();

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "name"){
				tag = xml.readElementText();
			}
			if(xml.name().toString() == "count"){
				count = xml.readElementText();
			}
		}
		if(xml.isEndElement()){
			if(xml.name().toString() == "tag"){
				entry.append(tag);
				entry.append(count);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyTrackInfo(QByteArray response)
{
	if(p_albumMenu!=0) p_albumMenu->setLoading(false);
	if(p_tagsMenu!=0) p_tagsMenu->setLoading(false);

	// sample: http://ws.audioscrobbler.com/2.0/?method=track.getInfo&artist=noisia&track=stigma&api_key=d84665f06309064914d4ae07488ea6e2
	if(!ObjectManager::objectDynamicExists(p_albumNode)) return;

	int tagCount = 0;

	QString artist;
	QString album;
	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "album"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				xml.readNextStartElement();
				album = xml.readElementText();
				//				continue;
			}
			if(p_tagsMenu!=0 && xml.name().toString() == "tag"){
				tagCount++;
				xml.readNextStartElement();
				QString tag = xml.readElementText();
				MenuTagAction *tagAction = new MenuTagAction(tag, p_tagsMenu);
				p_tagsMenu->addAction(tagAction);
			}
		}
	}

	if(p_tagsMenu!=0 && tagCount == 0){
		QAction *noTag = new QAction("No tags found!", p_tagsMenu);
		p_tagsMenu->addAction(noTag);
	}

	if(p_albumMenu != 0){
		if(album == ""){
			QAction *action = new QAction("No album found!", p_albumMenu);
			p_albumMenu->addAction(action);
			return;
		}
		MenuAlbumAction *albumMenuAction = new MenuAlbumAction(artist, album, p_albumMenu);
		p_albumMenu->addAction(albumMenuAction);

		//		MenuAlbumMenu *albumMenu = new MenuAlbumMenu(artist, album, p_albumMenu);
		//		p_albumMenu->addMenu(albumMenu);
	}
	qDebug() << "reply track info";
}

void LastFmSearch::replyAlbumInfo(QByteArray response)
{
	qDebug() << "reply album info";
	QList< QPair<QString, QString> > trackList;

	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	entry.append("Artist");
	entry.append("Title");
	resultsList.append(entry);
	entry.clear();


	bool flag = false;
	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "tracks") flag = true;
			if(!flag) continue;
			if(xml.name().toString() == "track"){
				xml.readNextStartElement();
				title = xml.readElementText();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				QPair<QString, QString> pair;
				pair.first = artist;
				pair.second = title;
				trackList.append(pair);

				entry.append(artist);
				entry.append(title);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotResults(resultsList);
	emit gotAlbumTracks(trackList);
}

void LastFmSearch::replySimilar(QByteArray response)
{
	// http://ws.audioscrobbler.com/2.0/?method=track.getsimilar&artist=noisia&track=stigma&api_key=d84665f06309064914d4ae07488ea6e2
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	QString match;
	entry.append("Artist");
	entry.append("Title");
//	entry.append("Match");
	resultsList.append(entry);
	entry.clear();
	//!##

	int count = 0;

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){

			if(xml.name().toString() == "track"){
				xml.readNextStartElement();
				title = xml.readElementText();
			}
			if(xml.name().toString() == "match"){
				match = xml.readElementText();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				entry.append(artist);
				entry.append(title);
//				entry.append(match);
				resultsList.append(entry);
				entry.clear();
				count++;
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyTagTopAlbums(QByteArray response)
{
	// http://ws.audioscrobbler.com/2.0/?method=tag.getTopAlbums&tag=neurofunk&api_key=d84665f06309064914d4ae07488ea6e2
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString album;
	entry.append("Artist");
	entry.append("Album");
	resultsList.append(entry);
	entry.clear();


	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){

			if(xml.name().toString() == "album"){
				xml.readNextStartElement();
				album = xml.readElementText();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				entry.append(artist);
				entry.append(album);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyArtistTopAlbums(QByteArray response)
{
	// // http://ws.audioscrobbler.com/2.0/?method=artist.gettopalbums&artist=noisia&api_key=d84665f06309064914d4ae07488ea6e2
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString album;
	entry.append("Artist");
	entry.append("Album");
	resultsList.append(entry);
	entry.clear();


	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){

			if(xml.name().toString() == "album"){
				xml.readNextStartElement();
				album = xml.readElementText();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				entry.append(artist);
				entry.append(album);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyGetRecentTracks(QByteArray response)
{
	//sample: http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&user=act1v1st&page=0&api_key=d84665f06309064914d4ae07488ea6e2
	qDebug() << "replySearchTrack";
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	QString date;
	entry.append("Artist");
	entry.append("Title");
	entry.append("Date");
	resultsList.append(entry);
	entry.clear();

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "artist")
				artist = xml.readElementText();
			if(xml.name().toString() == "name")
				title = xml.readElementText();
			if(xml.name().toString() == "date"){
				date = xml.readElementText();
				entry.append(artist);
				entry.append(title);
				entry.append(date);
				resultsList.append(entry);
				entry.clear();
				artist = "";
				title = "";
				date = "";
			}
		}
	}
	emit gotResults(resultsList);
}

void LastFmSearch::replyUserPlaylists(QByteArray response)
{
	qDebug() << "got playlists";
	// http://ws.audioscrobbler.com/2.0/?method=user.getplaylists&user=rj&api_key=d84665f06309064914d4ae07488ea6e2
	QList<QStringList> resultsList;
	QStringList entry;
	QString name;
	QString id;
	QString count;

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "id"){
				id = xml.readElementText();
			}
			if(xml.name().toString() == "title"){
				name = xml.readElementText();
			}
			if(xml.name().toString() == "size"){
				count = xml.readElementText();
				entry.append(id);
				entry.append(name);
				entry.append(count);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	emit gotUserPlaylists(resultsList);
}

void LastFmSearch::replyPlaylistFetch(QByteArray response)
{
	// http://ws.audioscrobbler.com/2.0/?method=playlist.fetch&playlistURL=lastfm://playlist/2614993&api_key=d84665f06309064914d4ae07488ea6e2
	QList< QStringList > resultsList;
	QStringList entry;
	QString artist;
	QString title;
	entry.append("Artist");
	entry.append("Title");
	resultsList.append(entry);
	entry.clear();


	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){
			if(xml.name().toString() == "title"){
				title = xml.readElementText();
			}
			if(xml.name().toString() == "creator"){
				artist = xml.readElementText();
			}
		}
		if(xml.isEndElement()){
			if(xml.name().toString() == "track"){
				entry.append(artist);
				entry.append(title);
				resultsList.append(entry);
				entry.clear();
			}
		}
	}
	qDebug() << "GOT PLAYLIST TRACKS";
	emit gotResults(resultsList);
}

void LastFmSearch::replyUserTopTags(QByteArray response)
{
	QList< QPair<QString, QString> > resultsList;
	QPair<QString, QString> entry;
	QString tag;
	QString count;
	//	entry.append("Artist");
	//	entry.append("Album");
	//	resultsList.append(entry);
	//	entry.clear();


	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
		xml.readNext();
		if(xml.isStartElement()){

			if(xml.name().toString() == "tag"){
				xml.readNextStartElement();
				tag = xml.readElementText();
				xml.readNextStartElement();
				count = xml.readElementText();
				entry.first = tag;
				entry.second = count;
				resultsList.append(entry);
			}
		}
	}
	emit gotUserTopTags(resultsList);
}
