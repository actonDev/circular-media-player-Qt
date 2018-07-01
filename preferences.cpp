#include "preferences.h"
#include "ui_preferences.h"
#include "settings.h"
#include "lastfm.h"

namespace {
	const int MAX_GET_IF_LESS_THAN = 3;
	const int MAX_TO_ADD = 10;
}
Preferences::Preferences(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences)
{
	ui->setupUi(this);
	ui->opt_zoomFactor->setValidator(new QDoubleValidator(1.0, 2.0, 2, this));
	QIntValidator *intValid = new QIntValidator(0, 40, this);
	ui->opt_maxStringLength->setValidator(intValid);
	ui->opt_maxStringLengthHover->setValidator(intValid);
	ui->opt_tooltipViewZoom->setValidator(new QDoubleValidator(0.0, 1.0, 1));

	ui->opt_relatedToAdd->setValidator(new QIntValidator(0,10,this));
	ui->opt_getRelatedIfLessThan->setValidator(new QIntValidator(0,3,this));

	QIntValidator *intPositiveValid = new QIntValidator(0, 1000000, this);
	ui->opt_dontAddHistoryLastNHours->setValidator(intPositiveValid);
	ui->opt_autoSavePlaylistMins->setValidator(intPositiveValid);

	ui->opt_lastFmUsername->setText(Settings::instance()->LASTFM_USERNAME);
	if(Settings::instance()->SESSION_KEY!="")
		ui->opt_lastFmStatus->setText("ok");
	else{
		ui->opt_scrobble->setEnabled(false);
		ui->opt_nowPlaying->setEnabled(false);
	}

	resetOptions();
	LastFm::setSession(Settings::instance()->LASTFM_USERNAME, Settings::instance()->SESSION_KEY);
	connect(LastFm::instance(), SIGNAL(lastFmAuthorizationInfo(QString)), this, SLOT(onLastFmAuthorization(QString)));
}

Preferences::~Preferences()
{
	delete ui;
}

void Preferences::resetOptions()
{
	//GENERAL
	ui->opt_checkUpdates->setChecked(Settings::instance()->CHECK_UPDATES);
	ui->opt_intstantSearch->setChecked(Settings::instance()->INSTANT_SEARCH);
	ui->opt_reorder->setChecked(Settings::instance()->REORDER_ON_MOUSE_RELEASE);
	ui->opt_zoom->setChecked(Settings::instance()->ZOOM_ON_HOVER);
	ui->opt_zoomFactor->setText(QString::number(Settings::instance()->ZOOM_FACTOR));
	ui->opt_maxStringLength->setText(QString::number(Settings::instance()->MAX_STRING_LENGTH));
	ui->opt_maxStringLengthHover->setText(QString::number(Settings::instance()->MAX_STRING_LENGTH_HOVER));
	ui->opt_autoSavePlaylistMins->setText(QString::number(Settings::instance()->AUTO_SAVE_PLAYLIST_MINS));

	ui->opt_tooltip->setChecked(Settings::instance()->TOOLTIP_ON_HOVER);
	ui->opt_tooltipClipped->setChecked(Settings::instance()->TOOLTIP_WHEN_CLIPPED);
	ui->opt_tooltipViewZoom->setText(QString::number(Settings::instance()->TOOLTIP_VIEW_ZOOM_LESS_THAN));
	ui->opt_nowPlayingNotification->setChecked(Settings::instance()->NOW_PLAYING_NOTIFICATION);

	//RELATED
	ui->opt_relatedToAdd->setText( QString::number(Settings::instance()->RELATED_TO_ADD) );
	ui->opt_dontAddSameArtist->setChecked(Settings::instance()->DONT_ADD_SAME_ARTIST);
	ui->opt_dontAddHistoryLastNHours->setText(QString::number(Settings::instance()->DONT_ADD_HISTORY_LAST_N_HOURS));
	ui->opt_getRelatedIfLessThan->setText( QString::number(Settings::instance()->GET_RELATED_IF_LESS_THAN) );

	// ### LAST.FM
	ui->opt_scrobble->setChecked(Settings::instance()->SCROBBLE);
	ui->opt_nowPlaying->setChecked(Settings::instance()->NOW_PLAYING);
	ui->opt_skipUnknownArtist->setChecked(Settings::instance()->SKIP_UNKNOWN_ARTIST);
}

