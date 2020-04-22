#include "jsonmodel.h"
#include <cpphash.h>

JsonModel::JsonModel(QJsonDocument &doc, QObject *parent)
    : QAbstractItemModel(parent)
{
}

int JsonModel::rowCount(const QModelIndex &parent) const
{
	return CppHash::instance()->count();
}

int JsonModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant JsonModel::data(const QModelIndex &index, int role) const
{
	if( !index.isValid() ) {
		return QVariant();
	}

	return QVariant("Unsupported");
}

QHash<int, QByteArray> JsonModel::roleNames() const
{
	QHash<int, QByteArray> names = {
	    {ImageName, "role_iamge_name"},
	    {ImagePath, "role_image_path"},
	    {Section,   "role_section_name"},
	    {Type,      "role_item_type"},
	    {EmojiName, "role_emoji_name"}
	};
	return names;
}
