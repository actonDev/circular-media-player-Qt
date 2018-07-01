#include "node.h"
#include "graph.h"
#include "qmath.h"
#include <typeinfo>

#include <QToolTip>
#include <QDrag>
#include <QGraphicsProxyWidget>

#define FOV 90
#define MAXSTRING 20

#include "graph.h"
#include "player.h"
#include "settings.h"
#include "Tools.h"
#include "lastfmrelated.h"
#include "myview.h"
#include "downloader.h"


const double PI = 3.141592653589793238462;

Node *Node::p_playingNode = 0;
Graph *Node::p_graph = 0;
QGraphicsScene *Node::p_scene = 0;

Node::Node(QString artist, QString title, QString source)
{
	p_parent = 0;
	p_backwardsEdgeLine = 0;
	m_pressed = false;
	m_dragged = false;
	m_hovered = false;
	m_dropTarget = false;
	m_diameter = 100;
	m_offsetAngle = 0;
	m_playing = false;
	m_playNext = false;
	m_waitToPlay = false;
	m_animating = false;
	m_clickReleased = false;
//	p_graph->view()->zoo

	m_customOffsetAngle = false;

	m_textColor = QColor(Settings::instance()->CIRCLE_TEXT_COLOR);
	m_textColorHovered = QColor(Settings::instance()->CIRCLE_HOVERED_TEXT_COLOR);

	m_animationAppear.setTargetObject(this);
	m_animationAppear.setPropertyName("opacity");
	m_animationAppear.setDuration(2000);
	m_animationAppear.setStartValue(0.1);
	m_animationAppear.setEndValue(1);

	m_animationShrink.setTargetObject(this);
	m_animationShrink.setPropertyName("scale");
	m_animationShrink.setDuration(300);
	m_animationShrink.setStartValue(1.5);
	m_animationShrink.setEndValue(1);

	m_animationGrow.setTargetObject(this);
	m_animationGrow.setPropertyName("scale");
	m_animationGrow.setDuration(300);
	m_animationGrow.setStartValue(1);
	m_animationGrow.setEndValue(1.5);

	m_animationX.setTargetObject(this);
	m_animationX.setPropertyName("x");

	m_animationY.setTargetObject(this);
	m_animationY.setPropertyName("y");

	connect(&m_animationX, SIGNAL(finished()), this, SLOT(onAnimationMoveEnd()));
	connect(&m_animationY, SIGNAL(finished()), this, SLOT(onAnimationMoveEnd()));

	connect(&m_animationGrow, SIGNAL(finished()), this, SLOT(animationGrowEnd()));
	connect(&m_animationShrink, SIGNAL(finished()), this, SLOT(updateSlot()));

	setFlags(ItemIsSelectable | ItemIsMovable);
	setAcceptHoverEvents(true);
	setZValue(10);
	setTransformOriginPoint(boundingRect().center());

	m_artist = artist;
	m_title = title;
	m_album = "Album";
	m_albumNumber = 0;
	setSource(source);
	p_scene->addItem(this);

	m_svgLocal.load(Settings::instance()->themeFolder() + "/circle/local.svg");
	m_svgRemote.load(Settings::instance()->themeFolder() + "/circle/remote.svg");
	m_svgPlaying.load(Settings::instance()->themeFolder() + "/circle/playing.svg");
	m_svgDropTarget.load(Settings::instance()->themeFolder() + "/circle/droptarget.svg");
	m_svgUnavailable.load(Settings::instance()->themeFolder() + "/circle/unavailable.svg");
	m_svgHovered.load(Settings::instance()->themeFolder() + "/circle/hovered.svg");

	m_svgOutlineNormal.load(Settings::instance()->themeFolder() + "/circle/outline_normal.svg");
	m_svgOutlineNext.load(Settings::instance()->themeFolder() + "/circle/outline_next.svg");
	m_svgOutlinePlaying.load(Settings::instance()->themeFolder() + "/circle/outline_playing.svg");

	m_animationAppear.start();

	m_clickReleaseTimer.setSingleShot(true);
	m_clickReleaseTimer.setInterval(qApp->doubleClickInterval());
	connect(&m_clickReleaseTimer, SIGNAL(timeout()), this, SLOT(onClickRelease()));
}

