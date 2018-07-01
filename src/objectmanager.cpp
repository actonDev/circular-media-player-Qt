#include "objectmanager.h"
#include "node.h"
#include "tools.h"
QList<ManagedObject*> ObjectManager::m_objectList;

ObjectManager::ObjectManager()
{

}

bool ObjectManager::objectExists(ManagedObject *object)
{
	if(object == 0) return false;
	for(int i=0; i<m_objectList.count(); i++){
		if(m_objectList.at(i) == object)
			return true;
	}
	return false;
}

void ObjectManager::addObject(ManagedObject * object)
{
	m_objectList.append(object);
}

void ObjectManager::removeObject(ManagedObject * object)
{
//	m_objectList.
	if(!objectExists(object))
		return;
	for(int i=0; i<m_objectList.count(); i++){
		if(m_objectList.at(i) == object)
			m_objectList.takeAt(i);
	}
}

QList<Node *> ObjectManager::nodeRootList()
{
	QList<Node *> list;
	for(int i=0; i< m_objectList.count(); i++){
		Node *node = (Node*)m_objectList.at(i);
		if(node->getParent() == 0)
			list.append(node);
	}
	return list;
}

ManagedObject *ObjectManager::randomObject()
{
	ManagedObject *object = 0;
	int count = m_objectList.count();
	if(count == 0)
		return object;
	int random = Tools::randInt(0, count -1);
	object = m_objectList.at(random);
	return object;
}


// OBJECT

ManagedObject::ManagedObject()
{
//	qDebug() << "ManagedObject::ManagedObject()";
	ObjectManager::addObject(this);
}

ManagedObject::~ManagedObject()
{
//	qDebug() << "ManagedObject::~ManagedObject()";
	ObjectManager::removeObject(this);
}

bool ManagedObject::managedObjectExists()
{
	return ObjectManager::objectExists(this);
}

