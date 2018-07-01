#ifndef DIALOGSHORTCUTS_H
#define DIALOGSHORTCUTS_H

#include <QDialog>

namespace Ui {
class DialogKeyboard;
}

class DialogShortcuts : public QDialog
{
	Q_OBJECT
	
public:
	explicit DialogShortcuts(QWidget *parent = 0);
	~DialogShortcuts();
	
private:
	Ui::DialogKeyboard *ui;
};

#endif // DIALOGSHORTCUTS_H
