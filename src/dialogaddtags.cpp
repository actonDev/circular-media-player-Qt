#include "dialogaddtags.h"
#include "ui_dialogaddtags.h"

DialogAddTags::DialogAddTags(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogAddTags)
{
	ui->setupUi(this);
}

DialogAddTags::~DialogAddTags()
{
	delete ui;
}

QString DialogAddTags::tags()
{
	return ui->lineEdit->text();
}
