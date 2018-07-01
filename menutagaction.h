#ifndef MENUTAGACTION_H
#define MENUTAGACTION_H

#include <QAction>

class MenuTagAction : public QAction
{
	Q_OBJECT
private:
	QString m_tag;
public:
	MenuTagAction(QString tag, QObject *parent = 0);
	~MenuTagAction();
private slots:
	void onTrigger();
};

#endif // MENUTAGACTION_H
