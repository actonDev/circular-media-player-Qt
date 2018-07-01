#ifndef TOOLS_H
#define TOOLS_H

#include "cstdlib"
#include "time.h"
#include <QDebug>
#include <QTime>
#include <QList>
#include <QTextBrowser>

//class Node;

class Tools{

public:
	static int randInt(int low, int high)
	{
		// Random number between low and high
		time_t seconds;
		time(&seconds);
		QTime time = QTime::currentTime();
		srand((unsigned int) seconds);
		int res = rand()%(high - low +1) + low;
		//		qDebug() << "Tools::randInt = " << res;
		return res;
	}

	static QList<int> randomN(int low, int high, int n){
		QList<int> res;
		time_t seconds;
		time(&seconds);
		srand((unsigned int) seconds);
		int gen = rand() % (high - low + 1) + low;
		for(int i=0; i<n; i++){
			while(res.contains(gen)){
				gen = rand() % (high - low + 1) + low;
			}
			res.append(gen);
		}
		return res;
	}

	template <class Type, typename Func>
	static QList<Type> orderList(QList<Type> list, Func func, bool ascending = true ){
		qDebug() << "Tools::orderList" ;
		int count = list.count();
//		for(int i=0; i<count; i++){
//			Type object = list.at(i);
//			qDebug() << "angle of " << i << ", = " << func(object);
//		}

		//buble sorting
		for(int i=count-2; i>=0; i--){

			for(int j=0; j<=i; j++){
				if(ascending){
					if( func(list.at(j)) > func(list.at(j+1)) && ascending ){
						Type temp = list.at(j);
						list.replace(j, list.at(j+1));
						list.replace(j+1, temp);
					}
				}
				else if( func(list.at(j)) < func(list.at(j+1)) ){
					Type temp = list.at(j);
					list.replace(j, list.at(j+1));
					list.replace(j+1, temp);
				}
			}
		}

//		qDebug();
//		qDebug() << "#### ordered: ";
//		for(int i=0; i<count; i++){
//			Type object = list.at(i);
//			qDebug() << "angle of " << i << ", = " << func(object);
//		}

		return list;
	}

	template <class Type, typename Func>
	static void orderTest(Type object, Func func ){
		//		func(object);
		qDebug() << func(object);
	}

	static QString lastFmSignature(QList< QPair<QString, QString> > arguments);
	static QString timeStringFromMilliseconds(long msec);
	static QPointF rotate(QPointF point, QPointF rotationOrigin, qreal angle);

	static int polygonRadius(int n, int side, int minRes = 0);

	static qreal angleOfLineToAngle(QLineF line, qreal angle);
	static qreal angleToPositive(qreal angle);
	static qreal angleToRational(qreal angle);

	static QString styleSheetFromFile(QString file, QString folderForUrl);

	static QString themesLocation();
	static QString resourcesLocation();
	static void fixTextBrowserSizes(QTextBrowser *textBrowser);
	static int artistTitleMatch(QString artist, QString title, QString foundArtist, QString foundTitle);
};

#endif // TOOLS_H
