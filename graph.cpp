#include "graph.h"
#include <QDesktopServices>
#include <typeinfo>

#include "lastfmrelated.h"
#include "lastfm.h"
#include "menusourceaction.h"
#include "internetsearch.h"
#include "node.h"
#include "vkontakte.h"
#include "myview.h"

#include "downloader.h"
#include "tools.h"

MyView *Graph::p_view = 0;
Node *Graph::p_clickedNode = 0;
QGraphicsScene *Graph::p_scene = 0;
QPoint Graph::m_point;

Graph *Graph::p_instance = 0;

Graph::Graph(QObject *parent, QGraphicsScene *scene, MyView *view) : QObject(parent)
{
	p_instance = this;
	p_scene = scene;
	p_view = view;
	p_lastFm = new LastFmRelated(this->parent(),this);

	m_menuSources.setTitle("Source");
	m_menuAlbums.setTitle("Album(s)");

	QAction *actionPlayTill = new QAction("Play till this", this); //not used

	//search...
	m_menuTags.setTitle("Tag top tracks");
	QAction *actionDownload = new QAction("Download", this);
	QAction *actionSearchArtist = new QAction("Artist top tracks", this);
	QAction *actionSearchArtistAlbums = new QAction("Artist top albums", this);
	QAction *actionSearchSimilar = new QAction("Similar tracks", this);
	QAction *actionSearchGoogle = new QAction("Search on Google", this);
	m_menuSearch.setTitle("Search...");
	m_menuSearch.addAction(actionSearchArtist);
	m_menuSearch.addAction(actionSearchArtistAlbums);
	m_menuSearch.addAction(actionSearchSimilar);
	m_menuSearch.addMenu(&m_menuAlbums);
	m_menuSearch.addMenu(&m_menuTags);

	QAction *actionRelated = new QAction("Get related", this);
	QAction *actionEdit = new QAction("Edit", this);
	QAction *actionSearch = new QAction("Search for source", this);
	QAction *actionSetAsRoot = new QAction("Set as root", this);

	QAction *actionDelete = new QAction("This", this);
	QAction *actionDeleteSiblings = new QAction("Siblings", this);
	QAction *actionDeleteChildren = new QAction("Children", this);
	QAction *actionDeleteTree = new QAction("Whole tree", this);
	QAction *actionDeleteOtherTrees = new QAction("All other trees but this", this);
	m_menuDelete.setTitle("Delete...");
	m_menuDelete.addAction(actionDelete);
	m_menuDelete.addAction(actionDeleteSiblings);
	m_menuDelete.addAction(actionDeleteChildren);
	m_menuDelete.addAction(actionDeleteTree);
	m_menuDelete.addAction(actionDeleteOtherTrees);

	QAction *actionLoveTrack = new QAction("Love track", this);
	QAction *actionTagTrack = new QAction("Tag track", this);
	m_menuLastFm.setTitle("Last.fm");
	m_menuLastFm.addAction(actionLoveTrack);
	m_menuLastFm.addAction(actionTagTrack);

	//filling the main menu
	m_menu.addMenu(&m_menuSources);
	m_menu.addAction(actionSearch);
#ifdef DOWNLOAD_ENABLED
	m_menu.addAction(actionDownload);
#endif
	m_menu.addSeparator();


	m_menu.addMenu(&m_menuSearch);
	m_menu.addAction(actionSearchGoogle);
	m_menu.addSeparator();

	m_menu.addMenu(&m_menuLastFm);

	m_menu.addAction(actionRelated);
	m_menu.addAction(actionEdit);
	m_menu.addAction(actionSetAsRoot);
	m_menu.addSeparator();

	m_menu.addMenu(&m_menuDelete);
	m_menu.addSeparator();

	connect(actionDownload, SIGNAL(triggered()), this, SLOT(menuDownload()));
	connect(&m_menuSources, SIGNAL(aboutToShow()), this, SLOT(menuShowSources()));
	connect(&m_menuAlbums, SIGNAL(aboutToShow()), this, SLOT(menuShowAlbums()));
	connect(&m_menuTags, SIGNAL(aboutToShow()), this, SLOT(menuShowTags()));
	connect(actionSearchArtist, SIGNAL(triggered()), this, SLOT(menuSearchArtist()));
	connect(actionSearchArtistAlbums, SIGNAL(triggered()), this, SLOT(menuSearchArtistAlbums()));
	connect(actionSearchSimilar, SIGNAL(triggered()), this, SLOT(menuSearchSimilar()));
	connect(actionSearchGoogle, SIGNAL(triggered()), this, SLOT(menuSearchGoogle()));
	connect(actionRelated, SIGNAL(triggered()), this, SLOT(menuGetRelated()));

	connect(actionEdit, SIGNAL(triggered()), this, SLOT(menuEdit()));
	connect(&m_editDialog, SIGNAL(accepted()), this, SLOT(menuEditAccepted()));
	connect(actionSearch, SIGNAL(triggered()), this, SLOT(menuSearch()));
	connect(actionSetAsRoot, SIGNAL(triggered()),this, SLOT(menuSetAsRoot()));
	connect(actionPlayTill, SIGNAL(triggered()), this, SLOT(menuPlayTill()));

	connect(actionLoveTrack, SIGNAL(triggered()), this, SLOT(menuLoveTrack()));
	connect(actionTagTrack, SIGNAL(triggered()), this, SLOT(menuTagTrack()));

	connect(actionDelete, SIGNAL(triggered()), this, SLOT(menuDelete()));
	connect(actionDeleteSiblings, SIGNAL(triggered()), this, SLOT(menuDeleteSiblings()));
	connect(actionDeleteChildren, SIGNAL(triggered()), this, SLOT(menuDeleteChildren()));
	connect(actionDeleteTree, SIGNAL(triggered()), this, SLOT(menuDeleteTree()));
	connect(actionDeleteOtherTrees, SIGNAL(triggered()), this, SLOT(menuDeleteOtherTrees()));
}

