#include "mainwindow.hpp"
#include "common.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	qRegisterMetaType<Common::SETTINGS>("Common::SETTINGS");
	qRegisterMetaType<Common::LOGENTRY>("Common::LOGENTRY");
	qRegisterMetaType<Common::LOGLIST>("Common::LOGLIST");

//	QIcon::setThemeSearchPaths({ "icons" });
//	QIcon::setThemeName("gnome");

	a.setApplicationName("K-Auto-Renamer");
	a.setOrganizationName("Łukasz \"Kuszki\" Dróżdż");
	a.setOrganizationDomain("https://github.com/Kuszki");
	a.setApplicationVersion("1.0");

	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&qtTranslator);

	QTranslator baseTranslator;
	baseTranslator.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&baseTranslator);

	QTranslator appTranslator;
	appTranslator.load("K-Auto-Renamer_" + QLocale::system().name());
	a.installTranslator(&appTranslator);

	MainWindow w;
	w.show();

	return a.exec();
}
