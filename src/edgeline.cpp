#include "edgeline.h"
#include "node.h"
#include "settings.h"
#include "qmath.h"
EdgeLine::EdgeLine(QGraphicsScene *scene)
{
//	qDebug() << "constructor";

	p_scene = scene;
	p_scene->addItem(this);
	m_activeColor = QColor(Settings::instance()->EDGELINE_ACTIVE_COLOR);
	m_inactiveColor = QColor(Settings::instance()->EDGELINE_NORMAL_COLOR);
	m_activeWidth = Settings::instance()->EDGELINE_ACTIVE_WIDTH;
	m_inactiveWidth = Settings::instance()->EDGELINE_NORMAL_WIDTH;
	setLinePlayNext(false);
	setZValue(1);
}

EdgeLine::~EdgeLine()
{
	p_scene->removeItem(this);
//	qDebug() << "DESTRUCTOR EdgeLine";
}

QRectF EdgeLine::boundingRect() const
{
	return QRectF(m_line.p1(), m_line.p2());
}

void EdgeLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	QPen pen;
//	pen.setCapStyle(Qt::RoundCap);
	if(m_active){
		pen.setWidth(m_activeWidth);
		pen.setColor(m_activeColor);
	}
	else{
		pen.setWidth(m_inactiveWidth);
		pen.setColor(m_inactiveColor);
	}
	pen.setJoinStyle(Qt::MiterJoin);
	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawLine(m_line);
	QPointF arrowPoint;
	qreal angle = m_line.angle();
//	qDebug() << "ANGLE = " << angle;
	angle *= 3.14/180;
//	qDebug() << "y before : " << m_line.p2().y()  << "after: " <<m_line.p2().y()- p_dest->diameter()/2*qSin(angle);

	// in y i add, cuz in qt coordinate system when you go down, y gets bigger
//	arrowPoint.setX(m_line.p2().x());
//	arrowPoint.setY(m_line.p2().y());
//	arrowPoint += QPointF(5,5);
	arrowPoint.setX(m_line.p2().x() + 5*qCos(angle));
	arrowPoint.setY(m_line.p2().y() - 5*qSin(angle));

	QPointF arrowEdgeLeft;
	arrowEdgeLeft.setX(arrowPoint.x() - 15*qCos(angle-0.5));
	arrowEdgeLeft.setY(arrowPoint.y() + 15*qSin(angle-0.5));

	QPointF arrowEdgeRight;
	arrowEdgeRight.setX(arrowPoint.x() - 15*qCos(angle+0.5));
	arrowEdgeRight.setY(arrowPoint.y() + 15*qSin(angle+0.5));

	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	if(m_active){
		brush.setColor(m_activeColor);
	}
	else
		brush.setColor(m_inactiveColor);
	painter->setBrush(brush);

	QPolygonF polygon;
	polygon << arrowPoint << arrowEdgeLeft << arrowEdgeRight;
	painter->drawPolygon(polygon);

	//debuggin drawing
//	pen.setStyle(Qt::DotLine);
//	pen.setWidth(1);
//	painter->setPen(pen);
//	painter->drawRect(boundingRect());
}

void EdgeLine::setEndings(Node *source, Node *dest)
{
	p_source = source;
	m_line.setP1(QPointF(p_source->x()+p_source->diameter()/2,p_source->y()+p_source->diameter()/2) );
	p_dest = dest;
	m_line.setP2( QPointF(p_dest->x()+p_dest->diameter()/2,p_dest->y()+p_dest->diameter()/2) );
	update();
}

void EdgeLine::setLinePlayNext(bool active)
{
	if(active)
		setZValue(5);
	else
		setZValue(1);
	m_active = active;
	update();
	scene()->update(boundingRect());
}

void EdgeLine::updatePoints()
{
	m_line.setP1(QPointF(p_source->x()+p_source->diameter()/2,p_source->y()+p_source->diameter()/2) );
	m_line.setP2( QPointF(p_dest->x()+p_dest->diameter()/2,p_dest->y()+p_dest->diameter()/2) );

	//OFFSET for the lines to begin after the circle outline
	//useful if circle's inside is somewhat transaparent
	qreal angle = m_line.angle();
//	qDebug() << "ANGLE = " << angle;
	angle *= 3.14/180;

	QPointF offset = QPointF( (p_dest->diameter()+15)/2*m_line.dx()/m_line.length(), (p_dest->diameter()+15)/2*m_line.dy()/m_line.length());
//	offset += QPointF(5,5);
//	m_line.setP1(m_line.p1() + offset);
	m_line.setP2(m_line.p2() - offset);

//	qDebug() << "line length: " << m_line.length();

	update();
}
