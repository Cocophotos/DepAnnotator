#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QFrame>
#include <QGroupBox>
#include <QProgressDialog>
#include <QSpinBox>
#include <QUndoStack>

#include "utils/config.h"


#include "project/scheme/schemereader.h"

#include "ui/projectmanagerwidget.h"
#include "ui/originalparsewidget.h"
#include "project/projectmanager.h"
#include "ui/projectwindow.h"
#include "scene/scenecreator.h"
#include "ui/relationswidget.h"
#include "ui/commentplainedit.h"
#include "ui/logconsolewidget.h"
#include "ui/preferencesdlg.h"
#include "ui/annotationtimer.h"

//------- Exporter ------
#include "exporter/exporter.h"
#include "exporter/svgexporter.h"
#include "exporter/pdfexporter.h"
#include "exporter/pngexporter.h"


class DependenciesAnnotator: public QMainWindow
{
    Q_OBJECT

public:
    DependenciesAnnotator(QWidget *parent = 0);

public slots:
    void newProject();
    void openProject();
    void importFile();
    void saveProject();
    void saveProjectAs();
    void exportProject();
    void exportInto();
    void loadAnnotationScheme();

    void commit();
    void rollback();
    void enableActions();

    void showGraph(const QModelIndex &index);
    void redrawScenes();
    void zoomIn();
    void zoomOut();
    void fitToView();
    void viewLogConsole();

    void updateRevision(int count, int i, QString status);
    void changeRevision();

    void changeSentence();
    void enableSave();
    void finishOpen(bool success);

    void about();
    void showOptions();

    //! Trigger when QApplication loses focus
    void onAppFocusChanged();

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool canExecute();

    void createFileActions();
    void createEditionActions();
    void createViewActions();
    void createHelpActions();

private:
    ProjectManager *projectManager;
    QDockWidget *projectPane,
                *relsPane,
                *originalParsePane,
                *annotationTimerPane;

    SceneCreator *currentSceneCreator,
                 *originalSceneCreator;        

    ProjectManagerWidget *projectManagerWidget;
    OriginalParseWidget *originalParseWidget;
    RelationsWidget *relationsWidget;

    QGraphicsView *currentParseView;

    QFrame *revisionsFrame;
    QLabel *revisionsStatus;
    QPushButton *revisionRollback;
    QSpinBox *revisionsSpinBox;

    AnnotationTimer *annotationTimer;

    QGroupBox *currentGroupBox;

    QUndoStack *undoStack;

    QMenu *fileMenu,
          *editionMenu,
          *viewMenu,
          *helpMenu;

    QAction *quitAction,
            *prefAction,
            *aboutAction;

    QAction *undoAction,
            *redoAction;

    QAction *newProjectAction,
            *openProjectAction,
            *importAction,
            *saveProjectAction,
            *saveProjectAsAction,
            *saveConllAction,
            *exportAction,
            *loadAnnotationSchemeAction;

    QAction *commitAction,
            *generateDepLoriaAction,
            *prevSentenceAction,
            *nextSentenceAction;

    QAction *addEdgeAction,
            *removeEdgeAction,
            *invertEdgeAction,
            *deepEdgeAction,
            *surfacicEdgeAction,
            *bothEdgeAction,
            *noneEdgeAction,
            *addNodeAction,
            *coindexNodesAction;
            /**viewLemmaAction;
            *viewPOSAction;*/

    QAction *viewRevisions,
            *zoomInAction,
            *zoomOutAction,
            *fitToViewAction,
            *viewLogConsoleAction;

    QProgressDialog *waitForSaving,
                    *waitForOpening;

    CommentPlainEdit *commentBox;
    QPushButton *commentBtn;

    bool isSaved;
};

#endif
