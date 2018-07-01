#include "librarysearch.h"
#include "ui_searchform.h"

//#include "taglib/tag.h"
//#include "taglib/fileref.h"
#include "taglib.h"
#include "fileref.h"
#include <QMessageBox>
#include "settings.h"
#include "tools.h"

LibrarySearch::LibrarySearch(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SearchForm)
{
	ui->setupUi(this);
	//	ui->comboBox->setVisible(false);
	openDb();
	m_model.setQuery(getSqlSearchQuery(""), m_libraryDb);
	ui->tableView->setModel(&m_model);
	ui->tableView->setColumnWidth(0, 200);
	ui->tableView->setColumnWidth(1, 200);
	ui->lineEdit->setText("");
	connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sendDoubleClick(QModelIndex)));
	//	connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_lineEdit_returnPressed()));
	//	m_timer.start(5000);
}

LibrarySearch::~LibrarySearch()
{
	delete ui;
}

QString LibrarySearch::searchForSource(QString artist, QString title)
{
	QSqlQueryModel model;
	model.setQuery( getSqlSearchQuery(artist + " " + title), m_libraryDb );
	QString source = "";
	if(model.rowCount()>0)
		source = model.record(0).field("source").value().toString();
	return source;
}

QList< QStringList > LibrarySearch::searchForSources(QString artist, QString title)
{
	QSqlQueryModel model;
	model.setQuery( getSqlSearchQuery(artist + " " + title), m_libraryDb );
	QList< QStringList > resultsList;
	for(int i=0; i<model.rowCount(); i++){
		QString foundArtist = model.record(i).field("artist").value().toString();
		QString foundTitle = model.record(i).field("title").value().toString();
		QString source = model.record(i).field("source").value().toString();
		if(Tools::artistTitleMatch(artist, title, foundArtist, foundTitle) == 3){
			QStringList sourceList;
			sourceList.append(foundArtist);
			sourceList.append(foundTitle);
			sourceList.append(source);
			resultsList.append(sourceList);
		}
		if(i==9) continue; //if 10 sources reached, continue
	}
	return resultsList;
}
void LibrarySearch::doSearch()
{
	QString query = getSqlSearchQuery( ui->lineEdit->text() );
	m_model.setQuery(query, m_libraryDb);
}

void LibrarySearch::on_searchButton_clicked()
{
	doSearch();
}

void LibrarySearch::on_lineEdit_textChanged(const QString &arg1)
{
	if(Settings::instance()->INSTANT_SEARCH){
		QString query = getSqlSearchQuery(arg1);
		m_model.setQuery(query, m_libraryDb);
	}
}

void LibrarySearch::on_lineEdit_returnPressed()
{
	doSearch();
}

void LibrarySearch::sendDoubleClick(QModelIndex index)
{
	int row = index.row();
	QString artist = m_model.record(row).field("artist").value().toString();
	QString title = m_model.record(row).field("title").value().toString();
	QString source = m_model.record(row).field("source").value().toString();

	//to start playing this
	emit doubleCliked(artist, title, source);
}

void LibrarySearch::clearDatabase()
{
	qDebug() << "clear db";
	QSqlQuery qry(m_libraryDb);
	qry.prepare("DELETE FROM tracks");
	if(!qry.exec())
		qDebug() << "failed to clear db";
	doSearch();
}

void LibrarySearch::populateDatabase()
{
	QString dirString = QFileDialog::getExistingDirectory(0, tr("Select database folder"));
	qDebug() << "selected dir = " << dirString;

	QDir dir(dirString);
	QtConcurrent::run(this, &LibrarySearch::scanDir, dir);

}

QString LibrarySearch::getSqlSearchQuery(QString text)
{
	QStringList searchList = text.split(" ");
	QString term = "";
	for(int i=0; i<searchList.length(); i++){
		if(i>0)
			term += " AND artist || title LIKE";
		term += "'%" + searchList.at(i) + "%'";
	}
	//	qDebug() << "SEARCH TERM =  "<< term ;

	QString query = "SELECT artist AS 'Artist', title AS 'Title', source AS 'Source' FROM tracks WHERE artist || title LIKE " + term;
	return query;
}

bool LibrarySearch::openDb()
{
	m_libraryDb = QSqlDatabase::addDatabase("QSQLITE", "library");
	m_libraryDb.setDatabaseName(Tools::resourcesLocation() + "/library.db");
	if(!m_libraryDb.open()){
		qDebug() << m_libraryDb.lastError();
		qDebug() << "failed to connect";
		QString error = "Failed to open database:\n" + m_libraryDb.lastError().text();
		QMessageBox::warning(0, tr("Error with the library"),
							 error);
		return false;
	}

	QSqlQuery query(m_libraryDb);

	query.prepare("CREATE TABLE IF NOT EXISTS tracks"
				  "(id INTEGER PRIMARY KEY, 'artist', 'title', 'duration', 'source', 'filename', 'folder')");
	if(!query.exec())
		qDebug() << "failed to exec query! (library)";

	return true;

}

void LibrarySearch::scanDir(QDir dir)
{
#ifdef HAVE_TAGLIB
	qDebug() << "here adding library!";
	int count = 0;
	QSqlQuery qry(m_libraryDb);
	QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
	QString fileDir = "";
	while (iterator.hasNext()) {
		iterator.next();
		if(iterator.fileInfo().isDir())
			fileDir = iterator.filePath();
		if (!iterator.fileInfo().isDir()) {
			QString source = iterator.filePath();
			QString fileName = iterator.fileName();
			if ( source.endsWith(".mp3") || source.endsWith(".wav") || source.endsWith(".flac") || source.endsWith(".ogg") || source.endsWith(".wma") ){
				emit addedFile("Added file: " + source);
				TagLib::FileRef fileRef(TagLib::FileName(source.toLocal8Bit().constData()));
				if(!fileRef.file()->isValid())
					continue;
				qry.prepare("INSERT INTO tracks(artist, title, source, filename, folder)"
							"VALUES (:artist, :title, :source, :filename, :folder)");

				qry.bindValue(":artist",  fileRef.tag()->artist().toCString());
				qry.bindValue(":title",  fileRef.tag()->title().toCString());
				qry.bindValue(":source", source);
				qry.bindValue(":filename", fileName);
				qry.bindValue(":folder", fileDir);
				if(!qry.exec())
					qDebug() << qry.lastError();
				count++;
				if(count%20 == 0){
					qDebug() << "50 tracks added in library!";
					doSearch();
				}
			}
		}
	}
	qDebug() << "finished!";
	doSearch();
	emit addedFile("Finished!");
#endif
}


