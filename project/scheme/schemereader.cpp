#include "schemereader.h"

#include <QtGui>
#include <QtDebug>

SchemeReader *SchemeReader::_singleton = NULL;


bool SchemeReader::read(QString filepath)
{
    mInitials.clear();
    mFinals.clear();

    QFile *f = new QFile(filepath);
    if(!f->open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    parser.setDevice(f);
    if (parser.readNextStartElement())
    {
        if (parser.name() == "properties")
            readScheme();
        else
            parser.raiseError(QObject::tr("The file seems to be a proper DGA scheme file."));
    }
    return !parser.error();

}

void SchemeReader::readScheme()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "properties");

    while( !(parser.tokenType() == QXmlStreamReader::EndElement
             && parser.name() == "properties" ))
    {
        QXmlStreamReader::TokenType token = parser.readNext();
        if(token == QXmlStreamReader::StartElement)
        {
            if(parser.name() == "entry")
            {
                QString key = parser.attributes().value("key").toString();
                if(key == "language"){
                    QString language = parser.readElementText().trimmed();
                    Q_UNUSED(language);
                }else if(key == "corpus"){
                    QString corpus = parser.readElementText().trimmed();
                    Q_UNUSED(corpus);
                }else if(key == "deprels"){
                    readDepRels();
                }else if(key == "pos"){
                    readPos();
                }else if(key == "coarsepos"){
                    readCoarsePos();
                }else if(key == "features"){
                    readFeatures();
                }else if(key == "doubled_relations")
                    readDoubledRels();
            }
        }

        if(parser.hasError())
        {
            qDebug() << parser.errorString();
            qDebug() << parser.lineNumber();
            break;
        }
    }
}

void SchemeReader::readDepRels()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "entry"
             && parser.attributes().value("key").toString() == "deprels");
    QSet<QString> rels = parser.readElementText().trimmed().split("\n").toSet();

    foreach(QString rel, rels){
        if(rel.startsWith("I_")){
            QStringList rel_comment = rel.remove(0,2).split("|");
            mInitials.insert(rel_comment[0], rel_comment.size() > 1 ? rel_comment[1] : "");
        }
        else if(rel.startsWith("F_")){
            QStringList rel_comment = rel.remove(0,2).split("|");
            mFinals.insert(rel_comment[0], rel_comment.size() > 1 ? rel_comment[1] : "");
        }
        else{
            QStringList rel_comment = rel.split("|");
            mFinals.insert(rel_comment[0], rel_comment.size() > 1 ? rel_comment[1] : "");
        }
    }
}

void SchemeReader::readCoarsePos()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "entry"
             && parser.attributes().value("key").toString() == "coarsepos");

    mCPos = parser.readElementText().trimmed().split("\n");
}

void SchemeReader::readPos()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "entry"
             && parser.attributes().value("key").toString() == "pos");

    mPOS = parser.readElementText().trimmed().split("\n");

    QStringList POSWithoutComments;
    foreach(QString pos, mPOS){
        POSWithoutComments << pos.split("|")[0];
    }
    mPOS = POSWithoutComments;
}

void SchemeReader::readFeatures()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "entry"
             && parser.attributes().value("key").toString() == "features");

    while( !(parser.tokenType() == QXmlStreamReader::EndElement
             && parser.name() == "entry" ))
    {
        QXmlStreamReader::TokenType token = parser.readNext();
        QString key = parser.attributes().value("name").toString();
        if(token == QXmlStreamReader::StartElement)
        {
            while( !(parser.tokenType() == QXmlStreamReader::EndElement
                     && parser.name() == "feat" ))
            {
                token = parser.readNext();
                if(token == QXmlStreamReader::StartElement)
                {
                    QString value = parser.readElementText().trimmed();                    
                    mFeatures.insert(key, value);
                }
            }
        }
    }
}

void SchemeReader::readDoubledRels()
{
    Q_ASSERT(parser.isStartElement() && parser.name() == "entry"
            && parser.attributes().value("key").toString() == "doubled_relations");

    QXmlStreamReader::TokenType token;
    while((token = parser.readNext())
          != QXmlStreamReader::EndElement
          && parser.name() != "entry")
    {
        if(token == QXmlStreamReader::StartElement && parser.name() == "rel")
            mDoubledRels.insert(parser.readElementText().trimmed());
    }
}
