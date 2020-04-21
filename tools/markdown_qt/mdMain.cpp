#include "mdMain.h"
#include "ui_mdMain.h"
//#include <quckobject.h>
#include <QByteArray>
#include <QDebug>
#include <QQmlProperty>
#include <QQuickWidget>
#include <QQuickItem>
#include <QPlainTextEdit>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QBitArray>

extern "C" {
#include <markdown.h>
#undef if
#undef while
}

mdMain::mdMain(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::mdMain)
{
	ui->setupUi(this);

//	qmlRegisterType<QuickObject>("ru.sashikknox", 1, 0, "QuickObject");

	ui->quickArea->setSource(QUrl("qrc:/qml/TextView.qml"));

//	ui->quickArea->quickWindow()->
	quickText = ui->quickArea->rootObject()->findChild<QObject*>("textObject");
	QQmlProperty(quickText, "text").write("Hello!");
	//object->setProperty("text", QString("Ebana rot!"));

//	parse_emoji_json();

	generateFlags();
	updateText();
}

mdMain::~mdMain()
{
	delete ui;
}

void mdMain::updateText()
{
	QString text = ui->sourceText->toPlainText();
	// first replace emiji
	if (true)
	{
		QRegExp em("\\:([a-zA-Z_0-9\\-\\+]+)\\:");
		int last_pos = em.indexIn(text, 0);
		while (  last_pos != -1 )
		{
			QString short_name = em.cap(1);
			QHash<QString, QString>::iterator it =  emoji.find(short_name);
			if ( it != emoji.end() )
			{
				text.replace(last_pos, short_name.length() + 2, it.value() );
			}
			else
			{
				last_pos ++;
			}
			last_pos = em.indexIn(text, last_pos);
		}
	}

	QByteArray data = text.toUtf8();
	unsigned int flags = 0;

	for (int i = 0; i < ui->m_flagsWidget->count(); i++)
	{
		QListWidgetItem *item = ui->m_flagsWidget->item(i);
		flags +=  (item->checkState() == Qt::Checked) ? item->data(Qt::UserRole).toUInt() : 0;
	}

	Document *doc = gfm_string(data.data(), data.size(), flags);

	if (!doc)
	{
		qDebug() << "Error";
	}
	int ok = mkd_compile(doc, flags);

	char *html_text = NULL, *css_text = NULL;
	int size = mkd_document(doc, &html_text);
	int size_2 = mkd_css(doc, &css_text);

	if (size_2)
	{
		qInfo() << css_text;
	}
	if (size)
	{
		QString r = QString::fromUtf8(html_text, size);
//		ui->textArea->setHtml(r);
		ui->htmlArea->setPlainText(r);
//		QQmlProperty(quickText, "text").write(r);
	}
	mkd_cleanup(doc);
}

