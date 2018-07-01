#ifndef MENUALBUMMENU_H
#define MENUALBUMMENU_H

#include <QMenu>
#include "lastfmsearch.h"
#include "menuactionloading.h"

class MenuAlbumMenu : public MenuActionLoading
{
	Q_OBJECT
private:
	QString m_artist;
	QString m_album;
	LastFmSearch m_lastFmSearch;
	QList<QPair<QString, QString> > m_trackList;
public:
	explicit MenuAlbumMenu(QString artist, QString album, QWidget *parent = 0);
	~MenuAlbumMenu();
	QString artist();
	QString album();
signals:

private slots:
	void showAlbumTracks();
	void gotTrackList(QList< QPair<QString, QString> > trackList);
	void enqueueAll();
public slots:
	
};

#endif // MENUALBUMMENU_H
