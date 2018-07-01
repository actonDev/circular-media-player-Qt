#include "internetsearch.h"
#include "ui_searchformonline.h"
#include "settings.h"
#include <QMovie>

InternetSearch *InternetSearch::p_instance = 0;

InternetSearch::InternetSearch(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SearchFormOnline)
{
	ui->setupUi(this);

	ui->comboBox->addItem("track/artist");
	ui->comboBox->addItem("artist");
	ui->comboBox->addItem("tag");
	ui->comboBox->addItem("tag top tracks");
	ui->comboBox->addItem("tag top albums");
	ui->comboBox->addItem("artist top albums");
	ui->comboBox->addItem("album");
	ui->comboBox->addItem("similar");
	connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(onSearchModeChange(QString)));
	ui->tableView->setModel(&m_model);
	ui->tableView->setColumnWidth(0, 200);
	ui->tableView->setColumnWidth(1, 200);
	connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
	connect(&m_lastFmSearch, SIGNAL(gotResults(QList< QStringList >)), this, SLOT(setResults(QList< QStringList >)));
	setSearchMode("track/artist");
	ui->stackedWidget->setCurrentIndex(0);
	p_instance = this;

}

InternetSearch::~InternetSearch()
{
	delete ui;
	p_instance = 0;
}

void InternetSearch::setSearchArtist(QString artist)
{
	setSearchMode("artist");
	ui->lineEdit->setText(artist);
	doSearch();

	emit makeVisible();
}

void InternetSearch::setSearchArtistAlbums(QString artist)
{
	setSearchMode("artist top albums");
	ui->lineEdit->setText(artist);
	doSearch();

	emit makeVisible();
}

void InternetSearch::setSearchAlbum(QString artist, QString album)
{
	setSearchMode("album");
	ui->lineEditAlbumArtist->setText(artist);
	ui->lineEditAlbumTitle->setText(album);
	doSearch();

	emit makeVisible();
}

void InternetSearch::setSearchSimilar(QString artist, QString title)
{
	setSearchMode("similar");
	ui->lineEditArtist->setText(artist);
	ui->lineEditTitle->setText(title);
	doSearch();

	emit makeVisible();
}

void InternetSearch::setSearchTagTopTracks(QString tag)
{
	setSearchMode("tag top tracks");
	ui->lineEdit->setText(tag);
	doSearch();

	emit makeVisible();
}
void InternetSearch::searchForTrack()
{
	QString term = ui->lineEdit->text();
	m_lastFmSearch.searchForTrack(term);
}

void InternetSearch::searchForTag()
{
	QString term = ui->lineEdit->text();
	m_lastFmSearch.searchTagTopTracks(term);
}

void InternetSearch::doSearch()
{
//	m_widgetLoading.setLoading(true);
	ui->tableView->setLoading(true);
	qDebug() << "seach mode " << m_searchMode;
	QString term = ui->lineEdit->text();
	if(m_searchMode=="track/artist")
		m_lastFmSearch.searchForTrack(term);
	else if(m_searchMode == "artist")
		m_lastFmSearch.searchForArtist(term);
	else if(m_searchMode == "tag")
		m_lastFmSearch.searchForTag(term);
	else if(m_searchMode=="tag top tracks")
		m_lastFmSearch.searchTagTopTracks(term);
	else if(m_searchMode == "tag top albums")
		m_lastFmSearch.searchForTagTopAlbums(term);
	else if(m_searchMode == "artist top albums")
		m_lastFmSearch.searchForArtistTopAlbums(term);

	else if(m_searchMode == "album"){
		QString artist = ui->lineEditAlbumArtist->text();
		QString album = ui->lineEditAlbumTitle->text();
		m_lastFmSearch.searchAlbumTracks(artist, album);
	}
	else if(m_searchMode == "similar"){
		QString artist = ui->lineEditArtist->text();
		QString title = ui->lineEditTitle->text();
		m_lastFmSearch.searchForSimilar(artist, title);
	}
}

void InternetSearch::setSearchMode(QString mode)
{
//	if(height() < 100)
//		qDebug() << "hidden!";

	m_searchMode = mode;
	for(int i=0; i<ui->comboBox->count(); i++){
		if(ui->comboBox->itemText(i) == mode){
			ui->comboBox->setCurrentIndex(i);
			break;
		}
	}
	if(mode == "album"){
		ui->stackedWidget->setCurrentIndex(1);
	}
	else if(mode == "similar"){
		ui->stackedWidget->setCurrentIndex(2);
	}
	else{
		ui->stackedWidget->setCurrentIndex(0);
	}
}

void InternetSearch::onSearchModeChange(QString mode)
{
	setSearchMode(mode);
}

void InternetSearch::on_searchButton_clicked()
{
	doSearch();
}

void InternetSearch::onDoubleClicked(QModelIndex index)
{
	qDebug() << "dclick";
	int row = index.row();
	if(m_model.headerTitle(0) == "Tag"){
		QString tag = m_model.record(row,0);
		setSearchTagTopTracks(tag);
	}
	if(m_model.headerTitle(1)=="Title"){
		QString artist = m_model.record(row, 0);
		QString title = m_model.record(row, 1);
		//to start playing this
		emit doubleCliked(artist, title, "");
	}
	else if(m_model.headerTitle(1)=="Album"){
		qDebug() << "set search album";
		QString artist = m_model.record(row, 0);
		QString album = m_model.record(row, 1);
		setSearchAlbum(artist, album);
	}
}

void InternetSearch::setResults(QList<QStringList> results)
{
	qDebug() << "set results";
//	m_widgetLoading.setLoading(false);
	ui->tableView->setLoading(false);
	m_model.setResults(results);
	ui->tableView->scrollToTop();
	ui->tableView->clearSelection();
}


