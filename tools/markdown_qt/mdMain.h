#ifndef MDMAIN_H
#define MDMAIN_H
#include <qglobal.h>

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui
{
	class mdMain;
}

class mdMain : public QMainWindow
{
	Q_OBJECT

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
protected:
	void changeEvent(QEvent *e);

	void parse_emoji_json();
	void parse_emoji_json2();

	void parse_emoji_from_rb();

private slots:
	void on_pushButton_clicked();

	void on_m_flagsWidget_itemChanged(QListWidgetItem *item);

	void on_sourceText_textChanged();

	void on_htmlArea_textChanged();

	void on_actEmojiTest_triggered();
	
	void on_actExit_triggered();
	
private:
	Ui::mdMain *ui;
	QObject *quickText;
	QHash<QString, QString> emoji;
};

#endif // MDMAIN_H
