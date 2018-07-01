#include "customtableview.h"
#include <QDebug>
#include <QVariant>
#include <QPaintEvent>

CustomTableView::CustomTableView(QWidget *parent) :
	QTableView(parent)
{
	//	setSelectionMode(QAbstractItemView::SingleSelection);
	setDragDropOverwriteMode(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	//	setDragEnabled(true);
	//	setDragDropMode(QAbstractItemView::DragOnly);

	m_loading = false;
	m_movie.setCacheMode(QMovie::CacheAll);
	m_movie.start();
	QString fileName = Settings::instance()->themeFolder() + "/loading.gif";
	m_movie.setFileName(fileName);
	connect(&m_movie, SIGNAL(frameChanged(int)), this, SLOT(repaint()));
	connect(&m_movie, SIGNAL(frameChanged(int)), this, SLOT(onFrameChange(int)));
//	connect(this, SIGNAL(cli))
	//	setLoading(true);
	//	QLabel *label = new QLabel();
	//	m_movie.stop();
	m_label.setMovie(&m_movie);

//	QHeaderView::setClickable(true);
	//horizontalHeader()->setClickable(true);
	//connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sort(int)));
//	setcli
}

void CustomTableView::setLoading(bool loading)
{
	m_loading = loading;
	if(loading){
		m_movie.start();
		//		m_label.show();
	}
	else{
		m_movie.stop();
		//		m_label.hide();
	}
	//	m_loading = loading;
}

void CustomTableView::mousePressEvent(QMouseEvent *event)
{
	qDebug() << "press";
	QTableView::mousePressEvent(event);
	if(event->button() == Qt::RightButton){
//		emit
		emit rightClicked(currentIndex());
	}

}

void CustomTableView::mouseMoveEvent(QMouseEvent *event)
{
	QDrag *drag = new QDrag(this);
	qDebug() << "CustomTableView selected indexes cout: " << selectedIndexes().count() ;
	if(selectedIndexes().count()==0)
		return;
	QList<int> selectedRows;
	qDebug() << "selected rows count: " << selectedRows.count();
	for(int i=0; i<selectedIndexes().count(); i++){
		int row = selectedIndexes().at(i).row();
		if(!selectedRows.contains(row))
			selectedRows.append(row);
	}

	QString artist;
	int artistColumn = -1;
	QString title;
	int titleColumn = -1;
	QString source;
	int sourceColumn = -1;
	for(int i=0; i<model()->columnCount();i++){
		if(model()->headerData(i,Qt::Horizontal).toString() == "Artist"){
			artistColumn = i;
		}
		if(model()->headerData(i,Qt::Horizontal).toString() == "Title"){
			titleColumn = i;
		}
		if(model()->headerData(i,Qt::Horizontal).toString() == "Source"){
			sourceColumn = i;
		}
	}

	if(artistColumn == -1 || titleColumn == -1)
		return;

	QList<QStringList> list;
	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-cmp", "true");
	mimeData->setData("count", QString::number(selectedRows.count()).toLocal8Bit());
	for(int i=0; i<selectedRows.count(); i++){
		int row = selectedRows.at(i);
		if(artistColumn>-1){
			QModelIndex artistIndex = model()->index(row,artistColumn);
			artist = model()->data(artistIndex).toString();
			qDebug() << "artist"<< i << ": " << artist;
//			mimeData->setData("artist"+i, artist.toLocal8Bit());
		}
		if(titleColumn>-1){
			QModelIndex titleIndex =  model()->index(row,titleColumn);
			title = model()->data(titleIndex).toString();
			qDebug() << "title"<<i <<": " << title;
//			mimeData->setData("title"+i, title.toLocal8Bit());
		}
		if(sourceColumn>-1){
			QModelIndex sourceIndex =  model()->index(row,sourceColumn);
			source = model()->data(sourceIndex).toString();
//			mimeData->setData("source"+i, source.toLocal8Bit());
		}
//		QPair<QString, QString> pair;
//		pair.first = artist;
//		pair.second = title;
		QStringList entry;
		entry.append(artist);
		entry.append(title);
		entry.append(source);
		list.append(entry);

	}

	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	stream << list;
//	encodedData << list;

	mimeData->setData("application/x-cmp-table-transfer", encodedData);

	qDebug() << "MIMEDATA: ";
//	for(int i=0; i<selectedRows.count(); i++){
//		QString artist = mimeData->data("artist"+i);
//		qDebug() << "artist"<< i << ": " << artist;
//		QString title = mimeData->data("title"+i);
//		qDebug() << "title"<< i << ": " << title;
//	}

	QByteArray encodedData2 =  mimeData->data("application/x-cmp-table-transfer");
	QDataStream stream2(&encodedData2, QIODevice::ReadOnly);
	QList<QStringList> list2;
	stream2 >> list2;
	qDebug() << list2.count();


	//	qDebug() << "move";

	//	mimeData->setData("artist", artist.toLocal8Bit());
	//	mimeData->setData("title", title.toLocal8Bit());
	//	mimeData->setData("source", source.toLocal8Bit());
	mimeData->setData("action", "create");
	drag->setMimeData(mimeData);
	drag->exec();

}

void CustomTableView::paintEvent(QPaintEvent *e)
{
	QTableView::paintEvent(e);
	if(m_loading){
		QPixmap currentFrame = m_movie.currentPixmap();
		QRect frameRect = currentFrame.rect();

		frameRect.moveCenter(rect().center());
		if (frameRect.intersects(e->rect())) {
			QPainter painter(this->viewport());
			painter.drawPixmap(frameRect.left(),frameRect.top(),currentFrame);
		}
	}

//	qDebug() << "custom table view painting";

}

void CustomTableView::onFrameChange(int frame)
{
	qDebug() << "movie frame: " << frame;
}

void CustomTableView::sort(int id)
{
	qDebug() << "table sort " << id;
	sortByColumn(id, Qt::AscendingOrder);
}
