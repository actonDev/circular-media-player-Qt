#ifndef NODE_H
#define NODE_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>
#include "edgeline.h"
#include <QString>
#include <QPainter>
#include "math.h"
#include <QApplication>
#include <QFileInfo>
#include <QtSvg/QSvgRenderer>
#include <QGraphicsWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsLinearLayout>

class Graph;

#include "objectmanager.h"
#include <QPropertyAnimation>
#include <QTimer>

class Node : public QGraphicsWidget, public ManagedObject
{
	Q_OBJECT

private:
	bool m_pressed;
	bool m_hovered;
	bool m_dragged;
	bool m_playing;
	bool m_playNext;
	bool m_waitToPlay;
	bool m_dropTarget;
	int m_diameter;
	bool m_animating;
	bool m_clickReleased;

	bool m_customOffsetAngle;

	qreal m_offsetAngle;
	QColor m_textColor;
	QColor m_textColorHovered;

	QSvgRenderer m_svgLocal;
	QSvgRenderer m_svgRemote;
	QSvgRenderer m_svgDropTarget;
	QSvgRenderer m_svgUnavailable;
	QSvgRenderer m_svgHovered;
	QSvgRenderer m_svgPlaying;

	QSvgRenderer m_svgOutlineNormal;
	QSvgRenderer m_svgOutlineNext;
	QSvgRenderer m_svgOutlinePlaying;


	QList<Node *> m_backwardsRelatedList;
	QList<Node *> m_forwardsRelatedList;

	EdgeLine *p_backwardsEdgeLine;
	Node *p_parent;

	QList< QPair<QString, QString> > m_sourcesList; // m_sourcesList list of lists[3] artist, title, source
	QString m_artist;
	QString m_title;
	QString m_album;
	int m_albumNumber;
	QString m_source;
	QString m_filePath;
	QString m_fileName;

	QString m_foundArtist;
	QString m_foundTitle;

	QPropertyAnimation m_animationAppear;
	QPropertyAnimation m_animationShrink;
	QPropertyAnimation m_animationGrow;

	QPropertyAnimation m_animationX;
	QPropertyAnimation m_animationY;

	static void setPlayingNode(Node *node, bool play = true);
	static Graph *p_graph;
	static QGraphicsScene *p_scene;
	static Node *p_playingNode;
//	static QList<Node*> m_rootList;
	QTimer m_clickReleaseTimer;

	qreal offsetAngle();

	void setHovered(bool hovered);

private slots:
	void animationGrowEnd(){
		m_animationShrink.start();
	}
	void updateSlot();
	void onAnimationMoveEnd();
	void onClickRelease();


public:
	Node(QString artist = "Enter artist", QString title = "Enter title", QString source = QString());
	~Node();
	static Node *playingNode() { return p_playingNode;}
//	static void setPlayingNode(Node *node) { p_playingNode = node;}
	static void setGraph(Graph *graph);
	QRectF boundingRect() const;
	QPainterPath shape() const;
	QRectF innerRect() const;
	QRectF outterRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void addRelated(Node *related, bool playNext = false, bool order = true);
	Node *addRelated(QString artist, QString title, bool playNext = false);
	void addBackwardsRelated(Node *backward, EdgeLine *edgeLine);

	int diameter() const;
	QString artist() const{return m_artist;}
	QString title() const{return m_title;}
	QString album() const{return m_album;}
	QString source() const{return m_source;}
	bool hasSource();
	QList< QPair<QString, QString> > sourcesList() const {return m_sourcesList;}
	QString sourceListAt(int i);
	QString sourceTextAt(int i);
	void clearSources() {m_sourcesList.clear();}
	void addSource(QString description, QString source);

	QList<Node *> *getChildrenList(){return &m_forwardsRelatedList;}
	void orderChildren(int animationDuration = 500);
	void moveChildren(QPointF dpoint, qreal dangle = 0);
	void reorderChildren();
	Node *getParent(){ return p_parent; }
	Node *getGlobalParent();
	void clearBackardsEdgeLine(){ p_backwardsEdgeLine = 0; }
	void clearParent();
	void setPlaying(bool playing, bool keepNext = false);
	void setPlayNext(bool playNext, bool toggle = true, bool ensureVisible = false);
	void setPressed(bool pressed){m_pressed = pressed;}
	bool toPlayNext(){ return m_playNext; }
	bool playing(){ return m_playing;}
	void clearNext();
	Node *next();
	void setAsRoot();
	void setAsPlayingNode(bool centerOnThis = false);
	void paintAsPlayingNode();
	void paintAsNext();
	void centerOnThis();
	void searchForSource(bool onlineToo = false);
	void setAsDropTarget(bool flag){m_dropTarget = flag; update();}
	bool isChildOf(Node *node);
	bool hasRelated(QString artist, QString title);
	void updateOffsetAngle();
	qreal backwardsAngle();
	qreal backwardsAngleOffset();
	EdgeLine *backwardsEdgeLine() {return p_backwardsEdgeLine;}
	void moveToCenter();
	void moveBy(QPointF dpoint);
	static void clearAll();

	void enqueueTillThis();

	void moveAnimated(qreal newX, qreal newY, int duration = 5000);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public slots:
	void setArtist(QString artist);
	void setTitle(QString title);
	void setAlbum(QString album){m_album = album;}
	void setSource(QString source);
	void setFoundArtist(QString artist){m_foundArtist = artist;}
	void setFoundTitle(QString title){m_foundTitle = title;}

	void deleteChildren(Node *excl = 0);
	void deleteSiblings();
	void deleteThis();
	void removeReferenceTo(Node *node);
	void onRelatedFinished();
	bool hasNext();

	void updateLines();
	void hideLines();
	void showLines();



signals:

};

#endif // NODE_H
