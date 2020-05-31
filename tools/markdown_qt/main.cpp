#include <QDebug>
#include <QApplication>
#include <mdMain.h>
#include <QQuickView>
//#include "quckobject.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
//	if(!SettingsContainer::getInstance()->debug())
//		return; // TODO test no stdout
	// TODO write log to databse in separate thread
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type) {
	case QtDebugMsg:
		fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtInfoMsg:
		fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtWarningMsg:
		fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtCriticalMsg:
		fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		break;
	case QtFatalMsg:
		fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
		//abort();
	}
}

int main(int argc, char *argv[])
{
	qInstallMessageHandler( myMessageOutput );

	QApplication app(argc, argv);

//	qmlRegisterType<QuickObject>("ru.sashikknox", 1, 0, "QuickObject");

	mdMain mWindow;
	mWindow.show();

	return app.exec();
}
