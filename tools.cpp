#include "tools.h"
#include <QDebug>
#include <QCryptographicHash>
#include <qmath.h>
#include <QPointF>
#include "settings.h"

//const double PI = 3.14159265;


namespace{
const QString API_KEY = "d84665f06309064914d4ae07488ea6e2";
const QString API_SECRET = "a75d7463d502eae07d8b1b435f966a07";
const double PI = 3.141592653589793238462;
}

QString Tools::lastFmSignature(QList<QPair<QString, QString> > arguments){
	QString toHash = "";
	for(int i=0; i<arguments.count(); i++){
		toHash += arguments.at(i).first;
		toHash += arguments.at(i).second;
	}
	toHash += API_SECRET;

	QString hash = QString(QCryptographicHash::hash(toHash.toLocal8Bit(),QCryptographicHash::Md5).toHex());
	return hash;
}

QString Tools::timeStringFromMilliseconds(long msec)
{
	int sec = msec/1000;
	int min = sec/60;
	int hour = min/60;
	QTime playTime(hour%60, min%60, sec%60, msec%1000);
	QString timeFormat = "m:ss";
	if (hour > 0)
		timeFormat = "h:mm:ss";
	QString timeString = playTime.toString(timeFormat);
	if(timeString == "")
		timeString = "0:00";
	return timeString;
}

QPointF Tools::rotate(QPointF point, QPointF rotationOrigin, qreal angle)
{
	if(angle == 0)
		return point;
	qreal oldX = point.x() - rotationOrigin.x();
	qreal oldY = point.y() - rotationOrigin.y();

	qreal distance = qSqrt(oldX*oldX + oldY*oldY);
	qreal angle0 = qAtan2(oldY, oldX);
	qreal angle1 = angle0 - angle*3.14159265/180;

	qreal newX = distance*qCos(angle1) + rotationOrigin.x();
	qreal newY = distance*qSin(angle1) + rotationOrigin.y();

	return QPointF(newX, newY);
}

int Tools::polygonRadius(int n, int side, int minRes)
{
	//	qDebug() << "polygon n: " << n;
	//	qDebug() << "polygon side: " << side;
	qreal phi = 360/n;
	//	qDebug() << "angle phi:" << phi;
	//	qDebug() << "phi cos:" << qCos(phi*3.14/180);
	qreal theta = (180-phi)/2;
	//	qDebug() << "angle theta: " << theta;
	//	int angle = 90 - 180/n;
	// to convert from degrees to radians: multiply with (pi/180)
	//	theta *= 3.14/180;
	int res = side / (2* qCos(theta* PI /180));
	if(res < minRes)
		res = minRes;
	return res;
}

qreal Tools::angleOfLineToAngle(QLineF line, qreal angle)
{
	qreal res = line.angle() - angle;
	return res;
}

qreal Tools::angleToPositive(qreal angle)
{
	if(angle>=0)
		return angle;
	else{ // angle negative
		return 360+angle;
	}
}

qreal Tools::angleToRational(qreal angle)
{
	if(angle == 360)
		return 0;
	if(angle >180 /*&& angle < 360*/)
		return angle - 360;

	if(angle < -180)
		return 360+angle;

	return angle;
}

QString Tools::styleSheetFromFile(QString file, QString folderForUrl)
{
	QFile f(file);
	f.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(f.readAll());
	f.close();
	styleSheet.replace("url(","url("+folderForUrl+"/");
	return styleSheet;
}

QString Tools::themesLocation()
{
	QString res;
#ifdef Q_OS_WIN
	res =  QApplication::applicationDirPath() + "/themes/";
#endif
#ifdef Q_OS_MAC
	res =  QApplication::applicationDirPath() + "/../Resources/Themes/";
#endif
	return res;
}

QString Tools::resourcesLocation()
{
	QString res;
#ifdef Q_OS_WIN
	res = QApplication::applicationDirPath() + "/resources";
#endif
#ifdef Q_OS_MAC
	res = QApplication::applicationDirPath() + "/../Resources";
#endif
	return res;
}

void Tools::fixTextBrowserSizes(QTextBrowser *textBrowser)
{
	QString html;
	html = textBrowser->toHtml();
	html.replace("font-size:14pt", "font-size:16pt");
//	html.replace("font-size:12pt", "font-size:14pt");
	html.replace("font-size:10pt", "font-size:12pt");
	textBrowser->setHtml(html);
}

int Tools::artistTitleMatch(QString artist, QString title, QString foundArtist, QString foundTitle)
{
	// returning
	// 3: exact match
	// 2: good match
	// 1: partial match (maybe remix)
	// 0: no match

	if(foundArtist == artist && foundTitle == title)
		return 3;

	if(QString::compare(artist, foundArtist,Qt::CaseInsensitive)==0 && QString::compare(title, foundTitle,Qt::CaseInsensitive)==0)
		return 3;

	if(foundArtist.contains(artist, Qt::CaseInsensitive) && foundTitle.contains(title, Qt::CaseInsensitive)){
		if(foundTitle.contains("mix",Qt::CaseInsensitive) && !title.contains("mix",Qt::CaseInsensitive))
			return 2;
		else
			return 1;
	}
	return 0;
}
