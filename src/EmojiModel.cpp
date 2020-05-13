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
	loadEmoji();
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
	return m_items.size();
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
	int num = index.row();
	if( num < 0 || num >= m_items.size() )
		return QVariant();

	switch (role) {
	case RoleName:
		return m_items[num]->name;
		break;
	case RoleImage:
		return m_items[num]->image;
		break;
	case RoleCategory:
		return m_items[num]->category;
	case RoleType:
		return QVariant::fromValue<ItemType>(m_items[num]->type);
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
	    {RoleCategory, "role_category"},
	    {RoleType,     "role_type"}
	};
	return names;
}

QStringList EmojiModel::categories() const
{
	return m_categories.keys();
}

QString EmojiModel::categoryIcon(QString category) const
{
	auto search = m_categories.find(category);
	if( search == m_categories.end() )
		return QString();
	Category c = search.value();
	return m_items[c.begin]->image;
}

//void EmojiModel::setMattermost(MattermostQt *mattermost)
//{
//	m_mattermost = mattermost;
//}

//MattermostQt *EmojiModel::getMattermost() const
//{
//	return m_mattermost;
//}

//void EmojiModel::setCategory(const QString &name)
//{
//	m_category = name;
//	emit categoryChanged();
//	loadEmoji();
//}

//QString EmojiModel::category() const
//{
//	return m_category;
//}

void EmojiModel::loadEmoji()
{
	beginResetModel();
	m_items.clear();
	m_categories.clear();

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

	QMap<QString, QVector<ItemPtr>* > categories;

	QJsonArray ar = doc.array();
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject current = ar[i].toObject();
		QString emoji_category = current["category"].toString();

		QVector<ItemPtr> *emoji = nullptr;
		QMap<QString, QVector<ItemPtr>* >::iterator search = categories.find(emoji_category);
		if( categories.end() != search )
		{
			emoji = search.value();
		}
		else
		{
			emoji = new QVector<ItemPtr>();
			categories.insert( emoji_category, emoji );
		}

		QString image_name = emji_path + current["image"].toString();
		QRegExp re(".*[a-z]+\\-[a-z]+.*");
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		for( int j = 0; j < short_names.size(); j++)
		{
			QString short_name = short_names[j].toString();
			ItemPtr item(new Item);
			item->type  = ItemTypeEmoji;
			item->image = image_name;
			item->name  = short_name;
			item->category = emoji_category;
			emoji->push_back( item );
		}
	}
	qDebug() << "All done!";

	for( auto it = categories.begin(); it != categories.end(); it++ )
	{
		ItemPtr item(new Item);
		item->type = ItemTypeCategory;
		item->name = it.key();
		item->category = it.key();
		m_items.push_back(item);
		Category c;
		c.begin = m_items.size();
		QVector<ItemPtr>* emoji = it.value();
		for(int i = 0 ; i < emoji->size(); i++ )
		{
			m_items.push_back( emoji->at(i) );
		}
		c.end = m_items.size();
		m_categories.insert(it.key(), c );
		delete emoji;
	}
	endResetModel();
	emit categoriesChanged();
}

EmojiProxyCategory::EmojiProxyCategory(QObject *parent)
    : QAbstractProxyModel(parent)
{

}

QModelIndex EmojiProxyCategory::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();
	if(!m_emojiModel || m_category.isEmpty() )
		return QModelIndex();
	if( m_categoryIndex.begin == -1 )
		return QModelIndex();
//		m_categoryIndex = m_emojiModel->m_categories.value(m_category);
	return m_emojiModel->createIndex( proxyIndex.row() + m_categoryIndex.begin, 0 );
}

QModelIndex EmojiProxyCategory::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();
	QString category = sourceIndex.data(EmojiModel::RoleCategory).toString();
	if( category != m_category )
		return QModelIndex();
	if( m_categoryIndex.begin == -1 )
		return QModelIndex();
	int row = sourceIndex.row();
//		m_categoryIndex = m_emojiModel->m_categories.value(m_category);
	if( row < m_categoryIndex.begin || row >= m_categoryIndex.end )
		return QModelIndex();
	return createIndex( row - m_categoryIndex.begin, 0 );
}

void EmojiProxyCategory::setSourceModel(QAbstractItemModel *sourceModel)
{
	m_emojiModel = qobject_cast<EmojiModel*>(sourceModel);
	QAbstractProxyModel::setSourceModel(sourceModel);

	updateCategoryIndex();
}

QModelIndex EmojiProxyCategory::index(int row, int col, const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return createIndex(row,col, nullptr);
}

QModelIndex EmojiProxyCategory::parent(const QModelIndex &child) const
{
	Q_UNUSED(child)
	return QModelIndex();
}

int EmojiProxyCategory::rowCount(const QModelIndex &parent) const
{
	if( m_categoryIndex.begin != -1 )
		return m_categoryIndex.end - m_categoryIndex.begin;
	return 0;
}

int EmojiProxyCategory::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant EmojiProxyCategory::data(const QModelIndex &index, int role) const
{
	QModelIndex sourceIndex = mapToSource(index);
	if(sourceIndex.isValid())
		return sourceIndex.data(role);
	return QVariant();
}

QString EmojiProxyCategory::category() const
{
	return m_category;
}

void EmojiProxyCategory::setCategory(const QString &in_category)
{
	m_category = in_category;
	emit categoryChanged();

	updateCategoryIndex();
}

void EmojiProxyCategory::updateCategoryIndex()
{
	if(m_emojiModel && !m_category.isEmpty()) {
		m_categoryIndex = m_emojiModel->m_categories.value(m_category);
	}
}
