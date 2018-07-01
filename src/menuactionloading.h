#ifndef MENUACTIONLOADING_H
#define MENUACTIONLOADING_H

#include <QMenu>
#include <QTimer>

class MenuActionLoading : public QMenu
{
	Q_OBJECT
private:
	bool m_loading;
	QString m_title;
	QString m_textLoading;
	QTimer m_timer;

private slots:
	void updateSlot();
public:
	explicit MenuActionLoading(QWidget *parent = 0);
	void setLoading(bool loading);
	void setTitle(const QString &title);
	
signals:
	
public slots:
	
};

#endif // MENUACTIONLOADING_H
