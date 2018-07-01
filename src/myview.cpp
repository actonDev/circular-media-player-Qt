#include "myview.h"
#include <QWheelEvent>
#include "graph.h"
#include "typeinfo"
#include "node.h"
#include "settings.h"

#include "taglib.h"
#include "fileref.h"


MyView::MyView(QWidget *parent) : QGraphicsView(parent)
{
	setAcceptDrops(true);
	QImage image(Settings::instance()->themeFolder() + "/back.png");
	QBrush brush;
	brush.setTextureImage(image);
	//    brush.setStyle(Qt::SolidPattern);
	//    brush.setColor(Qt::gray);
	p_dropNode = 0;
	setBackgroundBrush(brush);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	viewport()->setCursor(Qt::ArrowCursor);
	QAction *actionNewNode = new QAction("New node", this);
	QAction *actionClearAll = new QAction("Clear all", this);

	connect(actionNewNode, SIGNAL(triggered()), this, SLOT(menuNewNode()));
	connect(actionClearAll, SIGNAL(triggered()), this, SLOT(menuClearAll()) );

	m_menu.addAction(actionNewNode);
	m_menu.addSeparator();
	m_menu.addAction(actionClearAll);

	//	setContentsMargins(0, 2, 0, 0);
}

void MyView::wheelEvent(QWheelEvent *event)
{
	if(event->delta() > 0) {
		zoomIn();
	}
	else {
		zoomOut();
	}
}

void MyView::mousePressEvent(QMouseEvent *event)
{
	QPointF atScene = mapToScene(event->pos());
	QGraphicsItem *item = scene()->itemAt( atScene, QTransform() );
	if(event->button() == Qt::RightButton){
		//if this leads to crash, do an if else(typeid(*item).name() != typeid(Node).name())
		//and remove the OR ||
		if(item == 0 || typeid(*item).name() != typeid(Node).name()){
			qDebug() << "VIEW press";
			m_menu.exec(QCursor::pos());
		}
	}
	QGraphicsView::mousePressEvent(event);
}

void MyView::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	viewport()->setCursor(Qt::ArrowCursor);
}

void MyView::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "key press! " << event->text() << ", key" << event->key();
	if(event->key() == Qt::Key_Left)
		emit kbPrevious();
	else if(event->key() == Qt::Key_Right)
		emit kbNext();
	else if(event->key() == Qt::Key_Space){
		emit kbPlayPause();
		qDebug() << "play/pause";
	}
	else if (event->text() == ","){
		qDebug() << "previous";
		emit kbPrevious();
	}
	else if (event->text() == "."){
		qDebug() << "next";
		emit kbNext();
	}
	else if(event->key() == 67){ // 'c'
		resetTransform();
		Node::playingNode()->centerOnThis();
	}
	//zooming
	else if(event->text() == "-")
		zoomOut();
	else if(event->text() == "=" || event->text() == "+")
		zoomIn();
	else if (event->text() == "/"){
		resetTransform();
	}
}

void MyView::dragLeaveEvent(QDragLeaveEvent *event)
{
	event->accept();
	qDebug() << "my view dragleave";
}

void MyView::dragEnterEvent(QDragEnterEvent *event)
{
	p_dropNode = 0;
	qDebug() << "drop: text " << event->mimeData()->text();
	if(!event->mimeData()->data("application/x-cmp").isNull()){
		event->accept();
		qDebug() << "ACCEPT!";
	}
#ifdef HAVE_TAGLIB
	else if(event->mimeData()->hasUrls()){
		event->accept();
	}
#endif
	else
		qDebug() << "DID NOT ACCEPT";
}

void MyView::dragMoveEvent(QDragMoveEvent *event)
{
	//########################
	//	painting the 'to be dropped to' node!
	//	qDebug() << "dragMoveEvent";
	QPointF atScene = mapToScene(event->pos());
	QGraphicsItem *item = scene()->itemAt( atScene, QTransform() );

	if(item!=0){
		if( typeid(*item).name() == typeid(Node).name() ){
			Node *node = (Node*) item;
			if(p_dropNode!=0 && p_dropNode!=node){
				p_dropNode->setAsDropTarget(false);
			}
			p_dropNode = node;
			p_dropNode->setAsDropTarget(true);
		}
		else if(p_dropNode!=0){
			p_dropNode->setAsDropTarget(false);
			p_dropNode = 0;
			item = 0;
		}
	}
	else{
		if(p_dropNode!=0){
			p_dropNode->setAsDropTarget(false);
			p_dropNode = 0;
		}
	}
}

