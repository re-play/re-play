#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickview>
#include <QQmlContext>
#include <QFontDatabase>
#include <QtQml/qqml.h>
#include <QQmlListProperty>

#include <core/audio/audiodecoder.h>
#include <core/audio/speakers.h>
#include <core/audio/speaker.h>

#include <core/video/videodecoder.h>
#include <core/video/videocanvas.h>

#include <core/videoreader.h>
#include <core/videostream.h>

#include <core/decoder.h>

using namespace RePlay::Core;
using namespace RePlay::Core::Audio;
using namespace RePlay::Core::Video;
using namespace RePlay::Core::Timing;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);

	QQmlApplicationEngine engine;

	qmlRegisterSingletonType(QUrl("qrc:///resources/fonts/Fonts.qml"), "replay.fonts", 1, 0, "Fonts");
	qmlRegisterSingletonType(QUrl("qrc:///resources/fonts/Icons.qml"), "replay.fonts", 1, 0, "Icons");


	qmlRegisterType<AudioDecoder>("replay.core.audio", 1, 0, "AudioDecoder");
	qmlRegisterType<Speaker>("replay.core.audio", 1, 0, "Speaker");
	qmlRegisterUncreatableType<Speakers>("replay.core.audio", 1, 0, "Speakers", "Enum type");

	qmlRegisterType<VideoDecoder>("replay.core.video", 1, 0, "VideoDecoder");
	qmlRegisterType<VideoCanvas>("replay.core.video", 1, 0, "VideoCanvas");

	qmlRegisterType<VideoReader>("replay.core", 1, 0, "VideoReader");
	qmlRegisterType<VideoStream>("replay.core", 1, 0, "VideoStream");
	qmlRegisterType<Clock>("replay.core.timing", 1, 0, "Clock");


	QQuickView *view = new QQuickView;
	view->setTitle(QStringLiteral("Re-play"));
	view->setSource(QUrl(QLatin1String("qrc:///main.qml")));
	view->setMinimumSize(QSize(600,400));
	view->show();

	return app.exec();
}
