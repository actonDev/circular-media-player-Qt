#include "widgetloading.h"
#include "settings.h"
#include <QDebug>
#include <QVBoxLayout>

WidgetLoading::WidgetLoading(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(&m_label);
	setLayout(layout);

	m_label.setText("");
//	m_timer.start(500);
	m_loading = false;
//	m_label.show();

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateText()));
}

void WidgetLoading::setLoading(bool loading)
{
	if(loading){
		m_label.setText(".");
		m_timer.start(500);
	}
	else{
		m_label.setText("");
		m_timer.stop();
	}

	m_loading = loading;

}

void WidgetLoading::updateText()
{
	if(m_label.text() == ".")
		m_label.setText("..");
	else if(m_label.text() == "..")
		m_label.setText("...");
	else
		m_label.setText(".");
}
