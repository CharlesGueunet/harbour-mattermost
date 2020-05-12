#ifndef EMOJIMODEL_H
#define EMOJIMODEL_H

#include <QAbstractListModel>

//class MattermostQt;

class EmojiModel : public QAbstractListModel
{
	Q_OBJECT

//	Q_PROPERTY(MattermostQt *mattermost READ getMattermost WRITE setMattermost)
	Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
public:
	enum DataRoles {
		RoleName = Qt::UserRole,
		RoleImage,
		RoleCategory,
	};
	Q_ENUM(DataRoles)
public:
	EmojiModel(QObject *parent = Q_NULLPTR);

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual QHash<int, QByteArray> roleNames() const override;

//	void setMattermost(MattermostQt *mattermost);
//	MattermostQt *getMattermost() const;

	void setCategory(const QString &name);
	QString category() const;

private:
	void loadEmoji();

Q_SIGNALS:
	void categoryChanged();

private:
//	MattermostQt *m_mattermost = nullptr;
	QString       m_category;
	QVector< QPair<QString,QString> > m_emoji;
};

#endif // EMOJIMODEL_H
