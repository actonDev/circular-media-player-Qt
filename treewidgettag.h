#ifndef TREEWIDGETTAG_H
#define TREEWIDGETTAG_H

#include <QTreeWidgetItem>

class TreeWidgetTag : public QTreeWidgetItem
{
private:
	QString m_tag;
	QString m_count;
public:
	TreeWidgetTag(QTreeWidgetItem *parent, QString tag, QString count, int type = 1001);
	QString tagName();
};

#endif // TREEWIDGETTAG_H
