#include "downloader.h"
#include <QFile>
#include <QNetworkReply>
#include <QFileInfo>
#include <QNetworkRequest>
#include "Tools.h"

QNetworkAccessManager Downloader::m_network;
Downloader *Downloader::p_instance = 0;

Downloader::Downloader(QObject *parent) :
	QObject(parent)
{
}

Downloader *Downloader::instance()
{
	if(p_instance == 0){
		p_instance = new Downloader();
	}
	return p_instance;
}

void Downloader::downloadFile(QUrl url, QString info)
{
	qDebug() << "download " << url;
	instance();
	QNetworkRequest request;
	request.setUrl(url);
	DownloaderNetworkReply *reply = new DownloaderNetworkReply(info);
	reply->setReply(m_network.get(request));
	connect(reply, SIGNAL(downloaderReplyFinished(DownloaderNetworkReply*,QString)), instance(), SLOT(downloadFinished(DownloaderNetworkReply*, QString)));
}


void Downloader::downloadFinished(DownloaderNetworkReply *downloaderReply, QString info)
{
	qDebug() << "download finished!";
	QFileInfo fileInfo(downloaderReply->reply()->url().toString());
	QFile localFile(Tools::resourcesLocation() + "/Downloads/" + info + " _" + fileInfo.fileName());
	qDebug() << localFile.fileName();
	if (!localFile.open(QIODevice::WriteOnly)){
		qDebug() << "failed to open";
		return;
	}
	localFile.write(downloaderReply->reply()->readAll());
	localFile.close();
	delete downloaderReply;
}


DownloaderNetworkReply::DownloaderNetworkReply(QString info)
{
	m_info = info;
}

DownloaderNetworkReply::~DownloaderNetworkReply()
{
	p_reply->deleteLater();
}


void DownloaderNetworkReply::setReply(QNetworkReply *reply)
{
	p_reply = reply;
	connect(reply, SIGNAL(finished()), this, SLOT(on_replyFinished()));
}

void DownloaderNetworkReply::on_replyFinished()
{
	emit downloaderReplyFinished(this, m_info);
}