void Preferences::setOptions()
{

	//### GENERAL ###
	Settings::instance()->CHECK_UPDATES = ui->opt_checkUpdates->isChecked();
	Settings::instance()->INSTANT_SEARCH = ui->opt_intstantSearch->isChecked();
	Settings::instance()->REORDER_ON_MOUSE_RELEASE = ui->opt_reorder->isChecked();
	Settings::instance()->ZOOM_ON_HOVER = ui->opt_zoom->isChecked();
	Settings::instance()->ZOOM_FACTOR = ui->opt_zoomFactor->text().toDouble();
	Settings::instance()->MAX_STRING_LENGTH = ui->opt_maxStringLength->text().toInt();
	Settings::instance()->MAX_STRING_LENGTH_HOVER = ui->opt_maxStringLengthHover->text().toInt();
	Settings::instance()->setAutoSavePlaylistMins(ui->opt_autoSavePlaylistMins->text().toInt());
	Settings::instance()->TOOLTIP_ON_HOVER = ui->opt_tooltip->isChecked();
	Settings::instance()->TOOLTIP_WHEN_CLIPPED = ui->opt_tooltipClipped->isChecked();
	Settings::instance()->TOOLTIP_VIEW_ZOOM_LESS_THAN = ui->opt_tooltipViewZoom->text().toDouble();
	Settings::instance()->NOW_PLAYING_NOTIFICATION = ui->opt_nowPlayingNotification->isChecked();
	//### RELATED ###

	//RELATED_TO_ADD
	int toAdd = ui->opt_relatedToAdd->text().toInt();
	if(toAdd == 0)
		ui->opt_relatedToAdd->setText("0");
	if(toAdd<=MAX_TO_ADD)
		Settings::instance()->RELATED_TO_ADD = toAdd;
	else{
		ui->opt_relatedToAdd->setText( QString::number(MAX_TO_ADD) );
		Settings::instance()->RELATED_TO_ADD = MAX_TO_ADD;
	}

	//DONT ADD ....
	Settings::instance()->DONT_ADD_SAME_ARTIST = ui->opt_dontAddSameArtist->isChecked();
	Settings::instance()->DONT_ADD_HISTORY_LAST_N_HOURS = ui->opt_dontAddHistoryLastNHours->text().toInt();

	//GET_RELATED_IF_LESS_THAN
	int getIfLess = ui->opt_getRelatedIfLessThan->text().toInt();
	if(getIfLess == 0){
		ui->opt_getRelatedIfLessThan->setText("0");
	}
	if(getIfLess <= MAX_GET_IF_LESS_THAN){
		Settings::instance()->GET_RELATED_IF_LESS_THAN = getIfLess;
	}
	else{
		ui->opt_getRelatedIfLessThan->setText( QString::number(MAX_GET_IF_LESS_THAN) );
		Settings::instance()->GET_RELATED_IF_LESS_THAN = MAX_GET_IF_LESS_THAN;
	}


	// ### LAST.FM
	Settings::instance()->SCROBBLE = ui->opt_scrobble->isChecked();
	Settings::instance()->NOW_PLAYING = ui->opt_nowPlaying->isChecked();
	Settings::instance()->SKIP_UNKNOWN_ARTIST = ui->opt_skipUnknownArtist->isChecked();

}

void Preferences::reject()
{
//	qDebug() << "reject";
	resetOptions();
	hide();
}

void Preferences::accept()
{
//	qDebug() << "accept";
	setOptions();
	hide();
}

void Preferences::apply()
{
	setOptions();
}

void Preferences::authenticate()
{
	QString user = ui->opt_lastFmUsername->text();
	QString pass = ui->opt_lastFmPassword->text();
//	qDebug() << "authenticate: " << user << " : " << pass;

//	Settings::instance()->lastFmAuthorize(user, pass);
	LastFm::authorize(user, pass);
}

void Preferences::onLastFmAuthorization(QString text)
{
	ui->opt_lastFmStatus->setText(text);
	if(text == "ok"){
		ui->opt_scrobble->setEnabled(true);
		ui->opt_nowPlaying->setEnabled(true);
	}
	else{
		ui->opt_scrobble->setEnabled(false);
		ui->opt_nowPlaying->setEnabled(false);
	}
}
