#include "dialogabout.h"
#include "ui_dialogabout.h"
#include "settings.h"
#include "Tools.h"
DialogAbout::DialogAbout(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogAbout)
{
	ui->setupUi(this);
	setWindowTitle("About");
//	ui->image->setStyleSheet("QWidget {background-image: url(:/cmp_logo.png) }");
	ui->label->setText("Circular Media Player, version "+ Settings::instance()->version);

	QFile f(Tools::resourcesLocation()+"/whatsnew.txt");
	f.open(QFile::ReadOnly);
	QString whatsnew = QLatin1String(f.readAll());

	ui->textBrowser_new->setText(whatsnew);
}

DialogAbout::~DialogAbout()
{
	delete ui;
}