Node::~Node()
{
	//	if(p_backwardsEdgeLine!=0){
	//		delete p_backwardsEdgeLine;
	//		p_backwardsEdgeLine = 0;
	//	}
	if(p_playingNode == this)
		p_playingNode = 0;
//	qDebug() << "NODE DESTRUCTOR " << artist() << " - " << title();

	p_graph->emitNodeDeletion(this);
}

void Node::setGraph(Graph *graph)
{
	p_graph = graph;
	p_scene = graph->getScene();
}

Node *Node::getGlobalParent()
{
	if(p_parent!=0)
		return p_parent->getGlobalParent();
	else
		return this;
}

void Node::clearParent()
{
	if(p_parent==0)
		return;
	p_parent->removeReferenceTo(this);
	p_parent = 0;
	if(p_backwardsEdgeLine!= 0){
		delete p_backwardsEdgeLine;
		p_backwardsEdgeLine = 0;
	}
	setPlayNext(false);
}

void Node::setPlaying(bool playing, bool keepNext)
{
	if(m_playing==playing)
		return;
	m_playing = playing;
	//if setting to NOT play, clearing next (playlist)
	if(!playing){
		setZValue(zValue()-2);
		if(!keepNext)
			clearNext();
	}
	else{
		setPlayNext(false);
		setZValue(zValue()+2);
		m_animating = true;
		m_animationGrow.start();
	}
	update();
}

void Node::setPlayNext(bool playNext, bool toggle, bool ensureVisible)
{
	//if setting to NOT change state ok...
	if(m_playNext && playNext && toggle){ //if setting to play next while already playing, toggle
		qDebug() << "toggle play next";
		setPlayNext(false);
		p_backwardsEdgeLine->setLinePlayNext(false);
		return;
	}
	if(m_playNext == playNext){
		return;
	}
	if(!playNext){
		setZValue(zValue()-1);
		m_playNext = false;
		if(!m_playing) clearNext(); //clear the children, if setting not next!
		if(p_backwardsEdgeLine!=0)
			p_backwardsEdgeLine->setLinePlayNext(false);
		update();
		return;
	}
	else if(p_parent!=0){
		if(p_parent->playing() || p_parent->toPlayNext()){
			p_parent->clearNext();
			m_playNext = true;
			setZValue(zValue()+1);
			p_backwardsEdgeLine->setLinePlayNext(true);
			qDebug() << "play next " << title();
			update();

			if(p_parent->playing() && playNext && Player::isInErrorState()) // if phonon error!
				setAsPlayingNode();
			if(ensureVisible)
				p_graph->view()->ensureVisible(this);
		}
	}
}

void Node::clearNext()
{
	for(int i=0; i<m_forwardsRelatedList.length(); i++){
		m_forwardsRelatedList.at(i)->setPlayNext(false);
	}
}

Node *Node::next()
{
	for(int i=0; i<m_forwardsRelatedList.length(); i++){
		if( m_forwardsRelatedList.at(i)->toPlayNext() ){
			return m_forwardsRelatedList.at(i);
		}
	}
	return 0;
}

void Node::setAsPlayingNode(bool centerOn)
{
	qDebug() << "NODE setAsPlayingNode START";
	setPlaying(true);
	Node::setPlayingNode(this);
	if(m_source == ""){
		if(hasNext()) //for when having a queued list of tracks, and one in the middle has no source!
			next()->setAsPlayingNode();
		else{
			m_waitToPlay = true;
			return;
		}
	}

	//get related if children less than x... (settings)
	if(m_forwardsRelatedList.count()<Settings::instance()->GET_RELATED_IF_LESS_THAN  && !Settings::instance()->PLAYING_SHUFFLE_MODE)
		p_graph->lastFmRelated()->getRelated(this);
	if(Settings::instance()->SET_PLAYING_AS_ROOT)
		setAsRoot();

	if(centerOn)
		centerOnThis();
}

void Node::paintAsPlayingNode()
{
	m_playing = true;
	update();
	Node::setPlayingNode(this, false);
}

void Node::paintAsNext()
{
	m_playNext = true;
	qDebug() << "backwards line " << p_backwardsEdgeLine;
	if(p_backwardsEdgeLine!=0)
		p_backwardsEdgeLine->setLinePlayNext(true);
	update();
}

void Node::centerOnThis()
{
	p_graph->view()->fitInView(this);
	p_graph->view()->resetTransform();
	p_graph->view()->centerOn(pos().x()+diameter()/2, pos().y()+diameter()/2);
	qDebug() << "center on " << title();
}

