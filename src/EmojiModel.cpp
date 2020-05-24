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
QVector< QPair<QString, EmojiModel::IndexRange> > EmojiModel::m_categories;

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
	if( num >= m_items.size() )
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
	auto search = std::find_if(m_categories.begin(), m_categories.end(), [category](QPair<QString,IndexRange> current){
	    if( current.first == category )
	        return true;
	    return false;
    });
	if( search == m_categories.end() )
		return QString();
	IndexRange c = search->second;
	if( c.begin >= m_items.size() )
	{
		return QStringLiteral("%0/svg/1f553.svg").arg(EMOJI_PATH);
		//m_usedItems[c.begin - m_items.size()]->image;
	}
	return m_items[c.begin]->image;
}

EmojiModel::ItemPtr EmojiModel::getItem(int row) const
{
	if( row >= 0 && row < m_items.size() )
		return m_items[row];
	else {
		row -= m_items.size();
		if( row >= 0 && row < m_usedItems.size() )
		{
			return m_usedItems[row];
		}
	}
	return ItemPtr();
}

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
		if( current.contains("visible") && current["visible"].toBool() == false )
			continue;
		// remove all Skin Tones from emoji keyboard and reactions page
		// as in official desktop applications
		if( emoji_category == QLatin1String("Skin Tones") )
			continue;

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
		int category_index = m_items.size();
		item->category_index = category_index;
		m_items.push_back(item);
		IndexRange c;
		c.begin = m_items.size();
		QVector<ItemPtr>* emoji = categories[category];
		for(int i = 0 ; i < emoji->size(); i++ )
		{
			ItemPtr current_emoji_item = emoji->at(i);
			current_emoji_item->category_index = category_index;
			m_items.push_back( current_emoji_item );
		}
		c.end = m_items.size();
		m_categories.push_back( QPair<QString,IndexRange>(category, c) );
		delete emoji;
	}
	endResetModel();
	emit categoriesChanged();
}

void EmojiModel::onUsedReactionsChanged()
{
	QStringList short_names = SettingsContainer::getInstance()->usedReactions();
	int maxAliases = SettingsContainer::getInstance()->usedReactionsCount();

	if(!short_names.isEmpty())
	{
		bool isInsert = true;
		int from = 0;
		// first check if it reaction is used before
		if( !m_usedItems.empty() ) {
			// get last added reaction from front of list
			QString last_used = short_names.front();
			for( int index = 0 ; index < m_usedItems.size(); index ++ ) {
				if( m_usedItems[index]->type != ItemTypeEmoji)
					continue;
				if( m_usedItems[index]->name == last_used ) {
					from = index;
					isInsert = false;
					break;
				}
			}
//			auto search = std::find_if(m_usedItems.begin(), m_usedItems.end(), [=](ItemPtr current) {
//				if( categoryItem->type = ItemTypeEmoji && current->name == last_used ) {
//					from = index;
//					isInsert = false;
//					return true;
//				}
//				index++;
//				return false;
//			});
		}
		if( isInsert )
			beginInsertRows( QModelIndex(), m_items.size(), m_items.size() + short_names.size() );
		else
			beginMoveRows( QModelIndex(), m_items.size() + from, m_items.size() + from, QModelIndex(), 0 );
		m_usedItems.clear();
		ItemPtr categoryItem(new Item);
		categoryItem->category = lastUsed;
		categoryItem->category_index = 0;
		categoryItem->image =  QStringLiteral("%0/svg/1f553.svg").arg(EMOJI_PATH);
		categoryItem->name = lastUsed;
		categoryItem->type = ItemTypeCategory;
		m_usedItems.push_back( categoryItem );
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
		if( isInsert )
			endInsertRows();
		else
			endMoveRows();
	}
	if(m_usedItems.isEmpty())
		return;

	auto search = std::find_if(m_categories.begin(), m_categories.end(), [=](QPair<QString,IndexRange> current){
		if( current.first == lastUsed ) {
			return true;
		}
		return false;
	});
	if( search == m_categories.end() )
	{
		IndexRange c;
		c.begin = m_items.size() + 1;
		c.end = c.begin + m_usedItems.size() - 1;
		m_categories.push_front( QPair<QString,IndexRange>(lastUsed,c) );
	}
	else
	{
		m_categories[0].second.end = m_categories[0].second.begin + (m_usedItems.size() - 1);
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
		                           [=](QPair<QString,EmojiModel::IndexRange> current){
			if( current.first == m_category )
				return true;
			return false;
		});
		m_categoryIndex = search->second;
	}
}

EmojiProxyList::EmojiProxyList(QObject *parent)
    : QAbstractProxyModel(parent)
{

}

QModelIndex EmojiProxyList::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();
	if(!m_emojiModel )
		return QModelIndex();

	int proxy_row = proxyIndex.row();
	int proxy_column = proxyIndex.column();
	EmojiModel::IndexRange sourceEmojiRange = m_sourceLines[proxy_row];
	int source_row = sourceEmojiRange.begin + proxy_column;
//	qDebug() << QStringLiteral("proxy_row = %0").arg(proxy_row);
//	qDebug() << QStringLiteral("proxy_column = %0").arg(proxy_column);
//	qDebug() << QStringLiteral("source_row = %0").arg(source_row);

	return m_emojiModel->createIndex(source_row,0);
}

