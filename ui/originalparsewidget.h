#ifndef ORIGINALPARSEWIDGET_H
#define ORIGINALPARSEWIDGET_H

#include <QWidget>
#include <QGraphicsView>

class OriginalParseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OriginalParseWidget(QWidget *parent = 0);

    QGraphicsView* view() const {return treeView;}
        
private:
    QGraphicsView *treeView;
};

#endif // ORIGINALPARSEWIDGET_H
