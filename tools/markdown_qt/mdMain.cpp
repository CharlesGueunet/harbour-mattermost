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

	parse_emoji_json();

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
	if (false) // now it should work inside discout
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

bool mdMain::parse_emoji_json()
{
	QFile file( QStringLiteral("%0/emoji__.json").arg(EMOJI_PATH) );
	file.open(QFile::ReadOnly);
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
	file.close();
	if (!error.errorString().isEmpty())
		qWarning() << error.errorString();
	if (doc.isEmpty())
		return false;
	if (!doc.isArray())
		return false;

	QJsonArray ar = doc.array();
	QJsonArray out_array;
	for (int i = 0; i < ar.size(); i ++ )
	{
		QJsonObject out_item;

		QJsonObject current = ar[i].toObject();
		QString image_name = current["image"].toString();
		if( image_name.indexOf(QStringLiteral("00")) == 0 ) // if name bgins with 00, just remove it
			image_name = image_name.mid(2);
		QString image = QStringLiteral("svg/") + image_name.replace(".png",".svg");
		QRegExp re(".*[a-z]+\\-[a-z]+.*");

		out_item["image"] = image;
		//short_names
		QJsonArray short_names = current["short_names"].toArray();
		QJsonArray out_names;
		for( int j = 0; j < short_names.size(); j++)
		{
			QString short_name = short_names[j].toString();
			if( re.indexIn(short_name) != -1 )
				short_name.replace("-","_");
			emoji.insert( short_name, image );
			out_names.push_back( short_name );
		}
		out_item["short_names"] = out_names;
		out_item["category"] = current["category"];
		out_array.push_back(out_item);
	}
	// save result now! ( just for test )
	// for now, just modify it by code
	QStringList other;
	other << "bowtie.png";
	other << "feelsgood.png";
	other << "finnadie.png";
	other << "goberserk.png";
	other << "godmode.png";
	other << "hurtrealbad.png";
	other << "neckbeard.png";
	other << "octocat.png";
	other << "rage1.png";
	other << "rage2.png";
	other << "rage3.png";
	other << "rage4.png";
	other << "shipit.png";
	other << "suspect.png";
	other << "trollface.png";
	for(QString current : other)
	{
		QJsonObject out_item;
		QJsonArray out_names;
		QString short_name =current.left(current.length() - 4);
		QString image = current;
		out_names.push_back( short_name );
		out_item["image"]  = QString("png/") + image;
		out_item["short_names"] = out_names;
		out_item["category"] = QStringLiteral("Other");
		out_array.push_back(out_item);
	}
	doc.setArray( out_array );
	if( output.isNull() )
		output = QJsonDocument(doc);
	else
		output = doc;
	QFile out_file( QStringLiteral("%0/emoji_out.json").arg(EMOJI_PATH) );
	out_file.open(QFile::WriteOnly);
	QByteArray data = output.toJson(QJsonDocument::Indented);
	out_file.write( data );
	out_file.close();
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
	QFile file(":/resources/gistfile1.md");
	file.open(QFile::ReadOnly);
	ui->sourceText->setPlainText( QString::fromUtf8(file.readAll()) );
}

void mdMain::on_actExit_triggered()
{
	QApplication::exit(0);
}

void mdMain::on_act_saveJson_triggered()
{
	QFile out_file( QStringLiteral("%0/emoji_out.json").arg(EMOJI_PATH) );
	out_file.open(QFile::WriteOnly);
	out_file.write( output.toJson(QJsonDocument::Indented) );
	out_file.close();
}
