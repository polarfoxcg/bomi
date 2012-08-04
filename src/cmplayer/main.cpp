#include "app.hpp"
#include "rootmenu.hpp"
#include <QtCore/QDebug>
#include "pref.hpp"
#include "videorenderer.hpp"
#include "global.hpp"
#include "mrl.hpp"
#include "avmisc.hpp"
#include "mainwindow.hpp"
#include <QtCore/QMetaObject>
#include <QtOpenGL/QGLWidget>
#include "playengine.hpp"
#include "recentinfo.hpp"
#include <QtOpenGL/QGLContext>
#include <QtGui/QMessageBox>
#include <QtCore/QStringBuilder>
#include <sigar.h>

static bool checkOpenGL() {
	if (Q_LIKELY(QGLFormat::hasOpenGL() && (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0)))
		return true;
	QMessageBox::critical(0, "CMPlayer"
		, App::tr("CMPlayer needs OpenGL to render video. Your system has no OpenGL support. Exit CMPlayer."));
	return false;
}

int main(int argc, char **argv) {
	qRegisterMetaType<State>("State");
	qRegisterMetaType<Mrl>("Mrl");
	qRegisterMetaType<VideoFormat>("VideoFormat");
	QApplication::setAttribute(Qt::AA_X11InitThreads);

	App app(argc, argv);
	if (!checkOpenGL())
		return 1;
	const auto mrl = app.getMrlFromCommandLine();
	if (app.isUnique() && app.sendMessage("wakeUp")) {
		if (!mrl.isEmpty())
			app.sendMessage(_L("mrl ") % mrl.toString());
		return 0;
	}

	PlayEngine::obj = new PlayEngine;
	PlayEngine::obj->start();
	Pref::obj = new Pref;
	RecentInfo::obj = new RecentInfo;
	RootMenu::obj = new RootMenu;
	MainWindow *mw = new MainWindow;
	app.setMainWindow(mw);
	mw->show();
	if (!mrl.isEmpty())
		mw->openMrl(mrl);
	const int ret = app.exec();
	mw->exit();
	qDebug() << "gui loop end";
	PlayEngine::obj->wait();
	qDebug() << "playing thread finished";
	mw->unplug();
	qDebug() << "unplugged";
	delete mw;
	qDebug() << "main window deleted";
	delete PlayEngine::obj;
	qDebug() << "engine deleted";
	delete RecentInfo::obj;
	qDebug() << "recent deleted";
	delete Pref::obj;
	qDebug() << "pref deleted";
	delete RootMenu::obj;
	qDebug() << "menu deleted";
	return ret;
}
