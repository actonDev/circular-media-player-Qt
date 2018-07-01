#include <QApplication>
#include "mainwindow.h"

#include "tools.h"
#include "movieplayer.h"
#include "updatecheck.h"
#include "settings.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Home");
//	QCoreApplication::setOrganizationDomain("mysoft.com");
	QCoreApplication::setApplicationName("Circular Media Player");
	//initializing options here
//		a.addLibraryPath(a.applicationDirPath() + "/dll");

	#ifdef Q_OS_WIN
	a.addLibraryPath(QApplication::applicationDirPath() + "/dll");
//	a.removeLibraryPath("C:/QtSDK/Desktop/Qt/4.7.4/mingw/plugins");
//	a.removeLibraryPath("C:/QtSDK/Desktop/Qt/4.8.1/mingw/plugins");
	a.removeLibraryPath("C:/Qt/Qt5.0.0/5.0.0/msvc2010/plugins");
	a.removeLibraryPath("C:/SDK/Qt5-x32/plugins");
	#endif
	qDebug() << "LIBRARY PATHS = ";
	for(int i=0; i<a.libraryPaths().length(); i++){
		qDebug()<< a.libraryPaths().at(i);
	}

	QString styleFile = Settings::instance()->themeFolder() + "/style.css";
	QString styleFolder = Settings::instance()->themeFolder();
	QString styleSheet = Tools::styleSheetFromFile(styleFile, styleFolder);
	a.setStyleSheet(styleSheet);

	MainWindow w;
	w.show();

	if(!Settings::instance()->DONT_SHOW_HELP)
		w.showHelpDialog(true);

	//check for admin rights




	return a.exec();
}
