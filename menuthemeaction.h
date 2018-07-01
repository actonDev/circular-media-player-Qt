#ifndef MENUTHEMEACTION_H
#define MENUTHEMEACTION_H

#include <QAction>

class MenuThemeAction : public QAction
{
	Q_OBJECT
public:
	explicit MenuThemeAction(QObject *parent = 0);
	
signals:

private slots:
	void on_trigger();
	
public slots:
	
};

#endif // MENUTHEMEACTION_H
