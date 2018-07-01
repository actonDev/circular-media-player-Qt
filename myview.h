#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <QDebug>
#include <QMenu>

class Graph;
class Node;

class MyView : public QGraphicsView
{
	Q_OBJECT

private:
	Graph *p_graph;
	Node *p_dropNode;
	QMenu m_menu;
	void zoomIn();
	void zoomOut();
public:
	MyView(QWidget *parent = 0);

	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	void setGraph(Graph *graph){
		p_graph = graph;
	}
	void setBackgroundImage(QString imagePng);

	qreal scaleFactor();
private slots:
	void menuClearAll();
	void menuNewNode();
signals:
	void kbPlayPause();
	void kbPrevious();
	void kbNext();
};

#endif // MYVIEW_H
