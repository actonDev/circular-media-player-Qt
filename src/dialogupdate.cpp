#include "dialogupdate.h"
#include "ui_dialogupdate.h"

DialogUpdate::DialogUpdate(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogUpdate)
{
	ui->setupUi(this);
}

DialogUpdate::~DialogUpdate()
{
	delete ui;
}

void DialogUpdate::setText(QString text)
{
	ui->textBrowser->setHtml(text);
//	ui->textBrowser->setText(text);
}
