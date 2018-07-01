#include "menualbummenu.h"
#include <QDebug>
#include "menualbumtrackaction.h"
#include "graph.h"
#include "node.h"

MenuAlbumMenu::MenuAlbumMenu(QString artist, QString album, QWidget *parent) :
	MenuActionLoading(parent)
{
	qDebug() << "menu album constructor";
	m_artist = artist;
	m_album = album;
	setTitle(artist + " - " + album);
	if(m_album == "")
		setTitle("No album found!");
	connect(this, SIGNAL(aboutToShow()), this, SLOT(showAlbumTracks()));
	connect(&m_lastFmSearch, SIGNAL(gotAlbumTracks(QList<QPair<QString,QString> >)), this, SLOT(gotTrackList(QList<QPair<QString,QString> >)));
}

MenuAlbumMenu::~MenuAlbumMenu()
{
	qDebug() << "menu album destructor";
}

QString MenuAlbumMenu::artist()
{
	return m_artist;
}

QString MenuAlbumMenu::album()
{
	return m_album;
}

void MenuAlbumMenu::showAlbumTracks()
{
	setLoading(true);
	if(m_album == "")
		return;
	qDebug() << "search for album: " << m_artist << " - " << m_album;
	m_lastFmSearch.searchAlbumTracks(m_artist, m_album);
}

void MenuAlbumMenu::gotTrackList(QList<QPair<QString, QString> > trackList)
{
	setLoading(false);
	m_trackList = trackList;
	qDebug() << "############## Got track list for: " << m_artist << " - " << m_album;
	QAction *actionAll = new QAction("Enqueue all", this);
	connect(actionAll, SIGNAL(triggered()), this, SLOT(enqueueAll()));
	addAction(actionAll);
	addSeparator();
	for(int i=0; i<trackList.count(); i++){
		QString artist = trackList.at(i).first;
		QString title = trackList.at(i).second;
		qDebug() << i << ") " << artist << " - " << title;
		MenuAlbumTrackAction *track = new MenuAlbumTrackAction(artist, title, i+1, this);
		addAction(track);
	}
}

void MenuAlbumMenu::enqueueAll()
{
	qDebug() << "enqueueAll";
	Node *toAdd = Graph::clickedNode();
	for(int i=0; i<m_trackList.count(); i++){
		QString artist = m_trackList.at(i).first;
		QString title = m_trackList.at(i).second;
//		Node *temp = addRelated(artist,title,true);
		Node *temp = new Node(artist, title);
		temp->searchForSource();
		toAdd->addRelated(temp,true);
		toAdd = temp;
	}
}
