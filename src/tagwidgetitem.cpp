#include "tagwidgetitem.h"

TagWidgetItem::TagWidgetItem(QString tag, QString count, QListWidget *view) :
	QListWidgetItem(view)
{
	m_tag = tag;
	setText(tag + " (" + count +")");
}
