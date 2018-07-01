#ifndef EDGELINE_H
#define EDGELINE_H

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsView>

class Node;


class EdgeLine : public QGraphicsItem
{

	QPointF m_point1;
	QPointF m_point2;
	QLineF m_line;
	QGraphicsScene *p_scene;
	Node *p_source;
	Node *p_dest;

	int m_activeWidth;
	int m_inactiveWidth;
	QColor m_activeColor;
	QColor m_inactiveColor;
	bool m_active;


public:
	EdgeLine(QGraphicsScene *scene);
	~EdgeLine();

	QRectF boundingRect() const;
	QLineF *getLine(){ return &m_line; }
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setEndings(Node *source, Node *dest);
	void setLinePlayNext(bool active);
	void updatePoints();
};

#endif // EDGELINE_H
