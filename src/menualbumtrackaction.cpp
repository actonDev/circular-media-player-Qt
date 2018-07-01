#include "menualbumtrackaction.h"
#include <QDebug>
#include "graph.h"
#include "node.h"
MenuAlbumTrackAction::MenuAlbumTrackAction(QString artist, QString title, int number, QObject *parent) :
	QAction(parent)
{
	qDebug() << "Album track: " << number << artist << " - " << title;
	m_artist = artist;
	m_title = title;
	if(number==0)
		setText(artist + " - " + title);
	else
		setText(QString::number(number) + ". " + artist + " - " + title);

	connect(this, SIGNAL(triggered()), this, SLOT(onTrigger()));
}

MenuAlbumTrackAction::~MenuAlbumTrackAction()
{
	qDebug() << "MenuAlbumTrackAction DESTRUCTOR";
}

void MenuAlbumTrackAction::onTrigger()
{
	Node *node = new Node(m_artist, m_title);
	node->searchForSource();
	Graph::clickedNode()->addRelated(node, true);
//	Graph::clickedNode()->addRelated(m_artist, m_title, true)->searchForSource();
}
