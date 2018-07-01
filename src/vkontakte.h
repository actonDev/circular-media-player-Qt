#ifndef VKONTAKTE_H
#define VKONTAKTE_H

#include <QObject>
#include <QtNetwork>
#include "node.h"
#include "mynetworkreply.h"
//  https://api.vk.com/method/audio.search.xml?q=noisia&access_token=0bd99ec00162f6760162f67667014c2fec001620163f67eeb7d843c5c19d66c

class Vkontakte : public QObject
{
	Q_OBJECT
private:
	static QNetworkAccessManager m_network;
	static Vkontakte *p_instance;
	static QNetworkReply *p_searchReply;
	static void doSearch(Node *sender);
	static QList<Node *> m_requestList;
	static QTimer m_timer;
	static void restartTimer();
	Vkontakte(QObject *parent = 0);

public:
	//this is used when search is done from "internet search"
	static void search(Node *sender);
	static void search(QString term);
	static Vkontakte *instance();

signals:
	void gotResults(QList< QList<QString> > results);

public slots:
	void on_timerSearch();
	void on_replyFinished(Node *sender, QNetworkReply* reply);

	//this is used when search is done from "internet search"
	void on_replyFinished();

};

#endif // VKONTAKTE_H
