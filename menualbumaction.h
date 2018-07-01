#ifndef MENUALBUMACTION_H
#define MENUALBUMACTION_H

#include <QAction>

class MenuAlbumAction : public QAction
{
	Q_OBJECT
private:
	QString m_artist;
	QString m_album;
public:
	explicit MenuAlbumAction(QString artist, QString album, QObject *parent = 0);
	
signals:
	
public slots:

private slots:
	void onTriggered();
	
};

#endif // MENUALBUMACTION_H
