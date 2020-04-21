#include <QDebug>
#include <QApplication>
#include <mdMain.h>
#include <QQuickView>
//#include "quckobject.h"

int main(int argc, char **argv )
{
	QApplication app(argc, argv);

//	qmlRegisterType<QuickObject>("ru.sashikknox", 1, 0, "QuickObject");

	mdMain mWindow;
	mWindow.show();

	return app.exec();
}
