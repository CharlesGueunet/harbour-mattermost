#include "EmojiModel.h"

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>
#include <QDebug>

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
	return m_emoji.size();
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
	int num = index.row();
	if( num < 0 || num >= m_emoji.size() )
		return QVariant();

	switch (role) {
	case RoleName:
		return m_emoji[num].first;
		break;
	case RoleImage:
		return m_emoji[num].second;
		break;
	case RoleCategory:
		return m_category;
	default:
		break;
	}

	return QVariant();
}

QHash<int, QByteArray> EmojiModel::roleNames() const
{
	QHash<int, QByteArray> names = {
	    {RoleName,     "role_name"    },
	    {RoleImage,    "role_image"   },
	    {RoleCategory, "role_category"}
	};
	return names;
}

//void EmojiModel::setMattermost(MattermostQt *mattermost)
//{
//	m_mattermost = mattermost;
//}

//MattermostQt *EmojiModel::getMattermost() const
//{
//	return m_mattermost;
//}

void EmojiModel::setCategory(const QString &name)
{
	m_category = name;
	emit categoryChanged();
	loadEmoji();
}

QString EmojiModel::category() const
{
	return m_category;
}

void EmojiModel::loadEmoji()
{
	beginResetModel();
	qDebug() << "Start parse emoji.json file";
	QString emji_path = QStringLiteral("%0/").arg(EMOJI_PATH);
	QFile file(QStringLiteral("%0/emoji.json").arg(EMOJI_PATH));
	file.open(QFile::ReadOnly);
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

	if (!error.errorString().isEmpty())
		qWarning() << error.errorString();
	if (doc.isEmpty())
		return ;
	if (!doc.isArray())
		return /*false*/;

	QJsonArray ar = doc.array();
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject current = ar[i].toObject();
		QString emoji_category = current["category"].toString();
		if( emoji_category != m_category )
			continue;

		QString image_name = emji_path + current["image"].toString();
		QRegExp re(".*[a-z]+\\-[a-z]+.*");
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		for( int j = 0; j < short_names.size(); j++)
		{
			QString short_name = short_names[j].toString();
			m_emoji.push_back( QPair<QString,QString>(short_name, image_name) );
		}
	}
	qDebug() << "All done!";
	endResetModel();
}
