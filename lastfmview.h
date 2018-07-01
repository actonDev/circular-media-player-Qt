#ifndef LASTFMVIEW_H
#define LASTFMVIEW_H

#include <QWidget>
#include "resultsmodel.h"
#include "lastfmsearch.h"
#include <QMenu>
#include <QTreeWidgetItem>

namespace Ui {
class LastFmView;
}

class LastFmView : public QWidget
{
	Q_OBJECT
private:
	QMenu m_menu;
	ResultsModel m_model;
	LastFmSearch m_lastFmSearch;
	QString m_user;
	QString m_selectedTag;
	QString m_mode;

	QModelIndex m_indexRightClicked;
	QAction *p_actionUnlove;
	QAction *p_actionRemoveTag;

private slots:
	void setResults(QList< QStringList > results);
	void setTopTags(QList<QPair<QString, QString> > results);
	void setPlaylists(QList<QStringList> results);
	void updateUser();
	void onDoubleClicked(QModelIndex index);
	void clearTags();
	void clearPlaylists();

	void onItemClicked(QTreeWidgetItem* item,int column);

	void menuUnlove();
	void menuRemoveTag();

	void onRightClicked(QModelIndex index);
public:
	explicit LastFmView(QWidget *parent = 0);
	~LastFmView();
	
private:
	Ui::LastFmView *ui;

signals:
	void doubleCliked(QString artist, QString title, QString source);
};

#endif // LASTFMVIEW_H
