#include "menualbumaction.h"
#include "internetsearch.h"

MenuAlbumAction::MenuAlbumAction(QString artist, QString album, QObject *parent) :
	QAction(parent)
{
	m_artist = artist;
	m_album = album;
	setText(artist + " - " + album);
	connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));
}

void MenuAlbumAction::onTriggered()
{
	InternetSearch::instance()->setSearchAlbum(m_artist, m_album);
}
