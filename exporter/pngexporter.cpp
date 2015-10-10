#include "pngexporter.h"
#include <QtGui>

bool PNGExporter::exporting(QGraphicsView *view, QString filepath)
{
    if(view->scene())
    {
        QImage img(view->scene()->sceneRect().width(),
                   view->scene()->sceneRect().height(),
                   QImage::Format_ARGB32_Premultiplied);
        QPainter p(&img);
        view->scene()->render(&p);
        p.end();
        img.save(filepath, 0, 80);
        return true;
    }
    return false;
}
