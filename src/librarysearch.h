#ifndef LIBRARYSEARCH_H
#define LIBRARYSEARCH_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>

#include <QSqlDatabase>
#include <QSqlError>
#include <QFileDialog>
#include <QDirIterator>

#include <QtConcurrent/QtConcurrentRun>
#include <QTimer>


namespace Ui {
class SearchForm;
}

class LibrarySearch : public QWidget
{
	Q_OBJECT
private:
	QTimer m_timer;
	QSqlDatabase m_libraryDb;

	QSqlQueryModel m_model;
	Ui::SearchForm *ui;
	QString getSqlSearchQuery(QString text);
	bool openDb();
	void scanDir(QDir dir);
	void doSearch();
	
public:
	explicit LibrarySearch(QWidget *parent = 0);
	~LibrarySearch();
	Ui::SearchForm *getUi(){ return ui; }
	QString searchForSource(QString artist, QString title);
	QList<QStringList> searchForSources(QString artist, QString title);
	
private slots:
	void on_lineEdit_textChanged(const QString &arg1);
	void on_lineEdit_returnPressed();
	void sendDoubleClick(QModelIndex index);
	void clearDatabase();
	void populateDatabase();
	void on_searchButton_clicked();

signals:
	void doubleCliked(QString artist, QString title, QString source);
	void addedFile(QString source);

};

#endif // LIBRARYSEARCH_H