QModelIndex EmojiProxyList::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();
	if(!m_emojiModel || m_columnCount <= 1)
		return QModelIndex();

	int source_row = sourceIndex.row();
	EmojiModel::ItemPtr item = m_emojiModel->getItem(source_row);
	if( !item )
		return QModelIndex();
	int proxy_row = 0;
	int proxy_column = 0;
	EmojiModel::IndexRange proxyCategoryRange = m_proxyCategories[item->category_index];
	// check if it header
	if ( source_row == m_sourceLines[proxyCategoryRange.begin - 1].begin )
		return createIndex(proxyCategoryRange.begin - 1, 0);
	// that calculate row and col of emoji in proxy model
	for(int proxy_line = proxyCategoryRange.begin; proxy_line < proxyCategoryRange.end; proxy_line++ )
	{
		if( source_row >= m_sourceLines[proxy_line].begin && source_row < m_sourceLines[proxy_line].end )
		{
			proxy_row = proxy_line;
			proxy_column = source_row - m_sourceLines[proxy_line].begin;
		}
	}
	return createIndex(proxy_row, proxy_column);
}

void EmojiProxyList::setSourceModel(QAbstractItemModel *sourceModel)
{
	m_emojiModel = qobject_cast<EmojiModel*>(sourceModel);
	recalcCategories();
	QAbstractProxyModel::setSourceModel(sourceModel);
}

QModelIndex EmojiProxyList::index(int row, int col, const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return createIndex(row,col, nullptr);
}

QModelIndex EmojiProxyList::parent(const QModelIndex &child) const
{
	Q_UNUSED(child)
	return QModelIndex();
}

int EmojiProxyList::rowCount(const QModelIndex &parent) const
{
	if(!m_emojiModel || m_columnCount <= 1)
		return 0;
	return m_sourceLines.size();
}

int EmojiProxyList::columnCount(const QModelIndex &parent) const
{
	return emojiColumnCount();
}

QVariant EmojiProxyList::data(const QModelIndex &index, int role) const
{
	QModelIndex sourceIndex = mapToSource(index);
	if(sourceIndex.isValid())
		return sourceIndex.data(role);
	else {
		qWarning() << QStringLiteral("SourceIndex is Invalid (%0, %1)").arg(index.row()).arg(index.column());
	}
	return QVariant();
}

QVariant EmojiProxyList::getData(int row, int column, int role) const
{
	return data( createIndex(row,column), role);
}

int EmojiProxyList::lineSize(int row) const
{
	if( row < 0 || row >= m_sourceLines.size() )
		return 1;
	int column_count = m_sourceLines[row].end - m_sourceLines[row].begin;
//	qDebug() << QStringLiteral("getLineSize( %0 ) = %1").arg(row).arg(column_count);
	return column_count;
}

int EmojiProxyList::indexOfCategoryHeader(int categoryIndex) const
{
	if( categoryIndex < 0 || categoryIndex >= m_proxyCategories.size() )
	{
		qWarning() << QStringLiteral("Wrong category index %0").arg(categoryIndex);
		return 0;
	}
	return m_proxyCategories[categoryIndex].begin - 1;
}

void EmojiProxyList::setEmojiColumnCount(int count)
{
	if( count < 1 )
		count = 1;
	beginResetModel();
	m_columnCount = count;
	recalcCategories();
	endResetModel();
	emit emojiColumnCountChanged();
}

int EmojiProxyList::emojiColumnCount() const
{
	return m_columnCount;
}

void EmojiProxyList::recalcCategories()
{
	if(!m_emojiModel || m_columnCount <= 1)
		return;
	m_proxyCategories.clear();
	m_sourceLines.clear();
//	int row_count = 0;
	for( QPair<QString, EmojiModel::IndexRange> pc : m_emojiModel->m_categories )
	{
		// add category header index range to model
		{
			EmojiModel::IndexRange headerLine;
			headerLine.begin = pc.second.begin - 1;
			headerLine.end = pc.second.begin;
			m_sourceLines.append( headerLine );
		}
		// calculate row count for category
		int category_emoji_count = pc.second.end - pc.second.begin;
		int full_row_count = int( category_emoji_count / m_columnCount );
		int additioanl_row_emoji_count = category_emoji_count % m_columnCount;
		int additioanl_row = (additioanl_row_emoji_count == 0 ? 0 : 1);
		// add range of Proxy model to proxyCategories
		EmojiModel::IndexRange emojiProxyCategoryRowsRange;
		emojiProxyCategoryRowsRange.begin = m_sourceLines.size();
		emojiProxyCategoryRowsRange.end = m_sourceLines.size() + full_row_count + additioanl_row;
		m_proxyCategories.append(emojiProxyCategoryRowsRange);

		for(int new_line = 0, end = full_row_count + additioanl_row; new_line < end; new_line ++)
		{
			EmojiModel::IndexRange lineEmojiRange;
			lineEmojiRange.begin = pc.second.begin + new_line * m_columnCount;
			if( additioanl_row == 1 && new_line == end - 1) // last row
				lineEmojiRange.end = lineEmojiRange.begin + additioanl_row_emoji_count;
			else
				lineEmojiRange.end = lineEmojiRange.begin + m_columnCount;
			m_sourceLines.append(lineEmojiRange);
		}
	}
//	// Just Debug Data
	qDebug() << QStringLiteral("Print proxy categories:");
	for(int i = 0; i < m_proxyCategories.size(); i++ )
	{
		int category_header_index = m_emojiModel->m_categories[i].second.begin - 1;
		EmojiModel::ItemPtr item = m_emojiModel->getItem(category_header_index);
		QString name = item->name;
		qDebug() << QStringLiteral("  [%0] Category \"%1\" (%2 : %3);")
		            .arg(i)
		            .arg(name)
		            .arg(m_proxyCategories[i].begin)
		            .arg(m_proxyCategories[i].end);
	}
}
