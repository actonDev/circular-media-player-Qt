#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
//#include <phonon>
#include "node.h"
#include <QSystemTrayIcon>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QtMultimedia/QMediaPlayer>
#include <QSlider>
#include <QIcon>
#include <QToolBar>
class Player : public QWidget
{
	Q_OBJECT


private:
	QSystemTrayIcon m_trayIcon;
	QMenu m_trayMenu;
	QToolButton *p_muteButton;
	QMediaPlayer m_player;
	QMediaContent m_content;

	QWidget m_timeWidget;
	QLabel m_timeAt;
	QLabel m_timeTotal;
	QLabel m_timeLabel;
	QLabel m_secPlayingLabel;
	int m_secPlaying;
	bool m_scrobbled;
	int m_secSince1970;
	void createControls();

	QToolBar *p_toolbar;
	QWidget *p_controlsWidget;
	QIcon m_iconStop;
	QIcon m_iconPause;
	QIcon m_iconPlay;
	QIcon m_iconNext;
	QIcon m_iconPrevious;
	QIcon m_iconMuteOff;
	QIcon m_iconMuteOn;

	QAction *p_actionStop;
	QAction *p_actionPlayPause;
	QAction *p_actionNext;
	QAction *p_actionPrevious;
	QAction *p_actionMute;
	QSlider *p_seekSlider;
	QSlider *p_volumeSlider;
	QLabel *p_timeLabel;
public:
	explicit Player(QWidget *parent = 0);
	~Player();

	QLabel *timeLabel() {return &m_timeLabel;}
	QWidget *timeWidget() {return &m_timeWidget;}
	static bool isInErrorState();
	QWidget *toolbar(){ return p_toolbar; }

signals:
	void currentInfoChanged(QString, QString, QString);
	void playingChanged();
	void playingStateChanged(bool playing);
private slots:
	void onStateChanged(QMediaPlayer::State state);
	void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
	void onDurationChanged(qint64 duration);
	void onPositionChanged(qint64 position);
	void onFinish();
	void seek(int seconds);
	void onMuteTriggered();
	void onMuteChanged(bool muted);
	void onMetaDataChanged(QString key,QVariant value);

public slots:
	void updatePlayingNode();
	void onNextTriggered();
	void onPreviousTriggered();
	void onPlayPauseTriggered();
};

#endif // PLAYER_H
