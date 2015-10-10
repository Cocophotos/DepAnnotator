#ifndef CONLLPROJECT_H
#define CONLLPROJECT_H

#include "project/project.h"
#include "utils/logconsole.h"
#include <QTextStream>

/**
 * @brief The ConllProject class
 *
 * Describes a CoNLL project :
 * Current CoNLL files and revisions
 *
 * @todo Merge with ProjectModel
 * because it seems to be kind of a doublon
 */
class ConllProject: public Project
{
public:
    /**
     * @brief ConllProject
     * @param name
     * @param path
     * @see Project for more details
     */
    ConllProject(QString name, QString path);

    /**
     * @brief addFile
     * @param filepath
     * @return true on success
     *
     * @see Project for more details
     */
    bool addFile(QString filepath);

    /**
     * @brief exportProject
     * @param path
     * @return true on success
     *
     * @see Project for more details
     */
    bool exportProject(QString path);

    /**
     * @brief saveProject
     * @return true on success
     *
     * @see Project for more details
     */
    bool saveProject();

    /**
     * @brief openProject
     * @return true on success
     *
     * @see Project for more details
     */
    bool openProject();


    static QString generateConll(ConllGraph *graph);
    static bool generateGraph(QString conll, ConllGraph *g);
    static QString normalise(QString string);
    static QString denormalise(QString string);

private:
    static QMap<QString, QString> processFeatures(QString features);
    static bool processEdge(QString id, QString heads, QString rels, QMultiMap<QString, QPair<QString, ConllEdgeProperties> > &edges);

    void processDap(unsigned id, QString conll, QString &revisionTimestamp,
                    QList<CompleteAnnotationItem*> &items,
                    RevisionAnnotationList &revisions);   

    void saveAnnotation(AnnotationItem *annotation, QTextStream &out);
    void saveRevision(Revision<AnnotationItem*> revision, QTextStream &out);

private:
    ConllGraph *originalParse;
};


#endif // CONLLPROJECT_H
