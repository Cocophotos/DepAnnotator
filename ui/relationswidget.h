#ifndef RELATIONSWIDGET_H
#define RELATIONSWIDGET_H

#include <QWidget>
#include <QTreeWidget>

#include "project/scheme/schemereader.h"

class RelationsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RelationsWidget(QWidget *parent = 0);
    QTreeWidget *tree()const {return relationsTree;}
    SchemeReader *getReader()const {return reader;}

public slots:
    void populate(QString filepath);

private:
    QTreeWidget *relationsTree;

    SchemeReader *reader;


    
};

#endif // RELATIONSWIDGET_H
