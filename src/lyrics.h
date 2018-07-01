#ifndef LYRICS_H
#define LYRICS_H

#include <QWidget>
#include <QtNetwork>

namespace Ui {
class LyricsForm;
}
class Lyrics : public QWidget
{
	Q_OBJECT
private:
	Ui::LyricsForm *ui;
	QNetworkAccessManager m_network;
	QString m_artist;
	QString m_title;
public:
	explicit Lyrics(QWidget *parent = 0);
	
signals:
	
public slots:
	void searchFor(QString artist, QString title);
	void search_lyrdb_com();
	void reply_lyrdb_com(QNetworkReply *reply);
	void gotReply(QNetworkReply *reply);
};

#endif // LYRICS_H
