#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include <QNetworkAccessManager>

class LastFm : public QObject
{
	Q_OBJECT
private:
	static QNetworkAccessManager m_network;
	static LastFm* p_instance;

	static QString m_apiKey;
	static QString m_apiSecret;
	static QString m_sessionKey;
	static QString m_user;

	LastFm(QObject *parent = 0);

public:

	static void setSession(QString user, QString sessionKey);
	static LastFm* instance();
	static void authorize(QString user, QString pass);
	static void updateNowPlaying(QString artist, QString title, int duration=0);
	static void scrobble(QString artist, QString title, int secSince1970);
	static void trackAddTags(QString artist, QString track, QString tags);
	static void trackRemoveTag(QString artist, QString track, QString tag);
	static void loveTrack(QString artist, QString track);
	static void unloveTrack(QString artist, QString track);
	static bool authorized();
	
signals:
	void lastFmAuthorizationInfo(QString);
	void infoSignal(QString);
private slots:
	void replyFinished(QNetworkReply *reply);
public slots:

	
};

#endif // LASTFM_H