void Node::searchForSource(bool onlineToo)
{
	// bool onlineToo: search online even if local source is found?
	p_graph->searchForSource(this, onlineToo);
}

bool Node::isChildOf(Node *node)
{
	while(p_parent!=0){
		if(p_parent == node)
			return true;
		else
			return p_parent->isChildOf(node);
	}
	return false;
}

bool Node::hasRelated(QString artist, QString title)
{
	for(int i=0; i<m_forwardsRelatedList.count(); i++){
		if(m_forwardsRelatedList.at(i)->artist() == artist &&
		   m_forwardsRelatedList.at(i)->title() == title)
			return true;
	}
	return false;
}

void Node::updateOffsetAngle()
{
	if(!m_customOffsetAngle){
		if(p_backwardsEdgeLine!=0)
			m_offsetAngle = (p_backwardsEdgeLine->getLine()->angle()) +FOV/2;
		else
			m_offsetAngle = 0;
	}
//	else{
//		if(p_backwardsEdgeLine!=0)
//			m_offsetAngle = m_offsetAngle + (p_backwardsEdgeLine->getLine()->angle());
//	}
}

qreal Node::backwardsAngle()
{
	qDebug() << "custom offset angle: " << m_customOffsetAngle;
	if(!m_customOffsetAngle){
		if(p_backwardsEdgeLine){
			return (p_backwardsEdgeLine->getLine()->angle());
		}
		else
			return 0;
	}
	else{
		return Tools::angleToRational(m_offsetAngle);
	}
	//	else
	//		return 0;
}

qreal Node::backwardsAngleOffset()
{
	if(p_backwardsEdgeLine){
		qreal angle;
		angle = Tools::angleToRational(p_parent->backwardsAngle()) - Tools::angleToRational(p_backwardsEdgeLine->getLine()->angle());

		if(p_parent->getParent() == 0 )
			angle = Tools::angleToPositive(angle);
		else
			angle = Tools::angleToRational(angle);
		return angle;
	}
	else
		return 0;
}

void Node::moveToCenter()
{
	QPointF point = p_graph->view()->mapToScene(p_graph->view()->viewport()->rect().center());
	setPos(point);
}

void Node::moveBy(QPointF dpoint)
{
	if(!m_animating){
		setPos(pos() + dpoint);
	}
	else{
		qreal x = m_animationX.endValue().toReal();
		m_animationX.stop();
		qreal y = m_animationY.endValue().toReal();
		m_animationY.stop();
		setX(x + dpoint.x());
		setY(y + dpoint.y());
		onAnimationMoveEnd();
	}
}

void Node::setAsRoot()
{
	Node *globalParent = getGlobalParent();
	if(globalParent==this){
		return;
	}
	clearParent();
	globalParent->deleteChildren(this);
	orderChildren();
	delete globalParent;
	scene()->update();
}


QRectF Node::boundingRect() const{
	int diam = diameter();
	return QRectF(-25,0,diam+50,diam);
}

QPainterPath Node::shape() const
{
	QPainterPath path;
	path.addEllipse(outterRect());
	return path;
}

QRectF Node::innerRect() const
{
	double side = sqrt(2.0)*diameter()/2;
	double offset = (diameter() - side)/2 + 2;
	return QRectF(offset,offset,side -4,side-4);
}

