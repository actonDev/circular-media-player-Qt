#include "menuactionloading.h"

MenuActionLoading::MenuActionLoading(QWidget *parent) :
	QMenu(parent)
{
	m_loading = false;
//	m_timer.setInterval(500);
//	m_timer.setSingleShot(false);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateSlot()));
}

void MenuActionLoading::setLoading(bool loading)
{
	if(loading){
		m_textLoading = ".";
		QMenu::setTitle(m_title + " " + m_textLoading);
		m_timer.start(500);
//		QMenu::setTitle(m_title + " ...");
	}
	else{
		m_textLoading = "";
		m_timer.stop();
		QMenu::setTitle(m_title);
	}

	m_loading = loading;
}

void MenuActionLoading::setTitle(const QString &title)
{
	QMenu::setTitle(title);
	m_title = title;
}

void MenuActionLoading::updateSlot()
{
	if(m_textLoading == ".")
		m_textLoading  = "..";
	else if(m_textLoading == "..")
		m_textLoading = "...";
	else
		m_textLoading = ".";

	QMenu::setTitle(m_title + " " + m_textLoading);
}
