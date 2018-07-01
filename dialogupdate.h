#ifndef DIALOGUPDATE_H
#define DIALOGUPDATE_H

#include <QDialog>

namespace Ui {
class DialogUpdate;
}

class DialogUpdate : public QDialog
{
	Q_OBJECT
	
public:
	explicit DialogUpdate(QWidget *parent = 0);
	~DialogUpdate();
	void setText(QString text);
	
private:
	Ui::DialogUpdate *ui;
};

#endif // DIALOGUPDATE_H