#define add_flag(flag, description, checked) \
	{\
		QListWidgetItem *item = new QListWidgetItem(QString("%0 : %1").arg(#flag).arg(description),ui->m_flagsWidget);\
		item->setToolTip( item->text() );\
		item->setData(Qt::UserRole, QVariant::fromValue(flag) );\
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);\
		item->setCheckState((checked)?Qt::Checked:Qt::Unchecked);\
	}

void mdMain::generateFlags()
{
	add_flag(MKD_NOLINKS, "Don’t do link processing, block <a> tags", false)
	add_flag(MKD_NOIMAGE, "Don’t do image processing, block <img>", false)
	add_flag(MKD_NOPANTS, "Don’t run smartypants()", false)
	add_flag(MKD_NOHTML, "Don’t allow raw html through AT ALL", false)
	add_flag(MKD_STRICT, "Disable SUPERSCRIPT, RELAXED_EMPHASIS", false)
	add_flag(MKD_TAGTEXT, "Process text to go inside an html tag; no emphasis or html expansion & embedded html will be stripped out.", false)
	add_flag(MKD_NO_EXT, "Don’t allow pseudo-protocols", false)
	add_flag(MKD_CDATA, "Generate code for xml ![CDATA[...]]", false)
	add_flag(MKD_NOSUPERSCRIPT, "No A^B", false)
	add_flag(MKD_NORELAXED, "Emphasis happens everywhere", false)
	add_flag(MKD_NOTABLES, "Don’t process PHP Markdown Extra tables.", false)
	add_flag(MKD_NOSTRIKETHROUGH, "Forbid ~~strikethrough~~", false)
	add_flag(MKD_TOC, "Do table-of-contents processing", false)
	add_flag(MKD_1_COMPAT, "Compatability with MarkdownTest_1.0", false)
	add_flag(MKD_AUTOLINK, "Make http://foo.com a link even without <>s", true)
	add_flag(MKD_SAFELINK, "Paranoid check for link protocol", false)
	add_flag(MKD_NOHEADER, "Don’t process document headers", false)
	add_flag(MKD_TABSTOP, "Expand tabs to 4 spaces", false)
	add_flag(MKD_NODIVQUOTE, "Forbid >%class% blocks", false)
	add_flag(MKD_NOALPHALIST, "Forbid alphabetic lists", false)
	add_flag(MKD_NODLIST, "Forbid definition lists", false)
	add_flag(MKD_EXTRA_FOOTNOTE, "Enable PHP Markdown Extra-style footnotes (warning: not the later version of multiple-paragraph ones.)", false)
	add_flag(MKD_NOSTYLE, "Don’t extract <style> blocks", false)
	add_flag(MKD_NODLDISCOUNT, "Disable discount-style definition lists", false)
	add_flag(MKD_DLEXTRA, "Enable PHP Markdown Extra definition lists", false)
	add_flag(MKD_FENCEDCODE, "Enable Github-style fenced code blocks.", true)
	add_flag(MKD_GITHUBTAGS, "Allow dashes & underscores in element names", true)
//	add_flag(MKD_HTML5ANCHOR,"Use the html5 namespace for anchor names",false)
	add_flag(MKD_LATEX, "Enable embedded LaTeX (mathjax-style)", false)
	add_flag(MKD_EXPLICITLIST, "Don’t merge adjacent numbered/bulleted lists", true)
}
void mdMain::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void mdMain::on_pushButton_clicked()
{
	updateText();
}

void mdMain::on_m_flagsWidget_itemChanged(QListWidgetItem *item)
{
	updateText();
}

void mdMain::on_sourceText_textChanged()
{
	updateText();
}

void mdMain::on_htmlArea_textChanged()
{
	QString r = ui->htmlArea->toPlainText();
	ui->textArea->setHtml(r);
	QQmlProperty(quickText, "text").write(r);
}

#define char2hex(symbol,result,mv) \
	switch (symbol)\
	{\
	case '0':\
		result |= 0x00 << mv;\
		break;\
	case '1':\
		result |= 0x01 << mv;\
		break;\
	case '2':\
		result |= 0x02 << mv;\
		break;\
	case '3':\
		result |= 0x03 << mv;\
		break;\
	case '4':\
		result |= 0x04 << mv;\
		break;\
	case '5':\
		result |= 0x05 << mv;\
		break;\
	case '6':\
		result |= 0x06 << mv;\
		break;\
	case '7':\
		result |= 0x07 << mv;\
		break;\
	case '8':\
		result |= 0x08 << mv;\
		break;\
	case '9':\
		result |= 0x09 << mv;\
		break;\
	case 'A':\
	case 'a':\
		result |= 0x0a << mv;\
		break;\
	case 'B':\
	case 'b':\
		result |= 0x0b << mv;\
		break;\
	case 'C':\
	case 'c':\
		result |= 0x0c << mv;\
		break;\
	case 'D':\
	case 'd':\
		result |= 0x0d << mv;\
		break;\
	case 'E':\
	case 'e':\
		result |= 0x0e << mv;\
		break;\
	case 'F':\
	case 'f':\
		result |= 0x0f << mv;\
		break;\
	}

QString toUnicode(QString &s)
{
//	return QString("%0%1").arg(QChar('\\u')).arg(s);

	ushort r = 0;
	char2hex(s[0].toLatin1(), r, 12);
	char2hex(s[1].toLatin1(), r, 8);
	char2hex(s[2].toLatin1(), r, 4);
	char2hex(s[3].toLatin1(), r, 0);
	return QString::fromUtf16(&r, 1);
}

void mdMain::parse_emoji_json()
{
	QFile file( QStringLiteral(EMOJI_PATH) + "/emoji_pretty.json");
	file.open(QFile::ReadOnly);
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

	if (!error.errorString().isEmpty())
		qWarning() << error.errorString();
	if (doc.isEmpty())
		return;
	if (!doc.isArray())
		return;

	QJsonArray ar = doc.array();
//	Hash *simple_hash = hash_new();
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject current = ar[i].toObject();
		QString unicode = current["unified"].toString();
		//unicode.insert(0, "\\u");
		QString une = toUnicode(unicode);
		//qrc:/emoji/64/0023.png
		QString short_name = current["short_name"].toString();
		QString image_name = current["image"].toString();
		QString image = QString("<img width=\"16\" height=\"16\" src=\"qrc:/emoji/32/") + image_name + QString("\"/>");
		emoji.insert( short_name, image );
		QByteArray utf8_name = short_name.toLatin1();
		QByteArray utf8_image = image_name.toLatin1();
//		hash_add_entry(simple_hash, utf8_name.data(), utf8_name.size(), utf8_image.data(), utf8_image.size() );
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		for( int j = 0; j < short_names.size(); j++)
			emoji.insert( short_names[j].toString(), image );
	}
	
	qDebug() << "All done!";
}

