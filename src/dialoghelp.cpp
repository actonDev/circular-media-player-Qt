#include "dialoghelp.h"
#include "ui_dialoghelp.h"
#include "tools.h"
#include "settings.h"

DialogHelp::DialogHelp(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogHelp)
{
	ui->setupUi(this);
	setWindowTitle("Circular Media Player : Help");
	m_index = 0;
	m_count = 4;
	ui->buttonPrev->setVisible(false);
	ui->opt_dontShowHelp->setChecked(Settings::instance()->DONT_SHOW_HELP);
	connect(this, SIGNAL(rejected()), this, SLOT(onRejected()));
	//ui->movieContainer->addWidget(m_videoWidget.widget());
	ui->labelMovie->setVisible(false);
	ui->labelMovie->setStyleSheet("QLabel{background-image: url(" + Tools::resourcesLocation() + "/Help/logo.gif" +")}");
	//ui->movieContainer->setAlignment(m_videoWidget.widget(), Qt::AlignHCenter);
//	m_videoWidget.setSource(Tools::resourcesLocation() + "/Help/video1.wmv");

#ifdef Q_OS_MAC
	fixTextSizes();
#endif
}

DialogHelp::~DialogHelp()
{
	delete ui;
}

void DialogHelp::resetHelp()
{
	m_index = 0;
	updateHelp(0);
}

void DialogHelp::hide()
{
	m_videoWidget.stop();
	QDialog::hide();
}

void DialogHelp::on_buttonNext_clicked(){
	updateHelp(1);
}
void DialogHelp::on_buttonPrev_clicked(){
	updateHelp(-1);
}

void DialogHelp::on_DialogHelp_rejected()
{
	qDebug() << "help dialog closed";
}

void DialogHelp::updateHelp(int where)
{

	//Step 1: Starting
	//Step 2: Search Modes
	//Step 3: Organizing circles
	if(m_index+where == m_count){
		hide();
		return;
	}

	if(m_index+where >=0 && m_index+where<=m_count-1){
		m_index+=where;
		if(m_index==0)
			ui->buttonPrev->setVisible(false);
		else
			ui->buttonPrev->setVisible(true);

		if(m_index==m_count-1){
			ui->buttonNext->setText("Start using CMP!");
			//m_videoWidget.widget()->setVisible(false);
			ui->labelMovie->setVisible(true);
		}
		else{
			ui->buttonNext->setVisible(true);
			ui->labelMovie->setVisible(false);
			//m_videoWidget.widget()->setVisible(true);
		}

		ui->stackedWidget->setCurrentIndex(m_index);

		if(m_index==0){
			ui->buttonNext->setText("Step 2: Search Modes");
		}
		else if(m_index==1){
			ui->buttonPrev->setText("Step 1: Starting");
			ui->buttonNext->setText("Step 3: Organizing circles");
		}
		else if(m_index==2){
			ui->buttonPrev->setText("Step 2: Search Modes");
			ui->buttonNext->setText("Step 4: Misc.");
		}
		else if(m_index==3){
			ui->buttonPrev->setText("Step 3: Organizing circles");
		}


		QString videoSource = Tools::resourcesLocation() + "/Help/video" + QString::number(m_index+1);
#ifdef Q_OS_WIN
		videoSource+= ".wmv";
#endif

#ifdef Q_OS_MAC
		videoSource+= ".mov";
#endif

		m_videoWidget.setSource(videoSource);
		m_videoWidget.play();
	}
}

void DialogHelp::fixTextSizes()
{
	QString style = styleSheet();
	style.replace("font-size:16pt","font-size:18pt");
	style.replace("font-size:10pt","font-size:12pt");
	setStyleSheet(style);

	Tools::fixTextBrowserSizes(ui->textBrowser);
	Tools::fixTextBrowserSizes(ui->textBrowser_2);
	Tools::fixTextBrowserSizes(ui->textBrowser_3);
	Tools::fixTextBrowserSizes(ui->textBrowser_4);
}


void DialogHelp::on_opt_dontShowHelp_toggled(bool checked)
{
	Settings::instance()->DONT_SHOW_HELP = checked;
}

void DialogHelp::onRejected()
{
	qDebug() << "hide";
	m_videoWidget.stop();
}
