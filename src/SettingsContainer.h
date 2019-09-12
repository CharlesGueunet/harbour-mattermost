#ifndef SETTINGSCONTAINER_H
#define SETTINGSCONTAINER_H

#include <QObject>
#include <QCoreApplication>
#include <QtQuick>
#include <QString>
#include <QGradient>
#include "MattermostQt_gobal.h"
#include "MattermostQt.h"


class SettingsContainer : public QObject
{
	Q_OBJECT

	/** download image automaticly, when its size lower than autoDownloadImageSize */
	MT_PROPERTY(int,   autoDownloadImageSize);
	MT_PROPERTY(bool,  showBlobs);
	MT_PROPERTY(float, blobOpacity);
	MT_PROPERTY(float, pageMargin);
	MT_PROPERTY(bool,  formatedText); // show Markdown text
	MT_PROPERTY(bool,  debug);

public:
	explicit SettingsContainer(QObject *parent = nullptr);

	static SettingsContainer *getInstance();

	/**
	 * @brief strToSingleLine
	 * make one line string from multiline
	 * @param in multiline string
	 * @return  one line string
	 */
	Q_INVOKABLE static QString strToSingleLine(const QString &in);

	/**
	 * @brief resetToDefault
	 * reset settings to defalt values
	 */
	Q_INVOKABLE void resetToDefault();

	/**
	 * @brief asJsonObject
	 * write all settings to QJsonObject
	 * @return
	 */
	QJsonObject asJsonObject() const;

	/**
	 * @brief fromJsonObject
	 * read settings from json object
	 * @param object
	 */
	void fromJsonObject(const QJsonObject &settings);
Q_SIGNALS:
	void settingsChanged();
};

// Second, define the singleton type provider function (callback).
static QObject *SettingsContainer_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
//	engine->
//	MattermostQt *m = engine->findChild<MattermostQt*>();
//	if(SettingsContainerSingleton.data())
//	SettingsContainer *settings = new SettingsContainer();
//	if(m)
//		m->setSettingsContainer(settings);
	return SettingsContainer::getInstance();
}

#endif // SETTINGSCONTAINER_H
