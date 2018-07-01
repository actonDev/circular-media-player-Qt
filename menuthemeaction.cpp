#include "menuthemeaction.h"
#include <QApplication>
#include <QDebug>
#include "settings.h"

MenuThemeAction::MenuThemeAction(QObject *parent) :
	QAction(parent)
{
	connect(this, SIGNAL(triggered()), this, SLOT(on_trigger()));
}

void MenuThemeAction::on_trigger()
{
//	QString theme = QApplication::applicationDirPath() + "/themes/";
//	theme += text();
//	qDebug() << "SELECTED THEME: " << theme;

	Settings::instance()->THEME = text();
	Settings::instance()->updateThemeFolder();
}
