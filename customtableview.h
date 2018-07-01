#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QTableView>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include "movieplayer.h"
#include <QtGui>
#include "settings.h"

class CustomTableView : public QTableView
{
	Q_OBJECT
private:
	bool m_loading;
	QMovie m_movie;
	QLabel m_label;
public:
	explicit CustomTableView(QWidget *parent = 0);
	void setLoading(bool loading);
	QModelIndexList getSelectedIndexes(){ return selectedIndexes();}

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *e);
	
signals:
	void rightClicked(QModelIndex);
public slots:

private slots:
	void onFrameChange(int frame);
	void sort(int id);
	
};

#endif // CUSTOMTABLEVIEW_H
