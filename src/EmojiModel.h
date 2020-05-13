#ifndef EMOJIMODEL_H
#define EMOJIMODEL_H

#include <QAbstractListModel>
#include <QAbstractProxyModel>
#include <QSharedPointer>

//class MattermostQt;

class EmojiModel : public QAbstractListModel
{
	Q_OBJECT

//	Q_PROPERTY(MattermostQt *mattermost READ getMattermost WRITE setMattermost)
	Q_PROPERTY(QStringList categories READ categories NOTIFY categoriesChanged)
	friend class EmojiProxyCategory;
public:
	enum DataRoles {
		RoleName = Qt::UserRole,
		RoleImage,
		RoleCategory,
		RoleType,
	};
	Q_ENUM(DataRoles)

	enum ItemType {
		ItemTypeEmoji,
		ItemTypeCategory,
	};
	Q_ENUM(ItemType)

private:
	struct Item {
		QString  name; // name
		QString  image; // icon
		QString  category; // category
		ItemType type = ItemTypeEmoji;
	};
	typedef QSharedPointer<Item> ItemPtr;

	struct Category {
		int begin = -1;
		int end   = -1;
	};

public:
	EmojiModel(QObject *parent = Q_NULLPTR);

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual QHash<int, QByteArray> roleNames() const override;

//	void setMattermost(MattermostQt *mattermost);
//	MattermostQt *getMattermost() const;

	QStringList categories() const;

	Q_INVOKABLE QString categoryIcon(QString category) const;
private:
	void loadEmoji();

Q_SIGNALS:
	void categoriesChanged();

private:
//	MattermostQt *m_mattermost = nullptr;
	QVector<ItemPtr>  m_items;
	QMap<QString, Category> m_categories;
};

class EmojiProxyCategory : public QAbstractProxyModel
{
	Q_OBJECT

	Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
public:
	EmojiProxyCategory(QObject *parent = nullptr);

	Q_INVOKABLE virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
	Q_INVOKABLE virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

	Q_INVOKABLE virtual QModelIndex index(int row, int col, const QModelIndex &parent) const override;
	Q_INVOKABLE virtual QModelIndex parent(const QModelIndex &child) const override;

	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	QString category() const;
	void setCategory(const QString &in_category);

private :
	void updateCategoryIndex();
Q_SIGNALS:
	void categoryChanged();

private:
	EmojiModel           *m_emojiModel = nullptr;
	QString               m_category;
	EmojiModel::Category  m_categoryIndex;
};

#endif // EMOJIMODEL_H
