#ifndef UPDATECHECK_H
#define UPDATECHECK_H

#include <QObject>
#include <QtNetwork>
#include "dialogupdate.h"

class UpdateCheck : public QObject
{
	Q_OBJECT
private:
	QNetworkAccessManager m_network;
	DialogUpdate m_dialogUpdate;
	QString m_info;
	QString m_dateString;
	QTimer m_timer;
	QString m_user;

	void logEnd();

public:
	explicit UpdateCheck(QObject *parent = 0);
	~UpdateCheck();
	
signals:
	
private slots:
	void on_replyFinished(QNetworkReply *reply);
	void logKeepAlive();
	void logStart();
public slots:
	
};

#endif // UPDATECHECK_H
