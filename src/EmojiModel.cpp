#include "EmojiModel.h"

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegExp>
#include <QDebug>
#include <SettingsContainer.h>

#define lastUsed QStringLiteral("Last Used")

QVector<EmojiModel::ItemPtr>  EmojiModel::m_items;
//QVector<EmojiModel::ItemPtr>  EmojiModel::m_usedItems;
QVector< QPair<QString, EmojiModel::Category> > EmojiModel::m_categories;

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
{
	connect(SettingsContainer::getInstance(), &SettingsContainer::usedReactionsChanged,
	        this, &EmojiModel::onUsedReactionsChanged );
	loadEmoji();
	onUsedReactionsChanged();
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
	return m_items.size() + m_usedItems.size();
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
	int num = index.row();
	if( num < 0 || num >= m_items.size() )
	{
		num -= m_items.size();
		if( num < 0 || num >= m_usedItems.size() )
			return QModelIndex();
		switch (role) {
		case RoleName:
			return m_usedItems[num]->name;
			break;
		case RoleImage:
			return m_usedItems[num]->image;
			break;
		case RoleCategory:
			return lastUsed;
		case RoleType:
			return QVariant::fromValue<ItemType>(m_usedItems[num]->type);
		default:
			break;
		}
		return QVariant();
	}

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
	QStringList result;
	for(auto v : m_categories)
		result << v.first;
	return result;
}

QString EmojiModel::categoryIcon(QString category) const
{
	auto search = std::find_if(m_categories.begin(), m_categories.end(), [category](QPair<QString,Category> current){
	    if( current.first == category )
	        return true;
	    return false;
    });
	if( search == m_categories.end() )
		return QString();
	Category c = search->second;
	if( c.begin == m_items.size() )
	{
		return QStringLiteral("%0/svg/1f553.svg").arg(EMOJI_PATH);
		//m_usedItems[c.begin - m_items.size()]->image;
	}
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
	if(!m_items.empty())
		return;
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
	QVector<QString> categoriesOrder;

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
			categoriesOrder.push_back(emoji_category);
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
			auto search = std::find_if(emoji->begin(), emoji->end(), [image_name](ItemPtr current) -> bool {
			    if( current->image == image_name )
			    {
//			        qDebug() << QStringLiteral("Catch emoji double! %0 with image %1").arg(current->name).arg(image_name);
			        return true;
		        }
			    return false;
		    } );
			if( search == emoji->end() )
				emoji->push_back( item );
//			else
//				qDebug() << QStringLiteral("Catch emoji double! %0 with image %1").arg(item->name).arg(image_name);
		}
	}
	qDebug() << "All done!";

	for( auto category : categoriesOrder )
	{
		ItemPtr item(new Item);
		item->type = ItemTypeCategory;
		item->name = category;
		item->category = category;
		m_items.push_back(item);
		Category c;
		c.begin = m_items.size();
		QVector<ItemPtr>* emoji = categories[category];
		for(int i = 0 ; i < emoji->size(); i++ )
		{
			m_items.push_back( emoji->at(i) );
		}
		c.end = m_items.size();
		m_categories.push_back( QPair<QString,Category>(category, c) );
		delete emoji;
	}
	endResetModel();
	emit categoriesChanged();
}

void EmojiModel::onUsedReactionsChanged()
{
	QStringList short_names = SettingsContainer::getInstance()->usedReactions();
	int maxAliases = SettingsContainer::getInstance()->usedReactionsCount();
//	if( m_usedItems.empty() || (m_usedItems.size() + 1 < short_names.size()) )
	if(!short_names.isEmpty())
	{
		// TODO do not reset model
//		beginResetModel();
		beginInsertRows( QModelIndex(), m_items.size(), m_items.size() + short_names.size() - 1 );
		m_usedItems.clear();
		for(auto name : short_names)
		{
			auto search = std::find_if(m_items.begin(), m_items.end(), [name](ItemPtr current) {
			    if( current->name == name )
			        return true;
			    return false;
		    });
			if( search != m_items.end() )
				m_usedItems.append(*search);
		}
		endInsertRows();
//		endResetModel();
	}
//	else {
//		// TODO do not reset model
//		//beginResetModel();
//		if( m_usedItems.size() == maxAliases )
//			m_usedItems.pop_front();
//		QString name = short_names.back();
//		auto search = std::find_if(m_items.begin(), m_items.end(), [name](ItemPtr current) {
//		    if( current->name == name )
//		        return true;
//		    return false;
//	    });
//		if( search != m_items.end() )
//			m_usedItems.append(*search);
//		emit dataChanged(
//					index( m_items.size(), 0 ),
//					index( m_items.size() + m_usedItems.size() -1 ),
//					QVectorInt() << RoleImage << RoleImage << RoleName )
//		//endResetModel();
//	}
	if(m_usedItems.isEmpty())
		return;
//	QString lastUsed = QStringLiteral("Last Used");
	auto search = std::find_if(m_categories.begin(), m_categories.end(), [=](QPair<QString,Category> current){
		if( current.first == lastUsed ) {
			return true;
		}
		return false;
	});
	if( search == m_categories.end() )
	{
		Category c;
		c.begin = m_items.size();
		c.end = c.begin + m_usedItems.size();
		m_categories.push_front( QPair<QString,Category>(lastUsed,c) );
	}
	else
	{
		m_categories[0].second.end = m_categories[0].second.begin + m_usedItems.size();
	}
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
		auto search = std::find_if(m_emojiModel->m_categories.begin(),
		                           m_emojiModel->m_categories.end(),
		                           [=](QPair<QString,EmojiModel::Category> current){
			if( current.first == m_category )
				return true;
			return false;
		});
		m_categoryIndex = search->second;
	}
}
