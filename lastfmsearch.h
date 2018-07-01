#ifndef LASTFMSEARCH_H
#define LASTFMSEARCH_H

#include <QObject>
#include <QtNetwork>
#include <QDebug>
#include <QGraphicsScene>
#include <QMenu>
#include "menuactionloading.h"
class Node;

class LastFmSearch : public QObject
{
	Q_OBJECT
private:
	QNetworkAccessManager m_network;
	void replySearchTrack(QByteArray response);
	void replyTagTopTracks(QByteArray response);
	void replyTagSearch(QByteArray response);
	void replyTrackInfo(QByteArray response);
	void replyAlbumInfo(QByteArray response);
	void replySimilar(QByteArray response);
	void replyTagTopAlbums(QByteArray response);
	void replyArtistTopAlbums(QByteArray response);
	void replyGetRecentTracks(QByteArray response);
	void replyUserPlaylists(QByteArray response);
	void replyPlaylistFetch(QByteArray response);

	void replyUserTopTags(QByteArray response);
	Node *p_albumNode;
	MenuActionLoading *p_albumMenu;
	MenuActionLoading *p_tagsMenu;
public:
	explicit LastFmSearch(QObject *parent = 0);
	void searchForTrack(QString term);
	void searchTagTopTracks(QString term);
	void searchForTag(QString term);
	void searchForTagTopAlbums(QString term);
	void searchForArtistTopAlbums(QString term);
	void searchForArtist(QString term);
	void searchForAlbums(QString artist, QString title);
	void searchForSimilar(QString artist, QString track);
	void searchInfo(Node *sender, MenuActionLoading *menuAlbums, MenuActionLoading *menuTags);
	void searchAlbumTracks(QString artist, QString album);

	//lastfm view
	void searchUserLovedTracks(QString term);
	void searchUsertTopTags(QString term);
	void searchUserTaggedTracks(QString user, QString tag);
	void getweeklytrackchart(QString user);
	void getRecentTracks(QString user);
	void userGetPlaylists(QString user);
	void playlistFetch(QString playlistId);
signals:
	void gotResults(QList< QStringList >);
	void gotAlbumTracks(QList< QPair<QString, QString> >);
	void gotUserTopTags(QList< QPair<QString, QString> >);
	void gotUserPlaylists(QList<QStringList>);
public slots:
	void on_replyFinished(QNetworkReply *reply);
};

#endif // LASTFMSEARCH_H
