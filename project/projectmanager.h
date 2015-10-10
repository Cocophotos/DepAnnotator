#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QWidget>
#include <QTreeView>
#include <QModelIndex>
#include <QFutureWatcher>
#include <QPlainTextEdit>

#include "project/projectmodel.h"
#include "scene/scenecreator.h"
#include "project/conllproject.h"
#include "ui/wikigraphdialog.h"

class ProjectManager: public QObject
{
    Q_OBJECT
public:
    explicit ProjectManager(QTreeView *_projectTree,
                            SceneCreator *currentSceneCreator,
                            SceneCreator *originalSceneCreator);

    void addProject(QString name, QString path, QStringList files);
    const Project* getProject()const {return project;}

public slots:
    void openProject(QString filepath);
    void exportProject(QString path);
    void importFile(QString path, QStringList files);    
    void showGraph(const QModelIndex& index);
    void commit();
    void save(QString path = "");
    void changeRevision(int n);
    void rollbackToRevision(int n);
    void saveCompleted();
    void openCompleted();

    void generateWikiGraph();

signals:
    void scenesChanged();
    void revisionChanged(int count, int i, QString status);
    void saved();
    void opened(bool o);
    void canEnable();
    
private:
    QTreeView *projectTree;   

    SceneCreator *mCurrentSceneCreator,
                 *mOriginalSceneCreator;

    bool committed;
    QModelIndex currentIndex;

    Project *project;

    QFutureWatcher<void> *watcher;

    bool autocommit;

    WikiGraphDialog *wikiDlg;

};

#endif // PROJECTMANAGER_H