void Graph::searchForSource(Node *node, bool onlineToo)
{
	// bool onlineToo: search online even if local source is found?

	QList< QStringList > localList = p_lib->searchForSources(node->artist(), node->title());
	for(int i=0; i<localList.count(); i++){
		if(i==0){
			node->addSource(":::LOCAL:::", "");
		}
		QString artist = localList.at(i).at(0);
		QString title = localList.at(i).at(1);
		QString source = localList.at(i).at(2);
		node->addSource(artist + " - " + title, source);
		if(!node->hasSource()){
			int match = Tools::artistTitleMatch(node->artist(),node->title(), artist, title);
			if(match==3)
				node->setSource(source);
		}
		qDebug() << "FOUND LOCAL SOURCE: " << source;
		if(i==localList.count()-1 && !node->hasSource()){
			node->setSource(localList.at(i).at(2));
		}
	}
//	if(!node->hasSource())

	if(localList.isEmpty()){
		Vkontakte::search(node);
	}
	else if(onlineToo)
		Vkontakte::search(node);
}

QAction *Graph::showMenu(Node *sender)
{
	m_point = QCursor::pos();
	p_clickedNode = sender;

	if(LastFm::authorized())
		m_menuLastFm.setEnabled(true);
	else
		m_menuLastFm.setEnabled(false);

	return m_menu.exec(m_point);
}

void Graph::emitNodeDeletion(Node *deletedNode)
{
	if(p_clickedNode == deletedNode)
		p_clickedNode = 0;
	emit nodeDeleted(deletedNode);
	p_lastFm->onNodeDeleted(deletedNode);
}

void Graph::createEditNode(QPointF point)
{
	m_point = QCursor::pos();
	Node *node = new Node();
	if(point == QPointF())
		point = view()->mapToScene(view()->viewport()->rect().center());
	node->setPos(point);
	p_clickedNode = node;
	p_instance->menuEdit();
}

void Graph::createNode()
{
	qDebug() << "create node";
	new Node();
}

Node *Graph::createNode(QString artist, QString title, QString source, QPointF point)
{
	qDebug() << "create node source: " << source;
	Node *node = new Node(artist, title, source);
	if(source == "")
		node->searchForSource();
	node->setPos(point);
	return node;
}

void Graph::createAndPlayNode(QString artist, QString title, QString source)
{
	Node *playingNode = Node::playingNode();
	if(playingNode!=0)
		playingNode->setPlaying(false);
	Node *node = createNode(artist, title, source);
	node->setAsPlayingNode();
	QPointF point = view()->mapToScene(view()->viewport()->rect().center());
	node->setPos(point);
}

void Graph::menuDelete()
{
	p_clickedNode->deleteThis();
}

void Graph::menuDeleteSiblings()
{
	p_clickedNode->deleteSiblings();
}

void Graph::menuGetRelated()
{
	p_lastFm->getRelated(p_clickedNode);
}

void Graph::menuPlayTill()
{
	p_clickedNode->enqueueTillThis();
}

void Graph::menuEdit()
{
	m_editDialog.setArtist(p_clickedNode->artist());
	m_editDialog.setTitle(p_clickedNode->title());
	QRect rect(m_point, QSize(30,30));
	m_editDialog.setGeometry(rect);
	m_editDialog.exec();
}

void Graph::menuEditAccepted()
{
	p_clickedNode->setArtist(m_editDialog.artist());
	p_clickedNode->setTitle(m_editDialog.title());
	if(p_clickedNode->source()=="")
		p_clickedNode->searchForSource();
}

void Graph::menuDeleteChildren()
{
	p_clickedNode->deleteChildren(0);
}

void Graph::menuDeleteTree()
{
//	second way: p_clickedNode->getGlobalParent()->deleteThis();
	p_clickedNode->setAsRoot();
	p_clickedNode->deleteThis();
}

