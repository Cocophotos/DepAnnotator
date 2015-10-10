#include "pdfexporter.h"

#if QT_VERSION >= 0x050000
    #include <QtWidgets>
    #include <QtGui>
    #include<QtPrintSupport/QPrinter>
#else
    #include <QtGui>
#endif

bool PDFExporter::exporting(QGraphicsView *view, QString filepath)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOrientation(QPrinter::Landscape);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A3);
    printer.setOutputFileName(filepath);

    QPainter painter(&printer);
    if(view->scene())
    {
        view->scene()->render(&painter);
        return true;
    }
    return false;
}
