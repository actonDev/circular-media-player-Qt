#include "dialogshortcuts.h"
#include "ui_dialogshortcuts.h"

DialogShortcuts::DialogShortcuts(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogKeyboard)
{
	ui->setupUi(this);
	setWindowTitle("Shortcuts");
}

DialogShortcuts::~DialogShortcuts()
{
	delete ui;
}
