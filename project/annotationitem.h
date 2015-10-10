#ifndef ANNOTATIONITEM_H
#define ANNOTATIONITEM_H

#include <QHash>
#include <QPair>
#include <QList>

#include "project/revision.h"
#include "graph/conllgraph.h"

/**
 * @brief The AnnotationItem class
 * Describes an annotation item.
 * An annotation item has
 * - an Id
 * - a graph
 * - the original graph
 *
 * @todo Remove original graph
 * @todo enhance editable graph process
 */
class AnnotationItem
{
public:
    enum Status{NotStarted = 0, InProgress, Finished};
    /**
     * @brief AnnotationItem
     * Default constructor
     */
    AnnotationItem();

    /**
     * @brief AnnotationItem
     * @param _id is the annotation ID
     * @param orig is the original graph
     * @param current is the current graph
     */
    AnnotationItem(uint _id, ConllGraph *orig, ConllGraph *current);

    /**
     * @brief AnnotationItem
     * Copy constructor
     * @param other
     */
    AnnotationItem(const AnnotationItem &other);

    /**
     * Destructor
     */
    ~AnnotationItem();

    /**
     * @brief getOriginalParse
     * @return original graph
     */
    ConllGraph* getOriginalParse() const {return originalParse;}

    /**
     * @brief getCurrentParse
     * @return current graph
     */
    ConllGraph* getCurrentParse() const {return currentParse;}

    /**
     * @brief getEditableParse
     * @return editable graph
     * @see cloneFromCurrentToEditable
     */
    ConllGraph* getEditableParse() const{return editableParse;}

    /**
     * @brief cloneFromCurrentToEditable
     *
     * Editable graph is a trick to make commit
     * and rollback easier.
     *
     * We copy the current graph into another graph
     * and the user always works on this copy.
     * When he commits it's work, we replace the current
     * graph by the edited one
     *
     * This function takes care of copying the current graph
     * into a new editable graph
     */
    void cloneFromCurrentToEditable()
    {        
        editableParse = new ConllGraph(*currentParse);
    }


    /**
     * @brief cloneFromEditableToCurrent
     * @see cloneFromCurrentToEditable for more details
     * Copy from editable to current graph
     */
    void cloneFromEditableToCurrent()
    {
        if(editableParse)
            currentParse = new ConllGraph(*editableParse);
    }

    /**
     * @brief toString
     * @return a string representation of the item
     * it's necessary when we want to display the sentence
     * into the project panel
     */
    QString toString() const {
        ConllGraph::ordered_node_iter nb, ne;
        QString str = "";
        for(boost::tie(nb, ne) = currentParse->orderedNodes(); nb != ne; ++nb)
            str += currentParse->nodeProperties(nb->first).token() + " ";
        return str;
    }

    /**
     * @brief getId
     * @return annotation's id
     */
    uint getId()const {return id;}  

    /**
     * @brief getStatus
     * @return annotation status :
     */
    Status status() const {return m_status;}

    void setStatus(Status s) {m_status = s;}

private:
    uint id;
    ConllGraph *originalParse,
            *currentParse;
    ConllGraph *editableParse;
    Status m_status;
};

#endif // ANNOTATIONITEM_H
