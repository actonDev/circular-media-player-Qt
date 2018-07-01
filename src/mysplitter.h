#ifndef MYSPLITTER_H
#define MYSPLITTER_H

#include <QSplitter>
#include <QSplitterHandle>

class MySplitter : public QSplitter
{
	Q_OBJECT
	Q_PROPERTY(QString class READ cssClass)

public:
	explicit MySplitter(QWidget *parent = 0);
	void setOrientation(Qt::Orientation orientation);
	QString cssClass() { return QString("mySplitter"); }
signals:
	void orientationChanged(Qt::Orientation);
	
public slots:
protected:
	virtual QSplitterHandle* createHandle();
	
};

class MyHandle : public QSplitterHandle
{
	Q_OBJECT
public:
	MyHandle( Qt::Orientation orientation, QSplitter *parent = 0 );
	~MyHandle();
protected:
	virtual void mouseDoubleClickEvent( QMouseEvent * event );
signals:
//	void handleDoubleClicked( int index );
};

#endif // MYSPLITTER_H
