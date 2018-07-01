#ifndef MYNETWORKREPLY_H
#define MYNETWORKREPLY_H

#include <QNetworkReply>
#include "node.h"
#include <QSslError>


class MyNetworkReply : public QObject
{
	Q_OBJECT
	Node *p_sender;
	QNetworkReply *p_reply;
public:
	MyNetworkReply();
	~MyNetworkReply();

	void setReply(QNetworkReply *reply);
	QNetworkReply *reply(){return p_reply;}

	void setSender(Node *sender);
	Node *sender();

private slots:
	void on_replyFinished();
	void onError(QNetworkReply::NetworkError error);
	void onSslErrors(QList<QSslError> errorList);
signals:
	void myReplyFinished(Node*, QNetworkReply*);
};

#endif // MYNETWORKREPLY_H