QRectF Node::outterRect() const {
	//return QRectF(0,0,m_diameter,m_diameter);
	int diam = diameter();
	return QRectF(0,0,diam,diam);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

	QPen pen;
	QFont font("Arial");
	font.setPixelSize(8);
	painter->setFont(font);
	pen.setColor(m_textColor);

	int factorTest;
	factorTest = innerRect().width() / painter->fontMetrics().width(m_artist);
	int pixelSizeArtist = font.pixelSize() * factorTest;
	if(pixelSizeArtist<12) pixelSizeArtist =12;
	if(pixelSizeArtist>30) pixelSizeArtist = 30;

	factorTest = innerRect().width() / painter->fontMetrics().width(m_title);
	int pixelSizeTitle = font.pixelSize() * factorTest;
	if(pixelSizeTitle<12) pixelSizeTitle =12;
	if(pixelSizeTitle>30) pixelSizeTitle = 30;

//	if(m_hovered && p_graph->view()->scaleFactor()<0.6){
//		qreal scale = 1/p_graph->view()->scaleFactor() *0.8;
//		pixelSizeArtist *= scale;
//		pixelSizeTitle *= scale;
//	}

	double backgroundOpacity = Settings::instance()->TEXT_BACKGROUND_OPACITY;
	if( (m_hovered || m_pressed) && backgroundOpacity>0 && !m_animating){
		font.setBold(true);
		painter->setOpacity(backgroundOpacity);
		// artist
		QString text = m_artist;
		int maxLengthHover = Settings::instance()->MAX_STRING_LENGTH_HOVER;
		if(text.count()>maxLengthHover){
			text.resize(maxLengthHover -2);
			text+="..";
		}
		font.setPixelSize(pixelSizeArtist);
		painter->setFont(font);
		QRectF rectTest = painter->fontMetrics().boundingRect(0,innerRect().y() , diameter(), diameter(),Qt::TextDontClip | Qt::AlignHCenter, text );
		QBrush brush(QColor(Settings::instance()->CIRCLE_HOVERED_TEXT_BACKGROUND_COLOR));
		rectTest.adjust(-2,0,2,2);
		painter->setBrush(brush);
		painter->setPen(Qt::NoPen);
		painter->drawRect(rectTest);
		painter->setPen(pen);

		QRectF rectTitleTextOffset = innerRect();
		rectTitleTextOffset.translate(0,painter->fontMetrics().height());

		// title
		text = m_title;
		if(text.count()>maxLengthHover){
			text.resize(maxLengthHover -2);
			text+="..";
		}
		font.setPixelSize(pixelSizeTitle);
		painter->setFont(font);

		rectTest = painter->fontMetrics().boundingRect(0,rectTitleTextOffset.y() , diameter(), diameter(),Qt::TextDontClip | Qt::AlignHCenter, text );
		rectTest.adjust(-2,0,2,2);
		brush.setColor(QColor(Settings::instance()->CIRCLE_HOVERED_TEXT_BACKGROUND_COLOR));
		painter->setBrush(brush);
		painter->setPen(Qt::NoPen);
		painter->drawRect(rectTest);
		painter->setPen(pen);

		painter->setOpacity(1);
		font.setBold(false);
	}

	QString source4 = m_source;
	source4.resize(4);
	if(m_dropTarget)
		m_svgDropTarget.render(painter,outterRect());
	else if(!hasSource())
		m_svgUnavailable.render(painter,outterRect());
	else{
		if(source4 == "http")
			m_svgRemote.render(painter,outterRect());
		else
			m_svgLocal.render(painter, outterRect());
	}

	if(m_playing)
		m_svgPlaying.render(painter,outterRect());

	//debuggin draw
	//	painter->drawRect(boundingRect());


	if(m_hovered || m_pressed){
		m_svgHovered.render(painter,outterRect());
		if(Settings::instance()->ZOOM_ON_HOVER)
			font.setBold(true);
		pen.setColor(m_textColorHovered);
	}

	if(m_playing)
		m_svgOutlinePlaying.render(painter, outterRect());
	else if(m_playNext)
		m_svgOutlineNext.render(painter, outterRect());
	else
		m_svgOutlineNormal.render(painter, outterRect());

	painter->setPen(pen);
	QRectF rectText = innerRect();

	//drawing text
	int maxLength = (!m_hovered || m_animating) ? (Settings::instance()->MAX_STRING_LENGTH) :  (Settings::instance()->MAX_STRING_LENGTH_HOVER);
	if(m_artist!=""){
		font.setPixelSize(pixelSizeArtist);
		painter->setFont(font);
		QString text = m_artist;

		if(text.count()>maxLength){
			text.resize(maxLength -2);
			text+="..";
		}
		painter->drawText(innerRect(), Qt::TextDontClip | Qt::AlignHCenter, text);
		rectText.translate(0,painter->fontMetrics().height());
	}
	if(m_title!=""){
		font.setPixelSize(pixelSizeTitle);
		painter->setFont(font);
		QString text = m_albumNumber>0 ? QString::number(m_albumNumber) + ". " + m_title : m_title;
		if(text.count()>maxLength){
			text.resize(maxLength -2);
			text+="..";
		}
		painter->drawText(rectText, Qt::TextDontClip | Qt::AlignHCenter, text );
	}
}

