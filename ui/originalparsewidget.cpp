#include "originalparsewidget.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
#else
    #include <QtGui>
#endif

OriginalParseWidget::OriginalParseWidget(QWidget *parent) :
    QWidget(parent)
{
    treeView = new QGraphicsView(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(treeView);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(0,0,0,0);
}
