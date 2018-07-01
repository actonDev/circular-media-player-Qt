#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "librarysearch.h"
#include "internetsearch.h"
#include "myscene.h"
#include "historysearch.h"
#include "lyrics.h"
#include "menuthemeaction.h"
#include "tools.h"
#include "lastfmview.h"

#include "lastfm.h"
#include "node.h"
#include "graph.h"
#include "player.h"
#include "myview.h"
#include "settings.h"
#include "mysplitter.h"



MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QFile file(Tools::resourcesLocation()+"testFile");
	if(!file.open(QFile::ReadWrite)){
		QMessageBox::warning(this, "Warning","You do not have the rights to write files in the directory you are running the application from. \n" \
							 "Please do not put the program in folders like 'Program files'.\n" \
							 "Put in a folder like 'User/', 'Desktop/' , 'Documents/'.");
	}
	else{
		file.close();
		file.remove();
	}

	setWindowTitle("Circular Media Player "+ Settings::instance()->version);

#ifndef HAVE_TAGLIB
	ui->menuLibrary->setDisabled(true);
#endif

	QGraphicsScene *p_scene = new QGraphicsScene(this);
	//	MyScene *p_scene = new MyScene(this);
	p_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	MyView *p_myView = new MyView(this);
	p_myView->setScene(p_scene);
	LibrarySearch *p_lib = new LibrarySearch(this);
	InternetSearch *p_internet = new InternetSearch(this);
	LastFmView *p_lastFmView = new LastFmView(this);
	Lyrics *p_lyrics = new Lyrics(this);
	HistorySearch *p_history = new HistorySearch(this);
	Graph *p_graph = new Graph(this, p_scene, p_myView);
	Node::setGraph(p_graph);
	p_myView->setGraph(p_graph);
	p_graph->setLibrary(p_lib);
	p_player = new Player;
	p_player->setObjectName("Player");

	//####################################
	//SETTING LAYOUT
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	p_splitter = new MySplitter(this);
	p_splitter->setObjectName("mainSplitter");
	p_splitter->setContentsMargins(0, 0, 0, 0);

	p_tabWidget = new QTabWidget(this);
	p_tabWidget->setObjectName("tabWidget");
	p_tabWidget->setContentsMargins(0, 0, 0, 0);
	p_tabWidget->addTab(p_internet, "Online");
	p_tabWidget->addTab(p_lastFmView, "Last.fm");
	p_tabWidget->addTab(p_lib,"Library");
	p_tabWidget->addTab(p_history, "History");
	p_tabWidget->addTab(p_lyrics, "Lyrics");

	connect(p_splitter, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(onSplitterOrientationChange(Qt::Orientation)));
	p_splitter->setOrientation(Qt::Vertical);
	//	if(true){
	for(int i=0; i<p_tabWidget->count(); i++){
		QWidget *widget = p_tabWidget->widget(i);
		widget->setMinimumWidth(200);
	}
	//	}

	p_splitter->addWidget(p_tabWidget);
	p_splitter->addWidget(p_myView);
	QList<int> sizes;
	sizes.append(100);
	sizes.append(200);
	p_splitter->setSizes(sizes);
	layout->addWidget(p_splitter);

	//TRANSPORT BAR
	//adding to the toolbar
	layout->addWidget(p_player->toolbar());
	ui->centralWidget->setLayout(layout);

	ui->actionSet_playing_node_as_root->setChecked(Settings::instance()->SET_PLAYING_AS_ROOT);
	//ENDED LAYOUT
	//##################################
	// ...
	// CONNECTIONS


	connect(p_myView, SIGNAL(kbPlayPause()), p_player, SLOT(onPlayPauseTriggered()));
	connect(p_myView, SIGNAL(kbPrevious()), p_player, SLOT(onPreviousTriggered()));
	connect(p_myView, SIGNAL(kbNext()), p_player, SLOT(onNextTriggered()));

	connect(p_player, SIGNAL(playingStateChanged(bool)), this, SLOT(setPlayingState(bool)));


	connect(this, SIGNAL(newNodeSignal()), p_graph, SLOT(createNode()));
	connect(p_graph, SIGNAL(statusInfoChanged(QString)), this, SLOT(statusBarSlot(QString)));
	connect(p_lib, SIGNAL(addedFile(QString)), this, SLOT(statusBarSlot(QString)));
	//	connect(Settings::instance(), SIGNAL(infoSignal(QString)), this, SLOT(statusBarSlot(QString)));
	connect(LastFm::instance(), SIGNAL(infoSignal(QString)), this, SLOT(statusBarSlot(QString)));
	connect(ui->actionPopulate_database, SIGNAL(triggered()), p_lib, SLOT(populateDatabase()));
	connect(ui->actionClear_database, SIGNAL(triggered()), p_lib, SLOT(clearDatabase()));
	connect(p_graph, SIGNAL(playingNodeChanged()), p_player, SLOT(updatePlayingNode()));
	connect(p_player, SIGNAL(currentInfoChanged(QString,QString,QString)), p_history, SLOT(addToHistory(QString,QString, QString)));
	connect(p_player, SIGNAL(currentInfoChanged(QString,QString,QString)), p_lyrics, SLOT(searchFor(QString,QString)));
	connect(p_player, SIGNAL(currentInfoChanged(QString,QString,QString)), this, SLOT(onPlayingInfoChanged(QString,QString)));
	connect(p_internet, SIGNAL(makeVisible()), this, SLOT(onlineSearchMakeVisible()));

	//connecting: artist, title, source
	connect(p_lib, SIGNAL(doubleCliked(QString,QString,QString)), p_graph, SLOT(createAndPlayNode(QString,QString,QString)));
	connect(p_internet, SIGNAL(doubleCliked(QString,QString,QString)), p_graph, SLOT(createAndPlayNode(QString,QString,QString)));
	connect(p_history, SIGNAL(doubleCliked(QString,QString,QString)), p_graph, SLOT(createAndPlayNode(QString,QString,QString)));
	connect(p_lastFmView, SIGNAL(doubleCliked(QString,QString,QString)), p_graph, SLOT(createAndPlayNode(QString,QString,QString)));

	//##################################
	//...
	//creating theme entries
	//...
	QMenu *themeMenu = ui->menuSelect_theme;
	//	QDir dir(QApplication::applicationDirPath() + "/themes");
	QDir dir(Tools::resourcesLocation()+"/Themes");
	dir.setFilter( QDir::Dirs | QDir::NoSymLinks);
	QStringList dirListing= dir.entryList(QDir::NoDotAndDotDot | QDir::AllDirs);
	for(int i=0; i<dirListing.count(); i++){
		qDebug() << "entry: " << dirListing.at(i);
		MenuThemeAction *action = new MenuThemeAction(0);
		action->setCheckable(true);
		action->setText(dirListing.at(i));
		themeMenu->addAction(action);
	}
	connect(ui->menuSelect_theme, SIGNAL(aboutToShow()), this, SLOT(onSelectThemeShow()));


	Settings::instance()->loadPlaylist();

	if(!Settings::instance()->GEOMETRY.isNull())
		setGeometry(Settings::instance()->GEOMETRY);

	//testing purposes
	//Node *nodeMp3 = new Node("artist mp3", "title mp3", QApplication::applicationDirPath() +"/test.mp3");
	//Node *nodeWav = new Node("artist wav", "title wav", QApplication::applicationDirPath() +"/test.wav");

	//	m_dialogHelp.setWindowFlags(Qt::Tool);
}