void Node::addRelated(Node *related, bool playNext, bool order)
{
	m_forwardsRelatedList.append(related);
	EdgeLine *edgeLine = new EdgeLine(p_scene);
	edgeLine->setEndings(this, related);
	updateOffsetAngle();
	related->addBackwardsRelated(this, edgeLine);
	if(order){
		related->setPos( pos() );
		orderChildren();
	}
	else
		updateLines();
	if(playNext)
		related->setPlayNext(true);
	scene()->update();
}

Node *Node::addRelated(QString artist, QString title, bool playNext)
{
	Node *node = p_graph->createNode(artist, title);
	addRelated(node,playNext);
	return node;
}

void Node::addBackwardsRelated(Node *backward, EdgeLine *edgeLine)
{
	p_parent = backward;
	p_backwardsEdgeLine = edgeLine;
	updateOffsetAngle();
}

int Node::diameter() const
{
	if(p_parent==0)
		return 100;
	else
		return 80;
}

bool Node::hasSource()
{
	if(m_source == "" || m_source == "null")
		return false;
	else
		return true;
}

QString Node::sourceListAt(int i)
{
	if(m_sourcesList.count() < i+1)
		return QString();
	QString source = m_sourcesList.at(i).second;
	return source;
}

QString Node::sourceTextAt(int i)
{
	if(m_sourcesList.count() < i+1)
		return QString();
	QString text = m_sourcesList.at(i).first;
	return text;
}

void Node::addSource(QString description, QString source)
{
	QPair<QString, QString> list;
	list.first = description;
	list.second = source;
	m_sourcesList.append(list);
}

void Node::orderChildren(int animationDuration)
{
	//	qDebug() << "Node::orderChildren count ";
	int distance = 120;
	int count = m_forwardsRelatedList.count();
	//	qDebug() << "Node::orderChildren count " << count;
	if(count==0)
		return;
//	updateOffsetAngle();

	for(int i=0; i<count; i++){
		qreal angle;
		if(p_parent == 0){
			angle = 360/count *i - offsetAngle();
		}
		else if(count ==1)
			angle = 45 - offsetAngle();
		else
			angle = FOV/(count-1) *i - offsetAngle();

		//		qDebug() << "order: " << i << ": angle " << angle;

		if(count>2){
			int polygonN = 360/FOV * (count - 1);
			if(p_parent!=0)
				distance =  Tools::polygonRadius(polygonN, 85, 120);
			else
				distance = Tools::polygonRadius(count, 85, 120);
		}
		else
			distance = 120;

		// to convert from degrees to radians: multiply with (pi/180)
		Node *nodeChild = m_forwardsRelatedList.at(i);
		angle *= PI/180;
		qreal newX = x() + diameter()/2 + distance*qCos( angle ) - nodeChild->diameter()/2;
		qreal newY = y() + diameter()/2 + distance*qSin( angle ) - nodeChild->diameter()/2;

		nodeChild->moveAnimated(newX, newY, animationDuration);
	}
	updateLines();
}

void Node::moveChildren(QPointF dpoint, qreal dangle)
{
	//	qDebug() << "moveChildren";
	int count = m_forwardsRelatedList.count();
	for(int i=0; i<count; i++){
		Node *nodeChild = m_forwardsRelatedList.at(i);

		if(p_parent == 0 || dangle == 0){
			nodeChild->moveBy(dpoint);
			nodeChild->moveChildren(dpoint, dangle);
		}
		else{
			//rotating item
			QPointF posBefore = nodeChild->pos();
			QPointF rotatedPoint = Tools::rotate(nodeChild->pos(), pos(), dangle);
			nodeChild->setPos(rotatedPoint + dpoint);
			QPointF posAfter = nodeChild->pos();
			QPointF dpoint2 = posAfter - posBefore;
			nodeChild->moveChildren(dpoint2, dangle);
		}
		updateLines();
	}
}

void Node::reorderChildren()
{
	m_forwardsRelatedList = Tools::orderList(m_forwardsRelatedList, std::mem_fun(&Node::backwardsAngleOffset));
	orderChildren();
}

void Node::updateLines()
{
	for(int i=0; i<m_forwardsRelatedList.count(); i++){
		Node *child = m_forwardsRelatedList.at(i);
		child->backwardsEdgeLine()->updatePoints();
	}
	if(p_backwardsEdgeLine != 0)
		p_backwardsEdgeLine->updatePoints();
}

