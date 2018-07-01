#include "player.h"
#include "QDebug"
#include <QMessageBox>
#include "settings.h"
#include "tools.h"
#include <QVBoxLayout>
#include "objectmanager.h"
#include <QSlider>
#include "lastfm.h"

//Phonon::MediaObject Player::m_mediaObject;

Player::Player(QWidget *parent) :
	QWidget(parent)
{
	m_scrobbled = false;
	connect(&m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
	connect(&m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
	connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
	connect(&m_player, SIGNAL(mutedChanged(bool)), this, SLOT(onMuteChanged(bool)));
	connect(&m_player, SIGNAL(metaDataChanged(QString,QVariant)), this, SLOT(onMetaDataChanged(QString,QVariant)));
	QIcon icon(":/cmp_logo.png");
	m_trayIcon.setIcon(icon);
	m_trayIcon.show();
	createControls();

	connect(p_actionStop, SIGNAL(triggered()), &m_player, SLOT(stop()));
	connect(p_actionPlayPause, SIGNAL(triggered()), this, SLOT(onPlayPauseTriggered()));
	connect(p_actionPrevious, SIGNAL(triggered()), this, SLOT(onPreviousTriggered()));
	connect(p_actionNext, SIGNAL(triggered()), this, SLOT(onNextTriggered()));

	QAction *trayPrev = new QAction("Previous", this);
	QAction *trayPause = new QAction("Play/pause", this);
	QAction *trayNext = new QAction("Next", this);
	m_trayMenu.addAction(trayPrev);
	m_trayMenu.addAction(trayPause);
	m_trayMenu.addAction(trayNext);
	m_trayIcon.setContextMenu(&m_trayMenu);
	connect(trayPrev, SIGNAL(triggered()), this, SLOT(onPreviousTriggered()));
	connect(trayPause, SIGNAL(triggered()), this, SLOT(onPlayPauseTriggered()));
	connect(trayNext, SIGNAL(triggered()), this, SLOT(onNextTriggered()));

	m_player.setVolume(100);
	m_player.setNotifyInterval(1000);
	connect(&m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
	m_secPlaying = 0;

}

Player::~Player()
{
	m_trayIcon.hide();
}

bool Player::isInErrorState()
{
	//	if(m_mediaObject.state() == Phonon::ErrorState)
	//		return true;
	//	else
	//		return false;
	return false;
}

void Player::onStateChanged(QMediaPlayer::State state)
{
	qDebug() << "state changed " << state;
	if(state == QMediaPlayer::PlayingState){
		p_actionPlayPause->setIcon(m_iconPause);
	}
	else{
		p_actionPlayPause->setIcon(m_iconPlay);
	}
}

void Player::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	qDebug() << "media status: " << status;
	if(status == QMediaPlayer::EndOfMedia){
		Node *playingNode = Node::playingNode();
		if(playingNode!=0 && Settings::instance()->PARTY_MODE){
			if(playingNode->getChildrenList()->count()!=0){
				if(!playingNode->hasNext()){
					playingNode->onRelatedFinished();
				}
				onNextTriggered();
			}
			else{
				if(playingNode->getParent()!=0){
					QList<Node *> *parentChildren = playingNode->getParent()->getChildrenList();
					if(parentChildren->count()>1){
						for(int i=0; i<parentChildren->count(); i++){
							if(parentChildren->at(i)!= playingNode && parentChildren->at(i)->hasSource()){
								parentChildren->at(i)->setAsPlayingNode(true);
								break;
							}
						}
					}
				}
			}
		}
		// normal behaviour
		else
			onNextTriggered();
	}
	else if(status == QMediaPlayer::InvalidMedia){
		m_trayIcon.showMessage("Source not valid!", "will search for new source..", QSystemTrayIcon::Warning );
		Node *playingNode = Node::playingNode();
		if(playingNode!=0){
			playingNode->setSource("null");
			playingNode->clearSources();
			playingNode->searchForSource();
		}
	}
}

void Player::onDurationChanged(qint64 duration)
{
	m_secPlaying = 0;
	p_seekSlider->setRange(0, duration/1000);
	m_scrobbled = false;

	QDateTime time = QDateTime::currentDateTimeUtc();
	m_secSince1970 = time.toMSecsSinceEpoch()/1000;

	Node *playingNode = Node::playingNode();

	if(playingNode==0) return;
	QString artist = playingNode->artist();
	QString title = playingNode->title();
	QString source = playingNode->source();
	int timeSec = duration/1000;
	if(timeSec!=0){
		if(Settings::instance()->NOW_PLAYING){
			//			Settings::instance()->lastFmNowPlaying(artist, title, timeSec);
			LastFm::updateNowPlaying(artist, title, duration/1000);
		}
		emit currentInfoChanged(artist, title, source);

		if(Settings::instance()->NOW_PLAYING_NOTIFICATION)
			m_trayIcon.showMessage("Now playing:", artist + " - " + title);
		m_trayIcon.setToolTip("Now playing:\n" + artist + " - " + title);
	}
}

void Player::onPositionChanged(qint64 position)
{
	p_seekSlider->blockSignals(true);
	p_seekSlider->setValue(position/1000);
	p_seekSlider->blockSignals(false);

	//	qDebug() << "bitrate: " << m_player.metaData("BITRATE");


	m_secPlaying++;
	//	m_secPlayingLabel.setText(QString::number(m_secPlaying));
	long pos = m_player.position();
	long len = m_player.duration();
	QString posString = Tools::timeStringFromMilliseconds(pos);
	QString lenString = Tools::timeStringFromMilliseconds(len);
	p_timeLabel->setText(posString + " / " + lenString);

	Node *playingNode = Node::playingNode();
	if(playingNode==0) return;
	QString artist = playingNode->artist();
	QString title = playingNode->title();
	//check scrobble
	if(Settings::instance()->SESSION_KEY!="" && Settings::instance()->SCROBBLE){
		if(!m_scrobbled && m_secPlaying>30 && m_secPlaying> ((len/1000)/2) ){
			m_scrobbled = true;
			qDebug() << "TIME TO SCROBBLE";
			//			Settings::instance()->lastFmScrobble(artist, title, m_secSince1970);
			LastFm::scrobble(artist, title, m_secSince1970);
		}
	}
}

void Player::onFinish()
{
	//	onNextTriggered();
}

void Player::updatePlayingNode()
{
	Node *playingNode = Node::playingNode();
	if(playingNode == 0) return;
	qDebug() << "Player::setPlaying(Node *node)";
	QString source = playingNode->source();
	QUrl sourceUrl;
	source.resize(4);
	if(source == "http"){
		sourceUrl = QUrl(playingNode->source());
	}
	else{
		sourceUrl = QUrl::fromLocalFile(playingNode->source());
	}
	qDebug() << "play : " << playingNode->artist() << " " << playingNode->title() << "ur: " << sourceUrl;
	m_player.setMedia(QMediaContent(sourceUrl));
	m_player.play();
	m_player.setMedia(QUrl(playingNode->source()));
	m_player.play();
}

void Player::onNextTriggered()
{
	if(Settings::instance()->PLAYING_SHUFFLE_MODE == true){
		ManagedObject *randomObject = ObjectManager::randomObject();
		if(randomObject == 0) return;
		Node *randomNode = (Node*)randomObject;
		if(randomNode == 0) return;
		randomNode->setAsPlayingNode(true);
		return;
	}
	//normal mode

	Node *playingNode = Node::playingNode();

	if(playingNode==0) return;
	if(playingNode->hasNext()){
		playingNode->next()->setAsPlayingNode(true);
	}
}

void Player::onPreviousTriggered()
{
	Node *playingNode = Node::playingNode();
	if(playingNode==0) return;
	if(playingNode->getParent()!=0){
		playingNode->getParent()->setAsPlayingNode();
	}
}

void Player::onPlayPauseTriggered()
{
	QMediaPlayer::State state = m_player.state();
	qDebug() << "state: " << state;
	if(state == QMediaPlayer::StoppedState)
		updatePlayingNode();
	else if(state == QMediaPlayer::PlayingState)
		m_player.pause();
	else
		m_player.play();
}

void Player::createControls(){
	m_iconStop.addFile(Settings::instance()->themeFolder() + "/transport/stop.png");
	m_iconPause.addFile(Settings::instance()->themeFolder() + "/transport/pause.png");
	m_iconPlay.addFile(Settings::instance()->themeFolder() + "/transport/play.png");
	m_iconNext.addFile(Settings::instance()->themeFolder() + "/transport/next.png");
	m_iconPrevious.addFile(Settings::instance()->themeFolder() + "/transport/previous.png");
	m_iconMuteOff.addFile(Settings::instance()->themeFolder() + "/transport/mute_off.png");
	m_iconMuteOn.addFile(Settings::instance()->themeFolder() + "/transport/mute_on.png");

	p_actionStop = new QAction(m_iconStop, "stop", this);
	p_actionPrevious = new QAction(m_iconPrevious,"previous", this);
	p_actionPlayPause = new QAction(m_iconPlay,"play/pause", this);
	p_actionNext = new QAction(m_iconNext,"next", this);
	p_actionMute = new QAction(m_iconMuteOff, "mute", this);

	p_timeLabel = new QLabel("0:00 / 0:00", this);

	p_seekSlider = new QSlider(Qt::Horizontal, this);
	p_volumeSlider = new QSlider(Qt::Horizontal, this);
	p_volumeSlider->setMaximumWidth(100);

	p_toolbar = new QToolBar(this);
	p_toolbar->setObjectName("TransportBar");
	QSize size = QSize(26,26);
	p_toolbar->setIconSize(size);

	p_toolbar->addAction(p_actionStop);
	p_toolbar->addAction(p_actionPrevious);
	p_toolbar->addAction(p_actionPlayPause);
	p_toolbar->addAction(p_actionNext);

	p_toolbar->addWidget(p_seekSlider);
	p_toolbar->addWidget(p_timeLabel);
	p_toolbar->addAction(p_actionMute);
	p_toolbar->addWidget(p_volumeSlider);

	p_volumeSlider->setRange(0, 100);
	m_player.setVolume(100);
	p_volumeSlider->setValue(m_player.volume());
	connect(p_volumeSlider, SIGNAL(valueChanged(int)), &m_player, SLOT(setVolume(int)));
	connect(p_seekSlider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
	connect(p_actionMute, SIGNAL(triggered()), this, SLOT(onMuteTriggered()));

}

void Player::seek(int seconds){
	qDebug() << "seek " << seconds;
	m_player.setPosition(seconds*1000);
}

void Player::onMuteTriggered()
{
	if(m_player.isMuted())
		m_player.setMuted(false);
	else
		m_player.setMuted(true);
}

void Player::onMuteChanged(bool muted)
{
	if(muted)
		p_actionMute->setIcon(m_iconMuteOn);
	else
		p_actionMute->setIcon(m_iconMuteOff);
}

void Player::onMetaDataChanged(QString key, QVariant value)
{
	qDebug() << "got metadata: " << key << ": " << value;
}
