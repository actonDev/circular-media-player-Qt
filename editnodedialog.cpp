#include "editnodedialog.h"
#include "ui_editnodedialog.h"

EditNodeDialog::EditNodeDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditNodeDialog)
{
	ui->setupUi(this);
}

EditNodeDialog::~EditNodeDialog()
{
	delete ui;
}

void EditNodeDialog::setArtist(QString artist)
{
	ui->artist->setText(artist);
}

void EditNodeDialog::setTitle(QString title)
{
	ui->title->setText(title);
}

QString EditNodeDialog::artist()
{
	return ui->artist->text();
}

QString EditNodeDialog::title()
{
	return ui->title->text();
}
