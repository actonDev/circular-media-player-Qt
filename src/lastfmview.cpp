#include "lastfmview.h"
#include "ui_lastfmview.h"

#include "settings.h"
#include "lastfm.h"

#include "tagwidgetitem.h"
#include "treewidgettag.h"
#include "treewidgetplaylist.h"


LastFmView::LastFmView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LastFmView)
{
	ui->setupUi(this);


	ui->tableView->setModel(&m_model);
	ui->tableView->setColumnWidth(0, 200);
	ui->tableView->setColumnWidth(1, 200);

	connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(updateUser()));
	connect(ui->okButton, SIGNAL(clicked()), this, SLOT(updateUser()));
	connect(&m_lastFmSearch, SIGNAL(gotResults(QList< QStringList >)), this, SLOT(setResults(QList< QStringList >)));
	connect(&m_lastFmSearch, SIGNAL(gotUserTopTags(QList<QPair<QString,QString> >)), this, SLOT(setTopTags(QList<QPair<QString,QString> >)));

	connect(&m_lastFmSearch, SIGNAL(gotUserPlaylists(QList<QStringList>)), this, SLOT(setPlaylists(QList<QStringList>)));

	connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
	//		connect(ui->treeWidget, SIGNAL(cu
	connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
	//	connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
	connect(ui->tableView, SIGNAL(rightClicked(QModelIndex)), this, SLOT(onRightClicked(QModelIndex)));
	QString lastFmUsername = Settings::instance()->LASTFM_USERNAME;
	if(lastFmUsername != ""){
		ui->lineEdit->setText(lastFmUsername);
		updateUser();
	}


	p_actionUnlove = new QAction("unlove",this);
	connect(p_actionUnlove, SIGNAL(triggered()), this, SLOT(menuUnlove()));
	p_actionRemoveTag = new QAction("remove this tag", this);
	connect(p_actionRemoveTag, SIGNAL(triggered()), this, SLOT(menuRemoveTag()));
	m_menu.addAction(p_actionUnlove);
	m_menu.addAction(p_actionRemoveTag);

	QList<int> sizes;
	sizes.append(100);
	sizes.append(300);
	ui->splitter->setSizes(sizes);

}

LastFmView::~LastFmView()
{
	delete ui;
}


void LastFmView::setResults(QList<QStringList> results)
{
	m_model.setResults(results);
	ui->tableView->scrollToTop();
	ui->tableView->clearSelection();
	ui->tableView->setLoading(false);
}

void LastFmView::setTopTags(QList< QPair<QString, QString> > results)
{
	QPair<QString, QString> entry;
	QList<QTreeWidgetItem *> list = ui->treeWidget->findItems("Tagged tracks",Qt::MatchExactly);
	QTreeWidgetItem *tagsItem = list.at(0);

	for(int i=0; i<results.count(); i++){
		entry = results.at(i);
		TreeWidgetTag *treeItem = new TreeWidgetTag(0,entry.first, entry.second);
		if(list.count()!=0){
			tagsItem->addChild(treeItem);
		}

	}
	tagsItem->setExpanded(true);
}

void LastFmView::setPlaylists(QList<QStringList> results)
{
	qDebug() << "set playlists";
	QStringList entry;
	QList<QTreeWidgetItem *> list = ui->treeWidget->findItems("Playlists",Qt::MatchExactly);
	QTreeWidgetItem *parent = list.at(0);

	for(int i=0; i<results.count(); i++){
		entry = results.at(i);

		//QTreeWidgetItem *treeItem = new QTreeWidgetItem();
		//treeItem->setText(0, entry.at(1));
		TreeWidgetPlaylist *playlist = new TreeWidgetPlaylist(0, entry.at(0), entry.at(1), entry.at(2));
		if(list.count()!=0){
			parent->addChild(playlist);
		}
	}
	parent->setExpanded(true);
}

void LastFmView::updateUser()
{
	m_user = ui->lineEdit->text();
	clearTags();
	clearPlaylists();
//	ui->treeWidget->setCurrentItem();
	ui->tableView->setLoading(true);
	m_lastFmSearch.searchUserLovedTracks(m_user);
	m_lastFmSearch.searchUsertTopTags(m_user);
	m_lastFmSearch.userGetPlaylists(m_user);
}

