#ifndef HISTORYSEARCH_H
#define HISTORYSEARCH_H

#include <QWidget>

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>

#include <QSqlDatabase>
#include <QSqlError>
#include <QTimer>


namespace Ui {
class SearchForm;
}
class HistorySearch : public QWidget
{
	Q_OBJECT
private:
	QTimer m_timer;
	Ui::SearchForm *ui;
	QSqlQueryModel m_model;
	static QSqlDatabase m_historyDb;
	QString getSqlSearchQuery(QString text);

	bool openDb();
	void doSearch();
public:
	explicit HistorySearch(QWidget *parent = 0);
	static bool trackExistsLastN(QString artist, QString title, int n);
	static bool trackExistsLastNSecs(QString artist, QString title, int n);
	~HistorySearch();
	
signals:
	void doubleCliked(QString artist, QString title, QString source);
public slots:
	void on_lineEdit_textChanged(const QString &arg1);
	void on_lineEdit_returnPressed();
	void on_searchButton_clicked();
	void sendDoubleClick(QModelIndex index);
	void addToHistory(QString artist, QString title, QString source);
	
};

#endif // HISTORYSEARCH_H
