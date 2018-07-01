#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>


class DownloaderNetworkReply : public QObject
{
	Q_OBJECT
	QNetworkReply *p_reply;
	QString m_info;
public:
	DownloaderNetworkReply(QString info);
	~DownloaderNetworkReply();

	void setReply(QNetworkReply *reply);
	QNetworkReply *reply(){return p_reply;}
	QString info() {return m_info;}


private slots:
	void on_replyFinished();

signals:
	void downloaderReplyFinished(DownloaderNetworkReply*, QString);
};


class Downloader : public QObject
{
	Q_OBJECT
private:
	explicit Downloader(QObject *parent = 0);

	static QNetworkAccessManager m_network;
	static Downloader *p_instance;
	static Downloader *instance();
public:
	static void downloadFile(QUrl url, QString info);
	
signals:
	
public slots:
	void downloadFinished(DownloaderNetworkReply* downloaderReply,QString info);
	
};

#endif // DOWNLOADER_H
