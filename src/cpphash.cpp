#include "cpphash.h"
#include "c_cpphash.h"
#include <QString>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QHash>
#include <QFile>

#include <libsailfishsilica/silicatheme.h>

class CppHash
{
public:
	CppHash();

	static CppHash* instance();

	bool parse_emoji_json(QString path);

	int find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header);
public:
	QByteArray              buffer; //for paste image path in discout lib
	QHash<QString, QString> emoji;  // emoji hash short_name - image_name
	int h0,h1,h2,h3,h4,h5;
};

#define INCRASE_FONT 6

CppHash::CppHash()
{
	if(!parse_emoji_json( QString("%0/emoji.json").arg(EMOJI_PATH) ))
		qCritical() << "Cant read emoji.json file! Path:" << QString("%0/emoji.json").arg(EMOJI_PATH);

	h0 = Silica::Theme::instance()->fontSizeSmall();
	h5 = h0 + INCRASE_FONT;
	h4 = h5 + INCRASE_FONT;
	h3 = h4 + INCRASE_FONT;
	h2 = h3 + INCRASE_FONT;
	h1 = h2 + INCRASE_FONT;
}

CppHash *CppHash::instance()
{
	static CppHash *cpphash = new CppHash();
	return cpphash;
}

bool CppHash::parse_emoji_json(QString path)
{// parse emoji json and add it to hash
	qDebug() << "Start parse emoji.json file";
//	path = QString().arg(EMOJI_PATH);
	QFile file(path);
	file.open(QFile::ReadOnly);
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

	if (!error.errorString().isEmpty())
		qWarning() << error.errorString();
	if (doc.isEmpty())
		return false;
	if (!doc.isArray())
		return false;

	QJsonArray ar = doc.array();
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject current = ar[i].toObject();
		QString image_name = current["image"].toString();
		QString image = image_name.replace(".png",".svg");
		QRegExp re(".*[a-z]+\\-[a-z]+.*");
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		for( int j = 0; j < short_names.size(); j++)
		{
			QString short_name = short_names[j].toString();
			if( re.indexIn(short_name) != -1 )
				short_name.replace("-","_");
			emoji.insert( short_name, image );
		}
	}

	qDebug() << "All done!";
	return true;
}

int CppHash::find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header)
{
	switch(*header) {
	case 1:
		(*header) = h1;
		break;
	case 2:
		(*header) = h2;
		break;
	case 3:
		(*header) = h3;
		break;
	case 4:
		(*header) = h4;
		break;
	case 5:
		(*header) = h5;
		break;
	default:
		(*header) = h0;
		break;
	}

	QString short_name( QByteArray(emoji_short_name,size) );
	QHash<QString, QString>::iterator it =  emoji.find(short_name);
	if ( it != emoji.end() )
	{
		QString path_pic = QStringLiteral("%0/svg/%1").arg(EMOJI_PATH).arg(it.value());
		buffer = path_pic.toLocal8Bit();
		*picture_path = buffer.data();
		return path_pic.size();
	}
	else
		qDebug() << "No Emoji found: " << short_name;
	return 0;
}


// C part
int find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header)
{
	return CppHash::instance()->find_emoji(emoji_short_name,size,picture_path,header);
}
