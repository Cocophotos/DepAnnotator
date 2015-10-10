#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QList>
#include <stdexcept>

#include "project/completeannotationitem.h"

typedef QPair<QString, QList<CompleteAnnotationItem*> > StringAndAnnotations;

class Project
{
public:
    /**
     * @brief Constructor
     * @param name : project's name
     * @param path : project's path
     */
    Project(QString name, QString path)
        :mProjectName(name+".dap"), mProjectPath(path){}

    ///////////////////////////////
    // Pure virtual functions
    ///////////////////////////////
    /**
     * @brief addFile : add a file into the project
     * @param filepath : file path
     * @return true if success, false otherwise
     */
    virtual bool addFile(QString filepath) = 0;

    /**
     * @brief exportProject : export the project
     * @return true if success, false otherwise
     */
    virtual bool exportProject(QString path) = 0;

    /**
     * @brief saveProject : save the project
     * @return true if success, false otherwise
     */
    virtual bool saveProject() = 0;

    /**
     * @brief openProject : open a existing project
     * @return true if success, false otherwise
     */
    virtual bool openProject() = 0;

    ///////////////////////////////

    /**
     * @brief file : get filename and file content
     * @param i : index of the file
     * @return a pair with the filename and all the annotations
     */
    QPair<QString, QList<CompleteAnnotationItem*> > file(int i) const{ return container.at(i); }

    /**
     * @brief findFile : find the index of a file
     * @param file : file name
     * @return file index, or -1 if file name does not exist
     */
    int findFile(QString file) const
    {
        for(int i = 0; i < container.size(); i++)
        {
            if(container[i].first == file)
                return i;
        }
        return -1;
    }

    /**
     * @brief projectName
     * @return project's name
     */
    QString projectName() const {return mProjectName;}

    /**
     * @brief projectPath
     * @return project's path
     */
    QString projectPath() const {return mProjectPath;}

    /**
     * @brief filesCount
     * @return files number
     */
    int filesCount() const {return container.size();}

    /**
     * @brief totalTrees
     * @return total number of annotation items
     */
    int total()const
    {
        int total = 0;
        for(int i = 0; i < container.size(); i++)
        {
            QPair<QString, QList<CompleteAnnotationItem*> > strAndAnnot = container.at(i);
            total += strAndAnnot.second.size();
        }
        return total;
    }

    //////////////////////////////

    /**
     * @brief changeAnnotation : change the annotation at file index and annot index
     * @param fileIndex : file index
     * @param annotIndex : annotation index
     * @param newAnnot : new annotation (to replace the old one)
     */
    void changeAnnotation(int fileIndex, int annotIndex, CompleteAnnotationItem* newAnnot)
    {
        assert(container.size() > fileIndex);
        assert(container.at(fileIndex).second.size() > annotIndex);

        QList<CompleteAnnotationItem*> annotations = container.at(fileIndex).second;
        annotations.replace(annotIndex, newAnnot);

        QPair<QString, QList<CompleteAnnotationItem*> > newStrAndAnnot(container.at(fileIndex).first, annotations);
        container.replace(fileIndex, newStrAndAnnot);
    }

    /**
     * @brief getAnnotation
     * @param indexFile : file index
     * @param annotIndex : annotation index
     * @return get an annotation at indexFile and annotIndex (or NULL if not exists)
     */
    CompleteAnnotationItem* getAnnotation(int indexFile, int annotIndex){
        if(indexFile < container.size() && annotIndex < container[indexFile].second.size())
            return container.at(indexFile).second.at(annotIndex);
        else
            return NULL;
    }

protected:
    QString mProjectName, mProjectPath;
    QList<QPair<QString, QList<CompleteAnnotationItem*> > > container;
};

#endif // PROJECT_H