void Node::hideLines()
{
	for(int i=0; i<m_forwardsRelatedList.count(); i++){
		m_forwardsRelatedList.at(i)->backwardsEdgeLine()->hide();
	}
	if(p_backwardsEdgeLine != 0)
		p_backwardsEdgeLine->hide();
}

void Node::showLines()
{
	for(int i=0; i<m_forwardsRelatedList.count(); i++){
		m_forwardsRelatedList.at(i)->backwardsEdgeLine()->show();
	}
	if(p_backwardsEdgeLine != 0)
		p_backwardsEdgeLine->show();
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "NODE MOUSE PRESS";
	QGraphicsItem::mousePressEvent(event);
	if(!m_clickReleaseTimer.isActive())
		m_clickReleaseTimer.start();
	else
		m_clickReleaseTimer.stop();

	//###
	//fix for: escaping the menu by clicking eslewhere, causes the node to be "pressed" thus queued
	//so: if already pressed set pressed flag to false (pressed flag get false only in release, except for here)
	if(m_pressed){
		m_pressed = false;
		update();
		return;
	}
	//!###

	m_pressed = true;
	update();
	if(event->button() == Qt::LeftButton){
		//		if(event->modifiers() == Qt::ControlModifier)
	}
	else if(event->button() == Qt::RightButton){
		p_graph->setClickedNode(this);
		if(p_graph->showMenu(this)!=0){
			//menu ran action
		}
		else{
			//menu escaped
		}
		m_pressed = false;
		if(m_hovered)
			setHovered(false);
	}
	else if(event->button() == Qt::MiddleButton){
		if(event->modifiers() == Qt::NoModifier){
			deleteThis();
			return;
		}
		else if(event->modifiers() == Qt::ShiftModifier)
			deleteChildren();
		else if(event->modifiers() == Qt::ControlModifier)
			deleteSiblings();
	}
	qDebug() << "NODE MOUSE PRESS end";
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "NODE MOUSE RELEASE";
	QGraphicsItem::mouseReleaseEvent(event);

	if(!m_dragged && event->button()==Qt::LeftButton && m_pressed){ //mouse release with no mouse move
		if(event->modifiers() == Qt::ShiftModifier)
			enqueueTillThis();
		else if(event->modifiers() == Qt::NoModifier)
			setPlayNext(true);
		else if (event->modifiers() == Qt::AltModifier){ //toggle custom angle offst
			m_customOffsetAngle = false;
			orderChildren(300);
		}
	}

	// do a scene update cuz if a node is dragged quickly,
	//	on releasing the edge lines are weird (left over from previous move)..
	scene()->update();
	if(event->modifiers() == Qt::NoModifier){
		if(m_dragged && p_parent!=0 && Settings::instance()->REORDER_ON_MOUSE_RELEASE){
			if( !(p_parent->getParent() == 0 && p_parent->getChildrenList()->count()<=2) ) // dont reorder if moving a root's child and root has just 1 or 2 children
				p_parent->reorderChildren();
			else
				orderChildren(0);
		}
	}

	m_dragged = false;
	m_pressed = false;
	updateLines();
	update();
}

void Node::onClickRelease()
{
	//	m_clickReleased = true;
	//	if(qApp->mouseButtons() != Qt::NoButton)
	//		return;

	//	if(qApp->keyboardModifiers() == Qt::ShiftModifier)
	//		enqueueTillThis();
	//	else
	//		setPlayNext(true);

	//	updateLines();
	//	update();
	//	scene()->update();

	qDebug() << "### NODE TIMER CLICK RELEASE";
}

void Node::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//	qDebug() << "hover enter";
	setHovered(true);
//	scene()->update();
}

