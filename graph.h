#ifndef GRAPH_H
#define GRAPH_H
#include <QGraphicsScene>
#include <QMenu>

#include "librarysearch.h"
#include "editnodedialog.h"
#include "lastfmsearch.h"

#include "menuactionloading.h"
#include "dialogaddtags.h"

class LastFmRelated;
class Node;
class Vkontakte;
class MyView;

class Graph : public QObject
{
	Q_OBJECT
private:
	QMenu m_menu;
	QMenu m_menuSources;
	MenuActionLoading m_menuAlbums;
	MenuActionLoading m_menuTags;
	QMenu m_menuDelete;
	QMenu m_menuSearch;
	QMenu m_menuLastFm;
	EditNodeDialog m_editDialog;
	DialogAddTags m_dialogAddTags;
	static QGraphicsScene *p_scene;
	static MyView *p_view;
	static QPoint m_point;

	LastFmRelated *p_lastFm;
	LastFmSearch m_lastFmSearch;
	Vkontakte *p_vkontakte;
	LibrarySearch *p_lib;
	static Node *p_clickedNode;
	QList<Node*> m_toDelete;

	static void showEditDialog();

	static Graph *p_instance;

public:
	Graph(QObject *parent = 0, QGraphicsScene *scene = 0, MyView *view = 0);
	void setScene(QGraphicsScene *scene){
		p_scene = scene;
	}

	QGraphicsScene *getScene() const {return p_scene;}
	LastFmRelated *lastFmRelated() const {return p_lastFm;}
	Vkontakte *vkontakte() const {return p_vkontakte;}
	void searchForSource(Node *node, bool onlineToo = false);

	void changeStatusInfo(QString info){emit statusInfoChanged(info);}

	LibrarySearch *library() const {return p_lib;}
	void setLibrary(LibrarySearch *lib) {p_lib = lib;}
	static MyView *view() {return p_view;}
	static Node *clickedNode() {return p_clickedNode;}
	QAction *showMenu(Node *sender);

	void setClickedNode(Node *node) {p_clickedNode = node;}
	void emitNodeDeletion(Node *deletedNode);
	static void createEditNode(QPointF point = QPoint());

signals:
	void statusInfoChanged(QString);
	void on_nodeDoubleClick(QString source);
	void playingNodeChanged();
	void nodeDeleted(Node *deletedNode);

protected:
//	void mou

private slots:
	void menuDownload();
	void menuGetRelated();
	void menuPlayTill();
	void menuEdit();
	void menuEditAccepted();
	void menuSearch();
	void menuSearchArtist();
	void menuSearchArtistAlbums();
	void menuSearchSimilar();
	void menuSetAsRoot();
	void menuDelete();
	void menuDeleteSiblings();
	void menuDeleteChildren();
	void menuDeleteTree();
	void menuDeleteOtherTrees();

	void menuShowSources();
	void menuShowAlbums();
	void menuShowTags();
	void menuSearchGoogle();

	void menuLoveTrack();
	void menuTagTrack();
	void onMenuHidden();
public slots:
	void updatePlayingNode();
	void createNode();
	Node *createNode(QString artist, QString title, QString source = QString(), QPointF point = QPointF() );
	void createAndPlayNode(QString artist, QString title, QString source = "");

};

#endif // GRAPH_H