void mdMain::parse_emoji_json2()
{
	QFile file(QStringLiteral(EMOJI_PATH) + "/emoji.json");
	file.open(QFile::ReadOnly);
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

	if (!error.errorString().isEmpty())
		qWarning() << error.errorString();
	if (doc.isEmpty())
		return;
	if (!doc.isArray())
		return;

	QJsonArray ar = doc.array();
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject current = ar[i].toObject();
		QString unicode = current["emoji"].toString();
		//unicode.insert(0, "\\u");
		QString une = toUnicode(unicode);
		//qrc:/emoji/64/0023.png
		QString image = QString("<img width=\"16\" height=\"16\" src=\"qrc:/emoji/32/") + current["image"].toString() + QString("\"/>");
		
		QJsonArray aliases = current["aliases"].toArray();
		for( int j = 0; j < aliases.size(); j++)
			emoji.insert( aliases[j].toString(), image );
	}
	qDebug() << "All done!";
}

void mdMain::parse_emoji_from_rb()
{
	QFile file(QStringLiteral(EMOJI_PATH) + "/emoji_utf8.rb");
	file.open(QFile::ReadOnly);
}


void parse_emoji_html()
{
	QFile file(QStringLiteral(EMOJI_PATH) + "/table.html");
	file.open(QFile::ReadOnly);


//	QJsonArray ar = doc.array();
//	for (int i = 0; i < ar.size(); i ++ )
//	{
//		QJsonObject current = ar[i].toObject();
//		QString unicode = current["unified"].toString();
//		//unicode.insert(0, "\\u");
//		QString une = toUnicode(unicode);
//		//qrc:/emoji/64/0023.png
//		QString image = QString("<img src=\"qrc:/emoji/32/") + current["image"].toString() + QString("\"/>");
//		emoji.insert( current["short_name"].toString(), image );
//	}
	qDebug() << "All done!";
}

void mdMain::on_actEmojiTest_triggered()
{
	QFile file(":/gistfile1.md");
	file.open(QFile::ReadOnly);
	ui->sourceText->setPlainText( QString::fromUtf8(file.readAll()) );
}

void mdMain::on_actExit_triggered()
{
	QApplication::exit(0);
}