void LastFmView::onDoubleClicked(QModelIndex index)
{

	int row = index.row();
	if(m_model.headerTitle(1)=="Title"){
		QString artist = m_model.record(row, 0);
		QString title = m_model.record(row, 1);
		//to start playing this
		emit doubleCliked(artist, title, "");
		qDebug() << "last fm double click " << artist << " - " << title;

	}
}

void LastFmView::clearTags()
{
	QList<QTreeWidgetItem *> list = ui->treeWidget->findItems("Tagged tracks",Qt::MatchExactly);
	if(list.count()!=0){
		int count = list.at(0)->childCount();
		for(int i=count-1; i>=0; i--){
			QTreeWidgetItem *item = list.at(0)->child(i);
			delete item;
		}
	}
}

void LastFmView::clearPlaylists()
{
	QList<QTreeWidgetItem *> list = ui->treeWidget->findItems("Playlists",Qt::MatchExactly);
	if(list.count()!=0){
		int count = list.at(0)->childCount();
		for(int i=count-1; i>=0; i--){
			QTreeWidgetItem *item = list.at(0)->child(i);
			delete item;
		}
	}
}

void LastFmView::onItemClicked(QTreeWidgetItem *item, int column)
{
	if(!item->isSelected())
		return;
	ui->tableView->setLoading(true);
	if(item->parent()==0){
		//tagged, loved, chart
		qDebug() << item->text(column);
		if(item->text(column) == "Loved tracks"){
			m_mode = "loved";
			m_lastFmSearch.searchUserLovedTracks(m_user);
		}
		else if(item->text(column) == "Weekly chart"){
			m_mode = "chart";
			m_lastFmSearch.getweeklytrackchart(m_user);
			m_selectedTag = "";
		}
		else if(item->text(column) == "Recent tracks"){
			m_mode = "recent";
			m_lastFmSearch.getRecentTracks(m_user);
		}
	}
	else{
		if(item->parent()->text(0) == "Tagged tracks"){
			m_mode = "tag";
			TreeWidgetTag *tag = (TreeWidgetTag*)item;
			m_selectedTag = tag->tagName();
			m_lastFmSearch.searchUserTaggedTracks(m_user, m_selectedTag);

		}
		else if(item->parent()->text(0) == "Playlists"){
			m_mode = "playlist";
			TreeWidgetPlaylist *playlist = (TreeWidgetPlaylist*)item;
			//playlistID = playlist->playlistID();
			m_lastFmSearch.playlistFetch(playlist->playlistID());
		}
	}
}

void LastFmView::menuUnlove()
{
	int row = m_indexRightClicked.row();
	if(m_model.headerTitle(1)=="Title"){
		QString artist = m_model.record(row, 0);
		QString title = m_model.record(row, 1);
		LastFm::unloveTrack(artist, title);
		qDebug() << "last fm right click " << artist << " - " << title;
	}
}

void LastFmView::menuRemoveTag()
{
	int row = m_indexRightClicked.row();
	if(m_model.headerTitle(1)=="Title"){
		QString artist = m_model.record(row, 0);
		QString title = m_model.record(row, 1);
		LastFm::trackRemoveTag(artist, title, m_selectedTag);
		qDebug() << "last fm right click " << artist << " - " << title;
	}
}

void LastFmView::onRightClicked(QModelIndex index)
{
	if(ui->lineEdit->text()!=Settings::instance()->LASTFM_USERNAME || !LastFm::authorized())
		return;
	p_actionRemoveTag->setDisabled(true);
	p_actionUnlove->setDisabled(true);
	if(m_mode=="loved"){
		p_actionUnlove->setDisabled(false);
	}
	else if (m_mode == "tag"){
		p_actionRemoveTag->setDisabled(false);
		p_actionUnlove->setDisabled(true);
	}
	m_indexRightClicked = index;
	m_menu.exec(QCursor::pos());
}
