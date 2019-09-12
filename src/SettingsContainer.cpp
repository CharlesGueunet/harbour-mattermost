#include "SettingsContainer.h"
#include <sailfishapp.h>
#include <libsailfishsilica/silicatheme.h>

#define SET_SETTINGS_PROPERTY(_t, _n) MT_SET_PROPERTY_2(SettingsContainer, _t, _n, settingsChanged)

SET_SETTINGS_PROPERTY(int,   autoDownloadImageSize)
SET_SETTINGS_PROPERTY(bool,  showBlobs)
SET_SETTINGS_PROPERTY(float, blobOpacity)
SET_SETTINGS_PROPERTY(float, pageMargin)
SET_SETTINGS_PROPERTY(bool, formatedText)
SET_SETTINGS_PROPERTY(bool,  debug)

SettingsContainer::SettingsContainer(QObject *parent) : QObject(parent)
{
	resetToDefault();
}

SettingsContainer *SettingsContainer::getInstance()
{
	static SettingsContainer *singleton = new SettingsContainer();
	qDebug()<< "Get instance" << qlonglong(singleton);
	return singleton;
}

QString SettingsContainer::strToSingleLine(const QString &in)
{
	QString r = in;
	r= r.replace(QRegExp("(\\n|\\r\\n)")," ");
	return r;
}

void SettingsContainer::resetToDefault()
{
	m_autoDownloadImageSize = 512;
	m_showBlobs             = true;
	m_blobOpacity           = 0.7;
	m_formatedText          = true;
	m_pageMargin            = Silica::Theme::instance()->paddingMedium();
	m_debug                 = false;
}

#define ADD_VALUE(x) settings[#x] = x
#define FROM_VALUE(x,func) x = settings[#x].func

QJsonObject SettingsContainer::asJsonObject() const
{
	QJsonObject settings;
	ADD_VALUE(m_autoDownloadImageSize);
	ADD_VALUE(m_showBlobs);
	ADD_VALUE(m_blobOpacity);
	ADD_VALUE(m_formatedText);
	ADD_VALUE(m_pageMargin);
	return settings;
}

void SettingsContainer::fromJsonObject(const QJsonObject &settings)
{
	FROM_VALUE(m_autoDownloadImageSize,toInt());
	FROM_VALUE(m_showBlobs,toBool());
	FROM_VALUE(m_blobOpacity,toDouble());
	FROM_VALUE(m_formatedText,toBool());
	FROM_VALUE(m_pageMargin,toDouble());
}
