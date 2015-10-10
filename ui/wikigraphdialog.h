#ifndef WIKIGRAPHDIALOG_H
#define WIKIGRAPHDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QPlainTextEdit>
#include <QHBoxLayout>

#include "graph/conllgraph.h"


class WikiGraphDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WikiGraphDialog(ConllGraph *graph, QWidget *parent = 0);
    
    void generate();

public slots:
    //void readOnStdOut();

private:
    //QProcess *perlProcess;
    QPlainTextEdit *te;
    QHBoxLayout *mainLayout;

    ConllGraph *mGraph;
};

#endif // WIKIGRAPHDIALOG_H
