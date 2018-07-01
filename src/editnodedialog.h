#ifndef EDITNODEDIALOG_H
#define EDITNODEDIALOG_H

#include <QDialog>

namespace Ui {
class EditNodeDialog;
}

class EditNodeDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit EditNodeDialog(QWidget *parent = 0);
	~EditNodeDialog();
	void setArtist(QString artist);
	void setTitle(QString title);
	QString artist();
	QString title();
	
private:
	Ui::EditNodeDialog *ui;
};

#endif // EDITNODEDIALOG_H
