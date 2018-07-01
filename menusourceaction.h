#ifndef MENUSOURCEACTION_H
#define MENUSOURCEACTION_H

#include <QAction>
#include <QDebug>

class MenuSourceAction : public QAction
{
	Q_OBJECT
private:
	QString m_description;
	QString m_source;
public:
	explicit MenuSourceAction(QString descr, QString source, QObject *parent = 0);
	~MenuSourceAction();
	QString source(){
		return m_source;
	}
	
signals:
	void sourceSelected(QString);
private slots:
	void onTriggered(){
		qDebug() << "soure action: " <<  source();
		emit sourceSelected( source());
	}

public slots:
	
};

#endif // MENUSOURCEACTION_H
