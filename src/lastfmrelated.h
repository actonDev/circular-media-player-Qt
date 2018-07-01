#ifndef LASTFMRELATED_H
#define LASTFMRELATED_H


#include <QObject>
#include <QtNetwork>
#include <QDebug>
#include <QGraphicsScene>

class Node;
class Graph;

class LastFmRelated : public QObject
{
	Q_OBJECT
private:
	QNetworkAccessManager m_network;
	Graph *p_graph;
	QList<Node*> m_requestingNodes;

	bool isNodeValid(Node *node);
	bool nodeExists(Node *node);
public:
	LastFmRelated(QObject *parent = 0, Graph *graph = 0);
	void getRelated(Node *node);

public slots:
	void replyFinished(Node *sender, QNetworkReply* reply);
	void onNodeDeleted(Node *deletedNode);
};

#endif // LASTFM_H
