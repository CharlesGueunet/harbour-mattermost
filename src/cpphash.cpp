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

class CppHash
{
public:
	CppHash();

	static CppHash* instance();

	bool parse_emoji_json(QString path);

public:
	QByteArray              buffer; //for paste image path in discout lib
	QHash<QString, QString> emoji;  // emoji hash short_name - image_name
};


CppHash::CppHash()
{
	buffer = nullptr;

	if(!parse_emoji_json( QString("%0/emoji.json").arg(EMOJI_PATH) ))
		qCritical() << "Cant read emoji.json file! Path:" << QString("%0/emoji.json").arg(EMOJI_PATH);
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


// C part
int find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header)
{
	switch(*header) {
	case 1:
		(*header) = 54;
		break;
	case 2:
		(*header) = 42;
		break;
	case 3:
		(*header) = 32;
		break;
	case 4:
		(*header) = 24;
		break;
	case 5:
		(*header) = 20;
		break;
	default:
		(*header) = 32;
		break;
	}

	QString short_name( QByteArray(emoji_short_name,size) );
	QHash<QString, QString>::iterator it =  CppHash::instance()->emoji.find(short_name);
	if ( it != CppHash::instance()->emoji.end() )
	{
		QString path_pic = QStringLiteral("%0/svg/%1").arg(EMOJI_PATH).arg(it.value());
		CppHash::instance()->buffer = path_pic.toLocal8Bit();
		*picture_path = CppHash::instance()->buffer.data();
		return path_pic.size();
	}
	else
		qDebug() << "No Emoji found: " << short_name;
	return 0;
}
