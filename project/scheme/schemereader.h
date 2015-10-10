#ifndef SCHEMEREADER_H
#define SCHEMEREADER_H

#include <QString>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QXmlStreamReader>

/**
 * @brief The SchemeReader class
 * XML Reader of a DGA (http://medialab.di.unipi.it/Project/QA/Parser/DgAnnotator/)
 * XML annotation scheme which includes relations label and parts of speech
 *
 */
class SchemeReader
{
public:

    /**
     * @brief schemeReader
     * @return Create an instance of Scheme reader
     */
    static SchemeReader *schemeReader ()
    {
        if (NULL == _singleton)
        {
            _singleton =  new SchemeReader;
        }
        return _singleton;
    }

    /**
     * @brief kill
     * Destroy the instance
     */
    static void kill ()
    {
        if (NULL != _singleton)
        {
            delete _singleton;
            _singleton = NULL;
        }
    }

    /**
     * @brief read
     * Read a .xml DGA file
     * @param filepath
     * @return true on success, false otherwise
     */
    bool read(QString filepath);

    /**
     * @brief initials
     * @return list of initial functions
     */
    QMap<QString,QString> initials()const {return mInitials;}

    /**
     * @brief finals
     * @return list of final functions
     */
    QMap<QString,QString> finals()const {return mFinals;}

    /**
     * @brief pos
     * @return list of parts of speech
     */
    QStringList pos()const {return mPOS;}

    /**
     * @brief coarsePos
     * @return list of coarse parts of speech (column 4 in CoNLL format)
     */
    QStringList coarsePos()const {return mCPos;}

    /**
     * @brief featureValues
     * @param feature's key
     * @return set of values for a particular feature
     */
    QStringList featureValues(QString key)const {return mFeatures.values(key);}

    /**
     * @brief featureKeys
     * @return all features'name;
     */
    QStringList featureKeys()const
    {
        QStringList l = mFeatures.keys();
        l.removeDuplicates();
        return l;
    }

    /**
     * @brief doubledRels
     * @return list of relations that can be doubled (ex: ats --> ats:ats)
     */
    QSet<QString> doubledRels()const {return mDoubledRels;}

private:
    void readScheme();
    void readDepRels();
    void readPos();
    void readCoarsePos();
    void readFeatures();
    void readDoubledRels();

    /**
     * @brief SchemeReader
     * Default constructor
     */
    SchemeReader(){}

    /**
     * @brief ~SchemeReader
     * Default destructor
     */
    ~SchemeReader(){}

private:
    static SchemeReader *_singleton;
    QXmlStreamReader parser;

    QMap<QString,QString> mInitials, mFinals;
    QStringList mPOS, mCPos;
    QMultiHash<QString, QString> mFeatures;
    QSet<QString> mDoubledRels;
};

#endif // SCHEMEREADER_H
