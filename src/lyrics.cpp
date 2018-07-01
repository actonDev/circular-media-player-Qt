#include "lyrics.h"
#include "ui_lyricsform.h"

Lyrics::Lyrics(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LyricsForm)
{
	ui->setupUi(this);
	connect(&m_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotReply(QNetworkReply*)));
	QString styleSheet = QApplication::applicationDirPath() + "/style/textStyle.css";

//	setObjectName("Lyrics");
}

void Lyrics::searchFor(QString artist, QString title)
{
	m_artist = artist;
	m_title = title;
	ui->textEdit->setText("Searching lyrics for '" + artist + " - " + title+"'");
	search_lyrdb_com();
}

void Lyrics::search_lyrdb_com()
{
	QUrl urlRequest;
	QByteArray byteRequest = "http://webservices.lyrdb.com/lookup.php?q=";
	byteRequest += QUrl::toPercentEncoding(m_artist) + "|" + QUrl::toPercentEncoding(m_title);
	byteRequest += "&for=match&agent=CircularMediaPlayer";
	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	m_network.get(request);
}

void Lyrics::reply_lyrdb_com(QNetworkReply *reply)
{
	QByteArray replyArray = reply->readAll();
	QString replyText(replyArray);
//	qDebug() << "lyrdb.com reply " << reply->url();
//	qDebug() << replyText;
	QString path(reply->url().path());
	QUrlQuery urlQuery(reply->url());
	QString q = urlQuery.queryItemValue("q");
	reply->deleteLater();

	//###	FIRST REPLY, LYRICS FOUND
	if(path == "/lookup.php"){
//		qDebug()<< "reply query = " << q;
		QStringList list = q.split("|");
//		qDebug() << list.count();
		if(list.count()==2){
//			qDebug() << "artist = " << list.at(0) << " , title = " << list.at(1);
			if( (list.at(0) != m_artist) || (list.at(1) != m_title) ){
//				qDebug() << "reply from other!!SKIPPING ";
				return;
			}
			else{
//				qDebug() << "reply from the same:" << m_title;
			}
		}
		// to see what the results look like check: http://webservices.lyrdb.com/lookup.php?q=trains&for=trackname&agent=cmp
		// now from these, try to find porcupine tree
		QStringList listLines =	replyText.split("\n", QString::SkipEmptyParts); //lines of the results
		QList<QStringList> listSplitted; //list of list: every result line will be a list: [id  artist  track]

		for(int i=0; i<listLines.count(); i++){
			listSplitted.append( listLines.at(i).split("\\"));
			if(listSplitted.at(i).count()!=3){
//				qDebug() << "not valid format";
				continue;
			}
			if(QString::compare(m_artist, listSplitted.at(i).at(2),Qt::CaseInsensitive )==0){ //compare returns 0 if equal!
				m_network.get(QNetworkRequest("http://webservices.lyrdb.com/getlyr.php?q="+listSplitted.at(i).at(0)));
				return; //found result
			}
		}
		//no match
		ui->textEdit->setText(ui->textEdit->toPlainText() + "\nNo lyrics found!");
	}

	//### LYRICS TEXT
	else if(path == "/getlyr.php"){
		ui->textEdit->setText("Lyrics for: " + m_artist + " - " + m_title + "\n\n" + replyText);
	}
}

void Lyrics::gotReply(QNetworkReply *reply)
{
	QString host(reply->url().host());
//	qDebug() << "NETWORK REPLY FROM " << host;
	if(host=="webservices.lyrdb.com")
		reply_lyrdb_com(reply);

}
