#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <QList>
#include <QDebug>
//#include "node.h"

class Node;

class ManagedObject{
public:
	ManagedObject();
	~ManagedObject();
	bool managedObjectExists();
};


class ObjectManager
{
private:
	static QList<ManagedObject*> m_objectList;
public:
	ObjectManager();
	static bool objectExists(ManagedObject *object);

	static void addObject(ManagedObject *object);
	static void removeObject(ManagedObject *object);

	template <class Type>
	static bool objectDynamicExists(Type object)
	{
		if(object == 0) return false;
		ManagedObject *p_object = dynamic_cast<ManagedObject*>(object);
		if(p_object == 0 ){
			qDebug() << object;
			qDebug() << "object dynamic FALSE CAST";
			return false;
		}
//		qDebug() << "object dynamic exists";

		if(objectExists(p_object))
			return true;
		else
			return false;

	}

	//my additions for the project
	static QList<Node*> nodeRootList();

	static ManagedObject *randomObject();
};




#endif // OBJECTMANAGER_H
