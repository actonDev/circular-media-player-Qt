#include "treewidgetplaylist.h"

TreeWidgetPlaylist::TreeWidgetPlaylist(QTreeWidgetItem *parent, QString id, QString name, QString count, int type) : QTreeWidgetItem(parent, type)
{
	m_name = name;
	m_id = id;
	m_count = count;
	setText(0, name + "("+count+")");
}

QString TreeWidgetPlaylist::playlistID()
{
	return m_id;
}
