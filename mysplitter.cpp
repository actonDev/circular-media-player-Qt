#include "mysplitter.h"
#include <QLayout>
#include <QTabWidget>
#include <QDebug>
MySplitter::MySplitter(QWidget *parent) :
	QSplitter(parent)
{
}

void MySplitter::setOrientation(Qt::Orientation orientation)
{
//	qDebug() << "SET ORIENTATION!!!!!!!!!!!!!!!!!!!!!!";
	QSplitter::setOrientation(orientation);
	emit orientationChanged(orientation);
}

QSplitterHandle *MySplitter::createHandle()
{
	MyHandle *handle = new MyHandle(orientation(), this );
	return handle;
}


MyHandle::MyHandle(Qt::Orientation orientation, QSplitter *parent) : QSplitterHandle( orientation, parent )
{
}

MyHandle::~MyHandle()
{
}

void MyHandle::mouseDoubleClickEvent(QMouseEvent *event)
{
	MySplitter *mySplitter = (MySplitter*)splitter();
	if(mySplitter->orientation() == Qt::Horizontal){
		mySplitter->setOrientation(Qt::Vertical);
	}
	else{
		mySplitter->setOrientation(Qt::Horizontal);
	}
}



