#ifndef RESULTSMODEL_H
#define RESULTSMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class ResultsModel : public QAbstractTableModel
{
	Q_OBJECT
private:
	// qlist of ( qlist[0]=artist qlist[1]=title qlist[2]=source )
	QList< QStringList > m_resultsList;
	QStringList m_headerList;
protected:
	Qt::DropActions supportedDragActions();
public:
	explicit ResultsModel(QObject *parent = 0);

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QString record(int row, int column);
	void setResults(QList<QStringList> results);

	QString getArtist(QModelIndex index);
	QString getTitle(QModelIndex index);
	QString getSource(QModelIndex index);

	QString headerTitle(int column);
	void setHeaderList(QStringList list);

signals:
	
public slots:
	
};

#endif // RESULTSMODEL_H
