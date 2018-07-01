#ifndef WIDGETLOADING_H
#define WIDGETLOADING_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class WidgetLoading : public QWidget
{
	Q_OBJECT
private:
	QLabel m_label;
	QString m_text;
	QTimer m_timer;
	bool m_loading;
public:
	explicit WidgetLoading(QWidget *parent = 0);
	void setLoading(bool loading);
	
signals:
	
private slots:
	void updateText();
public slots:
	
};

#endif // WIDGETLOADING_H