MainWindow::~MainWindow()
{
	qDebug() << "mainwindow DESTRUCTOR";
	Settings::instance()->GEOMETRY = geometry();
	delete Settings::instance();
	delete p_player;
	delete ui;
}

void MainWindow::showHelpDialog(bool startup)
{
	m_dialogHelp.resetHelp();
	if(startup){
		//		m_dialogHelp.setWindowFlags(Qt::SplashScreen);
		m_dialogHelp.exec();
	}

	else{

		//		m_dialogHelp.setWindowFlags(Qt::Tool);
		m_dialogHelp.show();
	}
}


void MainWindow::on_actionNew_Node_triggered()
{
	Graph::createEditNode();
}

void MainWindow::statusBarSlot(QString text)
{
	ui->statusBar->showMessage(text);
}

void MainWindow::on_actionSet_playing_node_as_root_toggled(bool arg1)
{
	qDebug() << "set playing as parent: " << arg1;
	Settings::instance()->SET_PLAYING_AS_ROOT = arg1;
}

void MainWindow::on_actionPreferences_triggered()
{
	m_preferences.show();
}

void MainWindow::on_actionBrowseForTheme_triggered()
{
	QString folder = QFileDialog::getExistingDirectory(0, tr("Select theme folder (a program restart is advised for all elements to be updated correctly)"));
	if(folder=="")
		return;
	qDebug() << "theme folder: " << folder;
	Settings::instance()->setThemeFolder(folder);
}

void MainWindow::on_actionUpdate_theme_triggered()
{
	Settings::instance()->updateThemeFolder();
}

void MainWindow::onSelectThemeShow()
{
	QMenu *menu = ui->menuSelect_theme;
	for(int i=0; i<menu->actions().count(); i++){
		QAction *action=menu->actions().at(i);
		action->setChecked(false);
		if(Settings::instance()->THEME == action->text())
			action->setChecked(true);
	}
}

void MainWindow::onPlayingInfoChanged(QString artist, QString title)
{
	setWindowTitle("Circular Media Player (Now playing: " + artist + " - " +title + ")" );
}

void MainWindow::onlineSearchMakeVisible()
{
	if(p_splitter->sizes().at(0) < 100){
		QList<int> sizes;
		sizes.append(100);
		sizes.append(200);
		p_splitter->setSizes(sizes);
	}
	p_tabWidget->setCurrentIndex(0);
}

void MainWindow::onSplitterOrientationChange(Qt::Orientation orientation)
{
	qDebug() << "orientation changed!";
	for(int i=0; i<p_tabWidget->count(); i++){
		QWidget *widget = p_tabWidget->widget(i);
		int top = widget->layout()->contentsMargins().top();

		if(orientation == Qt::Horizontal){
			widget->layout()->setContentsMargins(top,top,2,top);
		}
		else{
			widget->layout()->setContentsMargins(top,top,top,2);
		}
	}
}

void MainWindow::on_actionSave_Playlist_triggered()
{
	Settings::instance()->savePlaylist(false);
}

void MainWindow::on_actionLoad_Playlist_triggered()
{
	Settings::instance()->loadPlaylist(false, true);
}

void MainWindow::on_actionAdd_Playlist_triggered()
{
	Settings::instance()->loadPlaylist(false, false);
}

void MainWindow::on_actionAbout_triggered()
{
	m_dialogAbout.show();
}

void MainWindow::on_actionShortcuts_triggered()
{
	m_dialogShortcuts.show();
}

void MainWindow::on_actionShuffle_mode_toggled(bool arg1)
{
	Settings::instance()->PLAYING_SHUFFLE_MODE = arg1;
}

void MainWindow::on_actionShow_help_triggered()
{
	showHelpDialog();
}

void MainWindow::on_actionParty_mode_toggled(bool arg1)
{
	Settings::instance()->PARTY_MODE = arg1;
}
