#ifndef DIALOGADDTAGS_H
#define DIALOGADDTAGS_H

#include <QDialog>

namespace Ui {
class DialogAddTags;
}

class DialogAddTags : public QDialog
{
	Q_OBJECT
	
public:
	explicit DialogAddTags(QWidget *parent = 0);
	~DialogAddTags();
	QString tags();
	
private:
	Ui::DialogAddTags *ui;
};

#endif // DIALOGADDTAGS_H
