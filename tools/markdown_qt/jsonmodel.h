#ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QAbstractItemModel>
#include <QJsonDocument>

class JsonModel : public QAbstractItemModel
{
    Q_OBJECT

	enum DataRoles {
		ImageName,
		ImagePath,
		Section ,
		Type,
		EmojiName = Qt::DisplayRole,
	};

	enum ItemType {
		ItemEmoji,
		ItemSection
	};

public:
	explicit JsonModel(QJsonDocument &doc, QObject *parent = nullptr);

	Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	virtual QHash<int,QByteArray> roleNames() const override;
signals:

public slots:

protected:
	QJsonDocument emojiJson;
};

#endif // JSONMODEL_H
