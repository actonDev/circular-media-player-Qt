#include "menutagaction.h"
#include <QDebug>
#include "internetsearch.h"
MenuTagAction::MenuTagAction(QString tag, QObject *parent) : QAction(parent)
{
	m_tag = tag;
	qDebug() << "TAG CONSTRUCTOR " << m_tag << " this: " << this;
	setText(tag);
	connect(this, SIGNAL(triggered()), this, SLOT(onTrigger()));
}

MenuTagAction::~MenuTagAction()
{
	qDebug() << "TAG DESTRUCTOR " << m_tag;
}

void MenuTagAction::onTrigger()
{
	qDebug() << "search for tag: " << m_tag;
	InternetSearch::instance()->setSearchTagTopTracks(m_tag);
}
