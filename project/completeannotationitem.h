#ifndef COMPLETEANNOTATIONITEM_H
#define COMPLETEANNOTATIONITEM_H

#include <QList>
#include "project/annotationitem.h"

typedef QList<Revision<AnnotationItem*> > RevisionAnnotationList;

/**
 * @brief The CompleteAnnotationItem class
 *
 * Current annotation item and revisions list
 */
class CompleteAnnotationItem
{
public:
    /**
     * @brief CompleteAnnotationItem
     * @param id annotation item's id
     * @param it current annotation item
     * @param rev revisions list
     */
    CompleteAnnotationItem(unsigned id, AnnotationItem* it, RevisionAnnotationList rev)
        :mId(id), item(it), revisions(rev){}

    /**
     * @brief getItem
     * @return annotation item
     */
    AnnotationItem* getItem()const {return item;}

    /**
     * @brief getRevisionsCount
     * @return number of revisions
     */
    int getRevisionsCount() const {return revisions.size();}

    /**
     * @brief getRevision
     * @param n
     * @return the nth revision
     */
    Revision<AnnotationItem*> getRevision(int n){ return revisions.at(n);}

    /**
     * @brief removeLastRevision
     * Remove last revision (the oldest one)
     */
    void removeLastRevision(){ revisions.removeLast(); }

    /**
     * @brief addRevision
     * Add a new revision
     * @param item annotation item
     * @param time current timestamp
     */
    void addRevision(AnnotationItem *item, QDateTime time = QDateTime::currentDateTime())
    {
        revisions << Revision<AnnotationItem*>(item, time);
    }

    /**
     * @brief id
     * @return annotation id
     */
    unsigned id()const{return mId;}

    /**
     * @brief setId
     * @param id
     * Change annotation's id by new ID id
     */
    void setId(unsigned id){mId = id;}

private:
    unsigned mId;
    AnnotationItem *item;
    RevisionAnnotationList revisions;
};

/// We want to register CompleteAnnotationItem as a QVariant
#include <QVariant>
Q_DECLARE_METATYPE(CompleteAnnotationItem*)

#endif // COMPLETEANNOTATIONITEM_H