void Graph::menuDeleteOtherTrees()
{
	qDebug() << "Delete other trees!";
	QList<Node*> rootList = ObjectManager::nodeRootList();
	Node *rootExcl = clickedNode()->getGlobalParent();
	for(int i=0; i<rootList.count(); i++){
		Node *root = rootList.at(i);
		if(root == rootExcl)
			continue;
		root->deleteThis();
	}
}

void Graph::menuShowSources()
{
	m_menuSources.clear();
	for(int i=0; i<p_clickedNode->sourcesList().count(); i++){
		QString text = p_clickedNode->sourceTextAt(i);
		QString source = p_clickedNode->sourceListAt(i);
		MenuSourceAction *action = new MenuSourceAction(text, source, &m_menuSources);
		connect(action, SIGNAL(sourceSelected(QString)), p_clickedNode, SLOT(setSource(QString)));
		if(p_clickedNode->source() == action->source()){
			action->setCheckable(true);
			action->setChecked(true);
		}
		m_menuSources.addAction(action);
	}
	qDebug() << "Graph::menuShowSources() END";
}

void Graph::menuShowAlbums()
{
	m_menuAlbums.setLoading(true);
	//////////////////////
	// CLEAR ALBUM ENTRIES

	//	m_menuAlbums.clear();
	QAction *selfAction = m_menuAlbums.menuAction();
	for(int i = m_menuAlbums.children().count() -1; i>=0; i--){
		if(selfAction == m_menuAlbums.children().at(i)) continue;
		qDebug() << m_menuAlbums.children().at(i);
		delete m_menuAlbums.children().at(i);
	}

	////////////////////////
	// NOW SEARCH FOR ALBUMS

	qDebug() << "show albums for " << p_clickedNode->title();
	m_lastFmSearch.searchInfo(p_clickedNode, &m_menuAlbums, 0);
}

void Graph::menuShowTags()
{
	m_menuTags.setLoading(true);
	//////////////////////
	// CLEAR TAG ENTRIES
	QAction *selfAction = m_menuTags.menuAction();
	qDebug() << "menu tags children, self action ==  " << selfAction;
	for(int i = m_menuTags.children().count()-1; i>= 0; i--){
		qDebug() << m_menuTags.children().at(i);
		if(selfAction != m_menuTags.children().at(i)){
			qDebug() << "deleting tag";
			delete m_menuTags.children().at(i);
		}
	}

	////////////////////////
	// NOW SEARCH FOR TAGS

	qDebug() << "show albums for " << p_clickedNode->title();
	m_lastFmSearch.searchInfo(p_clickedNode, 0, &m_menuTags);
}

void Graph::menuSearchGoogle()
{
	QByteArray address = "https://www.google.com/search?q=";
	address += QUrl::toPercentEncoding(p_clickedNode->artist() +" - " + p_clickedNode->title());
	QUrl url = QUrl::fromEncoded(address);
	QDesktopServices::openUrl(url);
}

void Graph::menuLoveTrack()
{
	QString artist = p_clickedNode->artist();
	QString title = p_clickedNode->title();
	LastFm::loveTrack(artist, title);
}

void Graph::menuTagTrack()
{
	QString artist = p_clickedNode->artist();
	QString title = p_clickedNode->title();
	QRect rect(m_point, QSize(30,30));
	m_dialogAddTags.setGeometry(rect);
	if(m_dialogAddTags.exec()){
		QString tags = m_dialogAddTags.tags();
		LastFm::trackAddTags(artist, title, tags);
	}
}

void Graph::onMenuHidden()
{
	MenuSourceAction *action = dynamic_cast<MenuSourceAction*>(m_menuSources.activeAction());
	if(action==0)
		m_menuSources.clear();
}

void Graph::menuSearch()
{
	p_clickedNode->clearSources();
	searchForSource(p_clickedNode, true);
}

void Graph::menuSearchArtist()
{
	InternetSearch::instance()->setSearchArtist(p_clickedNode->artist());
}

void Graph::menuSearchArtistAlbums()
{
	InternetSearch::instance()->setSearchArtistAlbums(p_clickedNode->artist());
}

void Graph::menuSearchSimilar()
{
	InternetSearch::instance()->setSearchSimilar(p_clickedNode->artist(), p_clickedNode->title());
}

void Graph::menuSetAsRoot()
{
	p_clickedNode->setAsRoot();
}

void Graph::updatePlayingNode()
{
	emit playingNodeChanged();
}

void Graph::showEditDialog()
{
//	m_editDialog.setArtist(p_clickedNode->artist());
//	m_editDialog.setTitle(p_clickedNode->title());
//	QRect rect(QCursor::pos(), QSize(30,30));
//	m_editDialog.setGeometry(rect);
//	m_editDialog.exec();
}


void Graph::menuDownload()
{
	QUrl url = QUrl(p_clickedNode->source());
	QString info = p_clickedNode->artist() + " - " + p_clickedNode->title();
	Downloader::downloadFile(url, info);
}