void MyView::dropEvent(QDropEvent *event)
{

	QPointF point = event->pos() + QPoint(-50,-50);
	qDebug() << "event pos drop: " << point.x() << "," << point.y();

	point = mapToScene(point.toPoint());
#ifdef HAVE_TAGLIB
	if(event->mimeData()->hasUrls()){
		QStringList pathList;
		QList<QUrl> urlList = event->mimeData()->urls();
		qDebug() << "url count: " << urlList.count();
		// extract the local paths of the files

		Node *prevNode = p_dropNode;
		for (int i = 0; i < urlList.count(); i++)
		{
			QString source = urlList.at(i).toLocalFile();
			if( source.endsWith(".mp3") || source.endsWith(".wav") || source.endsWith(".flac") || source.endsWith(".ogg") || source.endsWith(".wma") ){
				TagLib::FileRef fileRef(TagLib::FileName(source.toLocal8Bit().constData()));
				if(!fileRef.file()->isValid())
					continue;
				QString artist = fileRef.tag()->artist().toCString();
				QString title = fileRef.tag()->title().toCString();
				if(artist=="")
					artist = "Unknown Artist";
				if(title=="")
					title = QFileInfo(source).fileName();
				Node *node = new Node(artist, title, source);
				if(prevNode!=0){
					prevNode->addRelated(node);
					if(!prevNode->hasNext())
						node->setPlayNext(true);
				}
				else
					node->setPos(point);
				prevNode = node;

				if(p_dropNode!=0){
					p_dropNode->setAsDropTarget(false);
				}
			}
			else if(source.endsWith(".cmpl")){
				//playlist
				Settings::instance()->loadPlaylist(source, false);
			}
		}
		return;
	}
#endif

	if(!event->mimeData()->data("count").isEmpty()){ //if selection from the table
		int count = event->mimeData()->data("count").toInt();
		QByteArray encodedData =  event->mimeData()->data("application/x-cmp-table-transfer");
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QList<QStringList> list;
		stream >> list;

		qDebug() << "list count: " << list.count();


		qDebug() << "drop count: " << count;
		Node *prevNode = p_dropNode;
		QStringList entry;
		for(int i=0; i<count; i++){
			//			pair = list.at(i);
			entry = list.at(i);
			QString artist = entry.at(0);
			//			QString artist = event->mimeData()->data("artist"+i);
			//			qDebug() << "artist"<< i << ": " << artist;

			QString title = entry.at(1);
			//			QString title = event->mimeData()->data("title"+i);
			//			qDebug() << "title"<<i <<": " << title;

			QString source = entry.at(2);

			Node *node = new Node(artist, title, source);
			if(source=="")
				node->searchForSource();
			if(prevNode!=0){
				prevNode->addRelated(node);
				if(!prevNode->hasNext())
					node->setPlayNext(true);
			}
			else
				node->setPos(point);
			prevNode = node;
		}
		qDebug() << "drop count " << count;
		if(p_dropNode!=0){
			p_dropNode->setAsDropTarget(false);
		}
		return;
	}


	qDebug() << "DROP_START";
	if(event->mimeData()->data("artist").isEmpty()){
		qDebug() << "NO DRAG INFO";
		return;
	}
	//	event->accept();

	QString artist = event->mimeData()->data("artist");
	QString title = event->mimeData()->data("title");
	QString source = event->mimeData()->data("source");


	//### CREATIONS AND CHECKS
	Node *created = 0;
	if(event->mimeData()->data("action") == "create"){
		created = p_graph->createNode(artist, title, source, point);
	}
	else if(event->mimeData()->data("action") == "move" ){
		created = reinterpret_cast<Node*>(event->mimeData()->data("dragged").toInt(0, 16));
		created->setPressed(false);
		if(p_dropNode!=0){
			//checking if: dropping over itself, over its parent, over a child of it
			if(p_dropNode == created || p_dropNode->isChildOf(created) || created->getParent() == p_dropNode){
				p_dropNode->setAsDropTarget(false);
				return;
			}
		}
		created->clearParent();
	}

	//### NOW DROPPING OVER HOVERED TARGET
	if(p_dropNode!=0){
		qDebug() << "mouse over " << p_dropNode->title();
		p_dropNode->addRelated(created);
		if(!p_dropNode->hasNext())
			created->setPlayNext(true);
		p_dropNode->setAsDropTarget(false);
	}
	else{ //if just moving item
		created->setPos(point);
	}
	created->orderChildren();
	qDebug() << "DROP_END";
	p_dropNode = 0;
}

void MyView::setBackgroundImage(QString imagePng)
{
	QImage image(imagePng);
	QBrush brush;
	brush.setTextureImage(image);
	setBackgroundBrush(brush);
}

qreal MyView::scaleFactor()
{
	return transform().m11();
}

void MyView::menuClearAll()
{
	qDebug() << "Clear all";
	Node::clearAll();
}

void MyView::menuNewNode()
{
	QPointF point = mapFromGlobal(QCursor::pos());
	point = mapToScene(point.toPoint());
	Graph::createEditNode(point);
}

void MyView::zoomIn()
{
	qreal factor = 1.2;
	scale(factor, factor);
}

void MyView::zoomOut()
{
	qreal factor = 1.2;
	factor = 1/factor;
	scale(factor, factor);
}
