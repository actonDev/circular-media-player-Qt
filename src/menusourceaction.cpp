#include "menusourceaction.h"

MenuSourceAction::MenuSourceAction(QString descr, QString source, QObject *parent) :
	QAction(parent)
{
	m_description = descr;
	m_source = source;
	if(source!=""){
//		QString http = source;
//		http.resize(4);
//		if(http == "http")
//			setText(m_description /*+ " (" +m_source +")"*/);
//		else
			setText(m_description + " (" +m_source +")");
	}
	else{
		setText(m_description);
		setEnabled(false);
	}
	connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));
//	qDebug() << "menuSourceAction CONSTRUCTOR";
}

MenuSourceAction::~MenuSourceAction()
{
//	qDebug() << "menuSourceAction DESTRUCTOR";
}
