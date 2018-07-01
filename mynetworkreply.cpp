#include "mynetworkreply.h"
#include "QDebug"

MyNetworkReply::MyNetworkReply()
{
//	qDebug() << "CONSTRUCTOR my reply";
}

MyNetworkReply::~MyNetworkReply()
{
	//	qDebug() << "DESTRUCTOR my reply";
}

void MyNetworkReply::setReply(QNetworkReply *reply)
{
	p_reply = reply;
	p_reply->ignoreSslErrors();
	connect(p_reply, SIGNAL(finished()), this, SLOT(on_replyFinished()));
	connect(p_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
	connect(p_reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
}

void MyNetworkReply::setSender(Node *sender)
{
	p_sender = sender;
}

Node *MyNetworkReply::sender()
{
	return p_sender;
}

void MyNetworkReply::on_replyFinished()
{
	emit myReplyFinished(p_sender, p_reply);
	deleteLater();
}

void MyNetworkReply::onError(QNetworkReply::NetworkError error)
{
	qDebug() << "NETWROK ERROR!";
	qDebug() << error;
}

void MyNetworkReply::onSslErrors(QList<QSslError> errorList)
{
	qDebug() << "SSL ERRORS!";
	for(int i=0; i<errorList.count(); i++){
		qDebug() << "ssl error " << i << ": " << errorList.at(i).error() << ", " << errorList.at(i).errorString();
	}
}
