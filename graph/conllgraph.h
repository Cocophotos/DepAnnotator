#ifndef CONLLGRAPH_H
#define CONLLGRAPH_H

#include "graph/graph.h"
#include <QString>
#include <QHash>
#include <QtDebug>

/**
 * @brief The ConllNodeProperties class
 * Describes an extended CoNLL node
 *
 * Usually in the CoNLL format
 * nodes have several attributes :
 * - a token
 * - a lemma
 * - a coarse part of speech
 * - a fine part of speech
 * - features
 *
 * In Extended CoNLL we also have
 * coindexed nodes
 * So a node is elliptic (unrealised)
 * or is a perfectly "normal" node
 */
class ConllNodeProperties
{
public:
    //Unrealised means that it's a elliptical item
    enum NodeType{ Realised = 1, Unrealised, Dummy };

    typedef QMap<QString, QString>::const_iterator const_feature_iter;

    /**
     * @brief ConllNodeProperties
     * @param i is node's id
     * @param token is node's token
     * @param lemma is node's lemma
     * @param hyperpos is node's coarse POS
     * @param pos is node's fine POS
     * @param features is node ' features
     * @param type is node 's type (unrealised or normal)
     */
    ConllNodeProperties(unsigned i,
                        QString token,
                        QString lemma,
                        QString hyperpos,
                        QString pos,
                        QMap<QString, QString> features,
                        NodeType type = Realised)
        :id(i), mToken(token), mLemma(lemma), mHyperpos(hyperpos),
          mPos(pos), mFeatures(features), mType(type), mCoindex(0), mComment("")
    {
    }

    /**
     * @brief ConllNodeProperties
     * Copy constructor
     */
    ConllNodeProperties(const ConllNodeProperties &other)
        :id(other.id), mToken(other.mToken), mLemma(other.mLemma),
          mHyperpos(other.mHyperpos),
          mPos(other.mPos),
          mFeatures(other.mFeatures), mType(other.mType), mCoindex(other.mCoindex),
          mComment(other.mComment){}

    /**
     * @brief token
     * @return node's token
     */
    QString token()const {return mToken;}

    /**
     * @brief setToken
     * Change node's token by new token
     * @param token
     */
    void setToken(QString token) {mToken = token;}

    /**
     * @brief lemma
     * @return node's lemma
     */
    QString lemma()const {return mLemma;}

    /**
     * @brief setLemma
     * Change node's lemma by new lemma
     * @param lemma
     */
    void setLemma(QString lemma) {mLemma = lemma;}

    /**
     * @brief hyperpos
     * @return
     */
    QString hyperpos()const {return mHyperpos;}

    /**
     * @brief setHyperpos
     * Change node's coarse POS by new POS
     * @param hyperpos
     */
    void setHyperpos(QString hyperpos) {mHyperpos = hyperpos;}

    /**
     * @brief pos
     * @return node's pos
     */
    QString pos()const {return mPos;}

    /**
     * @brief setPos
     * Change node's POS by new POS
     * @param pos
     */
    void setPos(QString pos) {mPos = pos;}

    /**
     * @brief feature
     * @param key
     * @return feature's value associated with the key
     * or an empty string is the key does not exists
     */
    QString feature(QString key)const {return mFeatures.value(key, "");}

    /**
     * @brief setFeature
     * Change the feature's value for the key
     * @param key
     * @param value
     */
    void setFeature(QString key, QString value) {mFeatures.insert(key, value);}

    /**
     * @brief clearAllFeatures
     * Remove all features
     */
    void clearAllFeatures() {mFeatures.clear();}

    /**
     * @brief clearFeature
     * Remove the feature called "key"
     * @param key
     */
    void clearFeature(QString key) {mFeatures.remove(key);}

    /**
     * @brief featuresIterator
     * @return A pair of iterator to browse features
     */
    pair<const_feature_iter, const_feature_iter> featuresIterator()const
    {return make_pair(mFeatures.constBegin(), mFeatures.constEnd());}

    /**
     * @brief isRoot
     * @return true if node is root of the sentence
     */
    //bool isRoot()const {return !mRoot.isEmpty();}

    /**
     * @brief setRoot
     * Change the node status : is it the root or not
     * @param r
     */
    //void setRoot(QString r) { mRoot = r; }

    /**
     * @brief type
     * @return node's type
     */
    NodeType type()const {return mType;}

    /**
     * @brief setType
     * Change node's type to new type t
     * @param t
     */
    void setType(NodeType t) {mType = t;}


