#ifndef TREEWIDGETPLAYLIST_H
#define TREEWIDGETPLAYLIST_H

#include <QTreeWidgetItem>

class TreeWidgetPlaylist : public QTreeWidgetItem
{
private:
	QString m_name;
	QString m_id;
	QString m_count;
public:
	TreeWidgetPlaylist(QTreeWidgetItem *parent, QString id, QString name, QString count, int type = 1001);
	QString playlistID();
};

#endif // TREEWIDGETPLAYLIST_H
