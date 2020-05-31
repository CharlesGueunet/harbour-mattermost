#ifndef MDMAIN_H
#define MDMAIN_H
#include <qglobal.h>

#include <QMainWindow>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QNetworkAccessManager>

namespace Ui
{
	class mdMain;
}

class mdMain : public QMainWindow
{
	Q_OBJECT

	Q_PROPERTY(QString currentHtml READ currentHtml NOTIFY currentHtmlChanged)
public:
	class emoji2unicode
	{
	public:
		QString unicode;
		QString url;
	};

//	typedef struct emoji2unicode Emoji2Unicode;
public:
	explicit mdMain(QWidget *parent = nullptr);
	~mdMain();

	void updateText();

	void generateFlags();

	QString currentHtml() const;
protected:
	void changeEvent(QEvent *e);

	bool parse_emoji_json();

	void get_gemoji_json();
	void parse_gemoji_json();
private slots:
	void on_pushButton_clicked();

	void on_m_flagsWidget_itemChanged(QListWidgetItem *item);

	void on_sourceText_textChanged();

	void on_htmlArea_textChanged();

	void on_actEmojiTest_triggered();
	
	void on_actExit_triggered();
	
	void on_act_saveJson_triggered();

Q_SIGNALS:
	void currentHtmlChanged();

private:
	Ui::mdMain *ui;
	QObject *quickText;
	QHash<QString, QString> emoji;
	QJsonDocument output;
	QJsonDocument gemoji;
	QNetworkAccessManager *mNetwork = nullptr;
	QString mCurrentHtml;
};

#endif // MDMAIN_H
