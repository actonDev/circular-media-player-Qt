#include "myscene.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>

MyScene::MyScene(QObject *parent) : QGraphicsScene(parent)
{

}

void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "SCENE press";
}