void Node::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	if(!m_pressed){
		setHovered(false);
	}
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF diffPoint = event->scenePos()-event->lastScenePos();
	//	qDebug() << "MOUSE MOVE";
	if(event->modifiers() == Qt::NoModifier){
		m_dragged = true;
		updateLines();
//		updateOffsetAngle();
		qreal dangle = 0;
		if(p_parent!=0){
			QLineF prevLine(p_parent->pos(), event->lastScenePos());
			QLineF newLine(p_parent->pos(), event->scenePos());
			qreal angle = prevLine.angleTo(newLine);
			dangle = angle;
		}
		moveChildren(diffPoint, dangle);
		scene()->update();
		QGraphicsItem::mouseMoveEvent(event);
	}
	else{
		m_pressed = false;
		if(event->modifiers() == Qt::ShiftModifier){
			QDrag *drag = new QDrag( (QWidget*) this );
			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/x-cmp", "true");
			mimeData->setData("artist", m_artist.toLocal8Bit());
			mimeData->setData("title", m_title.toLocal8Bit());
			mimeData->setData("source", m_source.toLocal8Bit());
			mimeData->setData("action", "move");
			QString address = QString::number( (long)this, 16);
			mimeData->setData("dragged", address.toLocal8Bit());
			drag->setMimeData(mimeData);
			drag->exec();
		}
		else if(event->modifiers() == Qt::ControlModifier){
			QDrag *drag = new QDrag( (QWidget*) this );
			QMimeData *mimeData = new QMimeData;
			mimeData->setData("application/x-cmp", "true");
			mimeData->setData("artist", m_artist.toLocal8Bit());
			mimeData->setData("title", m_title.toLocal8Bit());
			mimeData->setData("source", m_source.toLocal8Bit());
			mimeData->setData("action", "create");
			drag->setMimeData(mimeData);
			drag->exec();
			m_pressed = false;
		}
		else if(event->modifiers() == Qt::AltModifier){
			m_dragged = true;
			if(!m_customOffsetAngle){
				m_offsetAngle = 0;
				m_customOffsetAngle = true;
			}
			if(m_customOffsetAngle){
				if(diffPoint.y()>0)
					m_offsetAngle--;
				else
					m_offsetAngle++;

				orderChildren(0);
	//			updateLines();
				scene()->update();
			}
		}

	}

}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	//	m_clickReleaseTimer.stop();
	//	m_clickReleaseTimer.
	if(event->modifiers() == Qt::NoModifier){
		qDebug() << "NODE MOUSE DOUBLE CLICK";
		QGraphicsItem::mouseDoubleClickEvent(event);
		if(hasSource()){
			setAsPlayingNode();
		}
	}
	else if(event->modifiers() == Qt::ShiftModifier)
		setAsRoot();
}

void Node::setArtist(QString artist)
{
	m_artist = artist;
}

void Node::setTitle(QString title)
{
	m_title = title;
}

void Node::setSource(QString source)
{
	m_source = source;
	qDebug() << "Node::setSource " << source;
	update();

	//adding the source into the sourceList, if no source is previously set
	if(m_sourcesList.isEmpty() && hasSource()){
		QPair<QString, QString> pair;
		pair.first = "";
		pair.second = source;
		m_sourcesList.append(pair);
	}

	//behavior if selecting a source from the menu, and node is already playing -> play new source
	if(m_playing && hasSource()){
		qDebug() << "Node:: setting source while m_playing";
		setAsPlayingNode();
		update();
		return;
	}
	if(m_waitToPlay && hasSource()){
		m_waitToPlay = false;
		setAsPlayingNode();
	}
	update();
}

void Node::deleteChildren(Node *excl)
{
	for(int i=m_forwardsRelatedList.length()-1; i>=0; i--){
		Node *childNode = m_forwardsRelatedList.at(i);
		if(childNode == excl){
			continue;
		}
		childNode->deleteChildren(excl);

		//childNode->clearParent();
		//delete childNode;

		//OR

		m_forwardsRelatedList.takeAt(i);
		delete childNode->backwardsEdgeLine();
		delete childNode;
	}
	if(excl!=0)
		reorderChildren();
	scene()->update();
}

void Node::deleteSiblings()
{
	if(p_parent!=0)
		p_parent->deleteChildren(this);
}

void Node::deleteThis()
{
	setHovered(false);
	deleteChildren(0);
	clearParent();
	p_scene->removeItem(this);
	delete this;
}

void Node::removeReferenceTo(Node *node)
{
	for(int i=0; i<m_forwardsRelatedList.length(); i++){
		if(m_forwardsRelatedList.at(i) == node){
			m_forwardsRelatedList.takeAt(i);
		}
	}
	orderChildren();
	scene()->update();
}

