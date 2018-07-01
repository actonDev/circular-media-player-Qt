#include "lastfmrelated.h"
#include "node.h"
#include "graph.h"
#include "Tools.h"
#include "historysearch.h"
#include "mynetworkreply.h"
#include "settings.h"


LastFmRelated::LastFmRelated(QObject *parent, Graph *graph) : QObject(parent)
{
	qDebug() << "lastfm constructor";
	p_graph = graph;
}

void LastFmRelated::getRelated(Node *node)
{
	// if a request is already pending for a node
	// meaning a request got sent but meanwhile no reply got received
	// return! (to avoid getting many replies at some point)
	if(nodeExists(node))
		return;
	QUrl urlRequest;
	m_requestingNodes.append(node);

	QByteArray byteRequest = "http://ws.audioscrobbler.com/2.0/?method=track.getsimilar&artist=";
	byteRequest += QUrl::toPercentEncoding(node->artist());
	byteRequest += "&track=";
	byteRequest += QUrl::toPercentEncoding(node->title().replace("(original mix)", "", Qt::CaseInsensitive));
	byteRequest += "&api_key=d84665f06309064914d4ae07488ea6e2";
	byteRequest += "&autocorrect=1";

	urlRequest = QUrl::fromEncoded(byteRequest);

	QNetworkRequest request;
	request.setUrl(urlRequest);
	MyNetworkReply *myReply = new MyNetworkReply;
	myReply->setSender(node);
	myReply->setReply( m_network.get(request) );
	connect(myReply, SIGNAL(myReplyFinished(Node*,QNetworkReply*)), this, SLOT(replyFinished(Node*,QNetworkReply*)));
	p_graph->changeStatusInfo("Sending Last.fm request: " + request.url().toString());
}

void LastFmRelated::replyFinished(Node *sender, QNetworkReply *reply)
{
	//sample: http://ws.audioscrobbler.com/2.0/?method=track.getsimilar&artist=noisia&track=stigma&api_key=d84665f06309064914d4ae07488ea6e2
	QByteArray response(reply->readAll());
	qDebug() << "related reply : " << reply->url();
	QString replyText(response);
	replyText.resize(200);
	qDebug() << replyText;
	reply->deleteLater();

	if(!isNodeValid(sender)){
		qDebug() << "requester node deleted";
		return;
	}
	QString replyInfo = "";
	QString artist = "";
	QString title = "";
	double match = 0;

	//### qlist of qlists([0] = title, [1] = artist )
	QList< QList<QString> > resultsList;
	QList<QString> entry;
	//!##

	QXmlStreamReader xml(response);
	while(!xml.atEnd()){
//		int dontAdd = Settings::instance()->DONT_ADD_HISTORY_LAST_N;
		int dontAddSecs = Settings::instance()->DONT_ADD_HISTORY_LAST_N_HOURS*3600;
		xml.readNext();
		if(xml.isStartElement()){

			if(xml.name().toString() == "track"){
				xml.readNextStartElement();
				title = xml.readElementText();
				entry.append(title);
			}
			if(xml.name().toString() == "match"){
				match = xml.readElementText().toDouble();
			}
			if(xml.name().toString() == "artist"){
				xml.readNextStartElement();
				artist = xml.readElementText();
				entry.append(artist);
				if(!sender->hasRelated(artist, title)){
					if(!HistorySearch::trackExistsLastNSecs(artist, title,dontAddSecs)){
						if(!Settings::instance()->DONT_ADD_SAME_ARTIST)
							resultsList.append(entry);
						else if(sender->artist()!=artist)
							resultsList.append(entry);
					}
				}
				entry.clear();
				if(resultsList.count()>20)
					break;
			}
		}
	}

	// #######
	// read xml, now handling the random selecting, and adding related
	// #######

	if(resultsList.isEmpty()){
		replyInfo += "No related found! Maybe try to (right click -> edit) to change artist - title?";
	}
	else{
		replyInfo += "added related ";

		int count = resultsList.count();
		QList<int> selectedList;

		int toAdd = qMin(resultsList.count(), Settings::instance()->RELATED_TO_ADD);
		selectedList = Tools::randomN(0, count-1, toAdd);

		//now to add related!
		for(int i=0; i<toAdd; i++){
			QString title = resultsList.at( selectedList.at(i) ).at(0);
			QString artist = resultsList.at( selectedList.at(i) ).at(1);
			Node *circle = new Node(artist, title);
			circle->searchForSource(false);
			sender->addRelated(circle);
			replyInfo+= "["+artist+","+title+"] ";
		}
	}

	p_graph->changeStatusInfo("Last.fm reply: " + replyInfo);
	//if no node set to play next, get one random
	if(!sender->hasNext())
		QTimer::singleShot(10000, sender, SLOT(onRelatedFinished()));
}

void LastFmRelated::onNodeDeleted(Node *deletedNode)
{
	isNodeValid(deletedNode); //removing it from the list
}

bool LastFmRelated::isNodeValid(Node *node) // check if node is valid, eg it exists in the requestingNodes list, and removing it
{
	for(int i=0; i<m_requestingNodes.count(); i++){
		if(m_requestingNodes.at(i) == node){
			m_requestingNodes.takeAt(i);
			return true;
		}
	}
	return false;
}

bool LastFmRelated::nodeExists(Node *node)
{
	for(int i=0; i<m_requestingNodes.count(); i++){
		if(m_requestingNodes.at(i) == node){
			return true;
		}
	}
	return false;
}
