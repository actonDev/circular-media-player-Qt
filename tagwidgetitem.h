#ifndef TAGWIDGETITEM_H
#define TAGWIDGETITEM_H

#include <QListWidgetItem>

class TagWidgetItem : public QListWidgetItem
{
private:
	QString m_tag;
public:
	explicit TagWidgetItem(QString tag, QString count = "0" ,QListWidget *view = 0);
	QString tag(){ return m_tag; }
signals:
	
public slots:
	
};

#endif // TAGWIDGETITEM_H
