#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QToolBar>
#include <QObject>
#include <QWheelEvent>
#include <QFileDialog>
#include <QIcon>
#include <QAction>

#include <QSplitter>
#include <QTabWidget>

#include "preferences.h"
#include "dialogabout.h"
#include "dialogshortcuts.h"
#include "dialoghelp.h"
#include "updatecheck.h"
class Player;
class MySplitter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Player *p_player;
	Preferences m_preferences;
	DialogAbout m_dialogAbout;
	DialogShortcuts m_dialogShortcuts;
	MySplitter *p_splitter;
	QTabWidget *p_tabWidget;
	UpdateCheck updater;
	DialogHelp m_dialogHelp;

//	Thumbnail

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	Ui::MainWindow *getUi(){
		return ui;
	}

	void showHelpDialog(bool startup = false);

protected:


private slots:
	void on_actionNew_Node_triggered();
	void statusBarSlot(QString text);
	void on_actionSet_playing_node_as_root_toggled(bool arg1);
	void on_actionPreferences_triggered();
	void on_actionBrowseForTheme_triggered();
	void on_actionUpdate_theme_triggered();
	void onSelectThemeShow();
	void onPlayingInfoChanged(QString artist, QString title);
	void onlineSearchMakeVisible();

	void onSplitterOrientationChange(Qt::Orientation orientation);

	void on_actionSave_Playlist_triggered();

	void on_actionLoad_Playlist_triggered();

	void on_actionAdd_Playlist_triggered();

	void on_actionAbout_triggered();

	void on_actionShortcuts_triggered();

	void on_actionShuffle_mode_toggled(bool arg1);

	void on_actionShow_help_triggered();

	void on_actionParty_mode_toggled(bool arg1);

signals:
	void newNodeSignal();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
