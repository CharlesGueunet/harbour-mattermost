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
protected:
	QHash<QString, QString> emoji;  // emoji hash short_name - image_name
};


CppHash::CppHash()
{
	buffer = nullptr;
}

CppHash *CppHash::instance()
{
	static CppHash *cpphash = new CppHash();
	return cpphash;
}

bool CppHash::parse_emoji_json(QString path)
{// parse emoji json and add it to hash
	qDebug() << "Start parse emoji.json file";
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
		QString unicode = current["unified"].toString();
		//unicode.insert(0, "\\u");
//		QString une = toUnicode(unicode);
		//qrc:/emoji/64/0023.png
//		QString short_name = current["short_name"].toString();
		QString image_name = current["image"].toString();
		QString image = image_name;
//		emoji.insert( short_name, image );
//		QByteArray utf8_name = short_name.toLatin1();
//		QByteArray utf8_image = image_name.toLatin1();
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		for( int j = 0; j < short_names.size(); j++)
			emoji.insert( short_names[j].toString(), image );
	}

	qDebug() << "All done!";
	return true;
}


// C part
int find_emoji(const char *emoji, int size, char **picture_path, int *header)
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

	QString comp("smile");
	QString w( QByteArray(emoji,size) );
	qDebug() << "CheckEmoji: " << w;
	if( comp.compare(w) == 0 )
	{
		QString path_pic = QString("%0/svg/1f604.svg").arg(EMOJI_PATH);
		CppHash::instance()->buffer = path_pic.toLocal8Bit();
		*picture_path = CppHash::instance()->buffer.data();
		return path_pic.size();
	}
	return 0;
}
