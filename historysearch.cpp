#include "historysearch.h"
#include "ui_searchform.h"

#include <QMessageBox>
#include "settings.h"
#include "tools.h"
QSqlDatabase HistorySearch::m_historyDb;

HistorySearch::HistorySearch(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SearchForm)
{
	ui->setupUi(this);
	//	ui->comboBox->setVisible(false);
	openDb();
	m_model.setQuery(getSqlSearchQuery(""), m_historyDb);
	ui->tableView->setModel(&m_model);
	ui->lineEdit->setText("");
	ui->tableView->setColumnWidth(0, 200);
	ui->tableView->setColumnWidth(1, 200);
	//	connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_lineEdit_returnPressed()));
	connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sendDoubleClick(QModelIndex)));
	//	m_timer.start(5000);
}

bool HistorySearch::trackExistsLastN(QString artist, QString title, int n)
{
	if(n==0)
		return false;

	QSqlQueryModel model;
	QString query = "SELECT artist, title FROM tracks ORDER BY ID DESC";
	model.setQuery(query, m_historyDb);
	//		qDebug() << "last n: " << n << " model row" << model.rowCount();
	for(int i=0; i<n; i++){
		if(model.rowCount()<i-1)
			return false;
		QString artistRec = model.record(i).field("artist").value().toString();
		QString titleRec = model.record(i).field("title").value().toString();
		//			qDebug() << i << " " << artistRec << " - " << titleRec;
		if(artist==artistRec && title==titleRec){
			qDebug() << "ALREADY PLAYED " << artist << " - " << title;
			return true;
		}
	}

	return false;
}

bool HistorySearch::trackExistsLastNSecs(QString artist, QString title, int n)
{
	if(n==0)
		return false;

	QSqlQueryModel model;
	QString query = "SELECT artist, title, date FROM tracks WHERE artist='" + artist +"' AND title='" + title+"'";
	query += " ORDER BY ID DESC";

	model.setQuery(query, m_historyDb);
//	qDebug() << "historySearch lastNSecs rowCount for " <<artist << " - " << title << " : "<< model.rowCount();

	if(model.rowCount() == 0 )
		return false;

	QString dateString = model.record(0).field("date").value().toString();
	QDateTime date = QDateTime::fromString(dateString, "dd/MM/yyyy hh:mm" );
	date = date.addSecs(n);
	QString newDateString = date.toString("dd/MM/yyyy hh:mm");
//	qDebug() << "old string " << dateString << ", new string: " << newDateString;
	QDateTime dateNow = QDateTime::currentDateTime();
	if(date>dateNow){
//		qDebug() << "returning true";
		return true;
	}


	return false;
}

HistorySearch::~HistorySearch()
{
	delete ui;
}

void HistorySearch::on_lineEdit_textChanged(const QString &arg1)
{
	if(Settings::instance()->INSTANT_SEARCH)
		doSearch();
}

void HistorySearch::on_lineEdit_returnPressed()
{
	doSearch();
}

void HistorySearch::on_searchButton_clicked()
{
	doSearch();
}

void HistorySearch::sendDoubleClick(QModelIndex index)
{
	int row = index.row();
	QString artist = m_model.record(row).field("artist").value().toString();
	QString title = m_model.record(row).field("title").value().toString();
	QString source = m_model.record(row).field("source").value().toString();
	//to start playing this
	emit doubleCliked(artist, title, source);
}

void HistorySearch::addToHistory(QString artist, QString title,QString source)
{
	QSqlQuery qry(m_historyDb);
	qry.prepare("INSERT INTO tracks(artist, title, date, source)"
				"VALUES (:artist, :title, :date, :source)");
	qry.bindValue(":artist",  artist);
	qry.bindValue(":title",  title);
	qry.bindValue(":source",  source);
	QString dateString = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
	qry.bindValue(":date", dateString);
	if(!qry.exec())
		qDebug() << "failed to add in history: " << qry.lastError();
	doSearch();
}

bool HistorySearch::openDb()
{
	m_historyDb = QSqlDatabase::addDatabase("QSQLITE", "history");
	m_historyDb.setDatabaseName(Tools::resourcesLocation() + "/history.db");
	if(!m_historyDb.open()){
		qDebug() << m_historyDb.lastError();
		qDebug() << "failed to connect";
		QString error = "Failed to open history database:\n" + m_historyDb.lastError().text();
		QMessageBox::warning(0, tr("Error with the library"),
							 error);
		return false;
	}

	QSqlQuery query(m_historyDb);

	query.prepare("CREATE TABLE IF NOT EXISTS tracks"
				  "(id INTEGER PRIMARY KEY, 'artist', 'title', 'source', 'date')");
	if(!query.exec())
		qDebug() << "failed to exec query! (history)";

	return true;

}

void HistorySearch::doSearch()
{
	QString query = getSqlSearchQuery( ui->lineEdit->text() );
	m_model.setQuery(query, m_historyDb);
}

QString HistorySearch::getSqlSearchQuery(QString text)
{
	QStringList searchList = text.split(" ");
	QString term = "";
	for(int i=0; i<searchList.length(); i++){
		if(i>0)
			term += " AND artist || title LIKE";
		term += "'%" + searchList.at(i) + "%'";
	}
	QString query = "SELECT artist AS 'Artist', title AS 'Title', date as 'Date', source AS 'Source' FROM tracks WHERE artist || title LIKE " + term;
	query += " ORDER BY ID DESC";
	return query;
}