void Node::onRelatedFinished()
{
	if(hasNext())
		return;
	qDebug() << "now to random select next";
	QList<int> canditates;
	for(int i=0; i<m_forwardsRelatedList.length(); i++){
		if(m_forwardsRelatedList.at(i)->source()!=""){
			canditates.append(i);
		}
	}
	if(!canditates.isEmpty()){
		int selection = Tools::randInt(0, canditates.length()-1);
		selection = canditates.at(selection);
		Node *node = m_forwardsRelatedList.at(selection);
		node->setPlayNext(true, false, true);
	}
	scene()->update();
}

bool Node::hasNext()
{
	for(int i=0; i<m_forwardsRelatedList.count(); i++){
		if(m_forwardsRelatedList.at(i)->toPlayNext())
			return true;
	}
	return false;
}

void Node::updateSlot()
{
	m_animating = false;
	scene()->update();
}

void Node::onAnimationMoveEnd()
{
	m_animating = false;
	updateLines();
	showLines();
	orderChildren(m_animationX.duration()*0.6);
	scene()->update();
}

void Node::setPlayingNode(Node *node, bool play)
{
	qDebug() << "Node::setPlayingNode";
	if(p_playingNode != 0 && p_playingNode != node){
		if(p_playingNode->getParent() == node){ //if setting node's parent to play, keep node's next and enqueue node
			p_playingNode->setPlaying(false, true);
			p_playingNode->setPlayNext(true);
		}
		else
			p_playingNode->setPlaying(false);
	}
	p_playingNode = node;
	if(play && node->hasSource())
		p_graph->updatePlayingNode();
}

qreal Node::offsetAngle()
{
	if(!m_customOffsetAngle){
		if(p_backwardsEdgeLine!=0)
			return (p_backwardsEdgeLine->getLine()->angle()) +FOV/2;
		else
			return 0;
	}
	else{
		if(p_backwardsEdgeLine!=0)
			return (p_backwardsEdgeLine->getLine()->angle()) + m_offsetAngle;
		else
			return m_offsetAngle;
	}
}

void Node::setHovered(bool hovered)
{
	if(hovered){
		if(Settings::instance()->ZOOM_ON_HOVER){
			setScale( (qreal)Settings::instance()->ZOOM_FACTOR );
		}


		m_hovered = true;
		setZValue(zValue()+3);
		QString tip = "Artist: " + m_artist + "\nTitle: " + m_title /*+ "\nSource: " + m_source*/;
		if(m_foundArtist!=""){
			tip += "\nOnline search returned:";
			tip += "\n      Arist: " + m_foundArtist;
			tip += "\n      Title: " + m_foundTitle;
		}

		int maxLength = Settings::instance()->MAX_STRING_LENGTH_HOVER;

		if(Settings::instance()->TOOLTIP_ON_HOVER ||
				(Settings::instance()->TOOLTIP_WHEN_CLIPPED && (m_title.count()>maxLength ||
																m_artist.count()>maxLength)) ||
				p_graph->view()->scaleFactor()< Settings::instance()->TOOLTIP_VIEW_ZOOM_LESS_THAN)
			QToolTip::showText(QCursor::pos(), tip);

		scene()->update();
	}
	//unhover
	else{
//		qDebug() << "hover leave";
		setZValue(zValue()-3);
		m_hovered = false;
		setScale(1);
		QToolTip::hideText();
		scene()->update();
	}
}

void Node::clearAll()
{
	QList<Node*> rootList = ObjectManager::nodeRootList();
	for(int i=0; i<rootList.count(); i++){
		Node *node = rootList.at(i);
		node->deleteThis();
	}
}

void Node::enqueueTillThis()
{
	Node *parent = getParent();
	if(parent == 0)
		return;
	parent->enqueueTillThis();
	if(parent->playing() || parent->toPlayNext())
		setPlayNext(true, false);
	qDebug() << "enqueue till this:" << title();
}

void Node::moveAnimated(qreal newX, qreal newY, int duration)
{
	if(x() == newX && y() == newY){
		return;
	}

	if(duration == 0){
		setX(newX);
		setY(newY);
		updateLines();
		orderChildren(0);
		return;
	}

	m_animating = true;
	hideLines();

	m_animationX.setStartValue(x());
	m_animationX.setEndValue(newX);
	m_animationX.setDuration(duration);

	m_animationY.setStartValue(y());
	m_animationY.setEndValue(newY);
	m_animationY.setDuration(duration);

	m_animationX.start();
	m_animationY.start();
}
