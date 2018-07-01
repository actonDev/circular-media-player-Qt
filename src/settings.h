#ifndef SETTINGS_H
#define SETTINGS_H
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtXml/QDomDocument>
#include "node.h"
class Settings : public QObject
{
	Q_OBJECT
private:
	Settings();
	QNetworkAccessManager m_network;
	void writeOptions();
	void readOptions();
	void readThemeOptions();
	void saveChildNode(QDomDocument *document, QDomElement *domParent, Node *nodeParent);
	void loadChildNode(QDomElement *domParent, Node *nodeParent);
	QTimer m_timerSavePlaylist;
	void updateTimerSavePlaylist();
public:
	~Settings();
	static Settings *instance(){
		static Settings* pOptionsInstance = new Settings();
		return pOptionsInstance;
	}
	QString version;

	bool DONT_SHOW_HELP;
	bool CHECK_UPDATES;
	bool SUBMIT_INFO;
	QRect GEOMETRY;
	bool SET_PLAYING_AS_ROOT;
	bool INSTANT_SEARCH;
	bool REORDER_ON_MOUSE_RELEASE;
	bool ZOOM_ON_HOVER;
	double ZOOM_FACTOR;
	bool TOOLTIP_ON_HOVER;
	bool TOOLTIP_WHEN_CLIPPED;
	qreal TOOLTIP_VIEW_ZOOM_LESS_THAN;
	bool NOW_PLAYING_NOTIFICATION;
	int MAX_STRING_LENGTH;
	int MAX_STRING_LENGTH_HOVER;
	int AUTO_SAVE_PLAYLIST_MINS;
	double TEXT_BACKGROUND_OPACITY;

	int RELATED_TO_ADD;
	bool DONT_ADD_SAME_ARTIST;
	int GET_RELATED_IF_LESS_THAN;
	int DONT_ADD_HISTORY_LAST_N;
	int DONT_ADD_HISTORY_LAST_N_HOURS;
	QString SESSION_KEY;
	QString LASTFM_USERNAME;
	bool SCROBBLE;
	bool NOW_PLAYING;
	bool SKIP_UNKNOWN_ARTIST;


	QString THEME;
	int EDGELINE_NORMAL_WIDTH;
	QString EDGELINE_NORMAL_COLOR;
	int EDGELINE_ACTIVE_WIDTH;
	QString EDGELINE_ACTIVE_COLOR;
	QString CIRCLE_TEXT_COLOR;
	QString CIRCLE_HOVERED_TEXT_COLOR;
	QString CIRCLE_HOVERED_TEXT_BACKGROUND_COLOR;

	bool PLAYING_SHUFFLE_MODE;
	bool PARTY_MODE;

	void lastFmAuthorize(QString user, QString pass);
	void lastFmNowPlaying(QString artist, QString title, int duration=0);
	void lastFmScrobble(QString artist, QString title, int secSince1970);
	QString themeFolder();
	void updateThemeFolder();
	void setThemeFolder(QString folder);	
	void loadPlaylist(bool defaultPlaylist = true, bool clear = false);
	void loadPlaylist(QString fileName, bool clear = false);
	void setAutoSavePlaylistMins(int mins);

public slots:
	void savePlaylist(bool defaultPlaylist = true);

private slots:
	void replyFinished(QNetworkReply *reply);

signals:
	void lastFmAuthorizationInfo(QString info);
	void infoSignal(QString info);
};
#endif // SETTINGS_H
