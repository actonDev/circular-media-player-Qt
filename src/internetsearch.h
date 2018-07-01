#ifndef INTERNETSEARCH_H
#define INTERNETSEARCH_H

#include <QWidget>
#include "resultsmodel.h"
#include "vkontakte.h"
#include "lastfmsearch.h"
#include "widgetloading.h"


namespace Ui {
class SearchFormOnline;
}
class InternetSearch : public QWidget
{
	Q_OBJECT
private:
	Ui::SearchFormOnline *ui;
	WidgetLoading m_widgetLoading;
	ResultsModel m_model;
	LastFmSearch m_lastFmSearch;
	void searchForTrack();
	void searchForTag();
	void doSearch();
	QString m_searchMode;
	static InternetSearch *p_instance;
	void setSearchMode(QString mode);
private slots:
	void onSearchModeChange(QString mode);
public:
	explicit InternetSearch(QWidget *parent = 0);
	~InternetSearch();
	static InternetSearch* instance(){ return p_instance;}
	void setSearchArtist(QString artist);
	void setSearchArtistAlbums(QString artist);
	void setSearchAlbum(QString artist, QString album);
	void setSearchSimilar(QString artist, QString title);
	void setSearchTagTopTracks(QString tag);
	
signals:
	void doubleCliked(QString artist, QString title, QString source);
	void makeVisible();
	
public slots:
	void on_searchButton_clicked();
	void onDoubleClicked(QModelIndex index);
	void setResults(QList< QStringList > results);
	
};

#endif // INTERNETSEARCH_H
