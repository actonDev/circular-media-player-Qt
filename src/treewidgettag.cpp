#include "treewidgettag.h"

TreeWidgetTag::TreeWidgetTag(QTreeWidgetItem *parent, QString tag, QString count, int type) : QTreeWidgetItem(parent, type)
{
	m_tag = tag;
	m_count = count;
	setText(0, tag + " ("+count+")");
}

QString TreeWidgetTag::tagName()
{
	return m_tag;
}
