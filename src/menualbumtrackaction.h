#ifndef MENUALBUMTRACKACTION_H
#define MENUALBUMTRACKACTION_H

#include <QAction>

class MenuAlbumTrackAction : public QAction
{
	Q_OBJECT
private:
	QString m_artist;
	QString m_title;
public:
	explicit MenuAlbumTrackAction(QString artist, QString title, int number = 0, QObject *parent = 0);
	~MenuAlbumTrackAction();
signals:

private slots:
	void onTrigger();
	
public slots:
	
};

#endif // MENUALBUMTRACKACTION_H
