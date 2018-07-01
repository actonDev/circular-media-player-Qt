#include "resultsmodel.h"
#include <QDebug>
#include <QStringListModel>

ResultsModel::ResultsModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	m_headerList.append("Artist");
	m_headerList.append("Title");
}

int ResultsModel::rowCount(const QModelIndex &parent) const
{
	return m_resultsList.count();
}

int ResultsModel::columnCount(const QModelIndex &parent) const
{
	return m_headerList.count();
}

QVariant ResultsModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return QVariant();
	if (index.row() >= m_resultsList.count() || index.row() < 0)
		return QVariant();
	if(role == Qt::DisplayRole){
		QList<QString> entry = m_resultsList.at(index.row());
		return entry.at(index.column());
	}
	return QVariant();
}

QVariant ResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if(section < 0)
		return QVariant();
	else if(section <= m_headerList.count()-1 && orientation == Qt::Horizontal){
		return m_headerList.at(section);
	}
	else
		return QVariant();

//	if (orientation == Qt::Horizontal) {
//		switch (section) {
//			case 0:
//				return tr("Artist");
//			case 1:
//				return tr("Title");
//			case 2:
//				return tr("Source");

//			default:
//				return QVariant();
//		}
//	}
//	return QVariant();
}

QString ResultsModel::record(int row, int column)
{
	if(row>m_resultsList.count()-1 || row<0)
		return "";
	if(column>m_resultsList.at(row).count()-1 || column<0)
		return "";
	return m_resultsList.at(row).at(column);
}

void ResultsModel::setResults(QList<QStringList > results)
{
	emit layoutAboutToBeChanged();
	if(results.count()>0)
		m_headerList = results.takeAt(0);
	m_resultsList = results;
	emit layoutChanged();
	emit headerDataChanged(Qt::Horizontal,0, m_headerList.count()-1);
//	emit columnsAboutToBeRemoved();
}

QString ResultsModel::getArtist(QModelIndex index)
{
	QString res = record(index.row(), 0);
	return res;
}

QString ResultsModel::getTitle(QModelIndex index)
{
	QString res = record(index.row(), 1);
	return res;
}

QString ResultsModel::getSource(QModelIndex index)
{
	QString res = record(index.row(), 2);
	return res;
}

QString ResultsModel::headerTitle(int column)
{
	if(column < 0)
		return QString();
	else if(column <= m_headerList.count()-1){
		return m_headerList.at(column);
	}
	else
		return QString();
}

void ResultsModel::setHeaderList(QStringList list)
{
	m_headerList = list;
}

Qt::DropActions ResultsModel::supportedDragActions()
{
	return Qt::CopyAction | Qt::MoveAction;
}