    /**
     * @brief coindex
     * Return the node index with which this
     * node is coindexed.
     * If there is no coindexation, return 0
     */
    unsigned coindex()const {return mCoindex;}

    /**
     * @brief setCoindex
     * Change the coindexation with the new index node c
     * @param c
     */
    void setCoindex(unsigned c)
    {
        if(c == 0)
            mToken = "_e_";
        else
            mToken = "_e(" + QString::number(c/10) + ")_";
        mCoindex = c;
    }

    /**
     * @brief comment
     * @return the comment attached to that node
     */
    QString comment()const {return mComment;}

    /**
     * @brief setComment
     * Change the comment attached to this node by
     * the new comment c
     * @param c
     */
    void setComment(QString c){mComment = c;}


    /*inline friend std::ostream& operator<<(std::ostream& out, const ConllNodeProperties& prop) {
        out << "(" << prop.id << ") "
            << prop.mToken.toStdString()
            << "(" << prop.mLemma.toStdString() << ", " << prop.mPos.toStdString() << ")";
        return out;
    }*/

    /**
     * @brief id
     * Node's id
     */
    unsigned id;
protected:
    QString mToken, mLemma, mHyperpos, mPos;
    QMap<QString, QString> mFeatures;
    NodeType mType;
    unsigned mCoindex;
    QString mComment;
};

/**
 * @brief The ConllEdgeProperties class
 * Describes an extended CoNLL edge
 *
 * Usually a node it's just a source, a target
 * and a label (which describes the relationship
 * between source and target).
 *
 * But extended CoNLL format describes a node
 * with at most two labels : the surfacic one
 * and the deeper one.
 * Surfacic is called final function and
 * deep is called initial function.
 *
 * Moreover an edge can be exclusively deep or
 * surfacic or even both.
 */
class ConllEdgeProperties
{
public:
    /**
     * @brief The DependencyType enum
     * Describe edge type :
     * - Surfacic
     * - Deep
     * - Both
     * - None : not Deep nor Surfacic (so it's an intermediary representation)
     */
    enum DependencyType{ Deep = 1, Surfacic, Both, None };

    /**
     * @brief ConllEdgeProperties
     * Default constructor
     */
    ConllEdgeProperties()
        :id(0), mRelInit(""), mRelFinal(""), mType(Both), mOriginal(false){}


    /**
     * @brief ConllEdgeProperties
     * @param i edge id
     * @param relInit edge initial function
     * @param relFinal edge final function
     * @param type edge type
     * @param original boolean telling if the edge was
     * in the original CoNLL file
     * --> To be backward compatible
     */
    ConllEdgeProperties(unsigned i, QString relInit, QString relFinal = "",
                        DependencyType type = Both, bool original = false)
        :id(i), mRelInit(relInit), mRelFinal(relFinal), mType(type), mOriginal(original){}

    /**
     * @brief ConllEdgeProperties
     * Copy constructor
     * @param other
     */
    ConllEdgeProperties(const ConllEdgeProperties &other)
        :id(other.id), mRelInit(other.mRelInit),
          mRelFinal(other.mRelFinal), mType(other.mType),
          mOriginal(other.mOriginal){}

    /**
     * @brief relInit
     * @return initial function
     */
    QString relInit()const {return mRelInit;}

    /**
     * @brief setRelInit
     * Change initial function by new rel
     * @param rel
     */
    void setRelInit(QString rel){mRelInit = rel;}

    /**
     * @brief relFinal
     * @return final function
     */
    QString relFinal()const {return mRelFinal;}

    /**
     * @brief setRelFinal
     * Change final function by new rel
     * @param rel
     */
    void setRelFinal(QString rel){mRelFinal = rel;}

    /**
     * @brief type
     * @return edge's type
     */
    DependencyType type()const {return mType;}

    /**
     * @brief setDependencyType
     * Replace type by new type newType
     * @param newType
     */
    void setDependencyType(DependencyType newType){mType = newType;}

    /**
     * @brief original
     * @return true if the edge is original (was present in the default CoNLL file),
     * false otherwise
     */
    bool original()const {return mOriginal;}

    /**
     * @brief setOriginal
     * @param o
     */
    void setOriginal(bool o){mOriginal = o;}

    /**
     * @brief id
     * edge's id
     */
    unsigned id;

protected:
    QString mRelInit, mRelFinal;
    DependencyType mType;
    bool mOriginal;
};

typedef Graph<ConllNodeProperties, ConllEdgeProperties> ConllGraph;
#endif // CONLLGRAPH_H
