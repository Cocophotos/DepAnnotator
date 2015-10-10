#include "projectmanagerwidget.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

ProjectManagerWidget::ProjectManagerWidget(QWidget *parent)
    :QWidget(parent)
{
    projectTree = new QTreeView(this);
    //projectTree->setHeaderHidden(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(projectTree);
}
