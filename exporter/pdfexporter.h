#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include "exporter/exporter.h"

/**
 * @brief The PDFExporter class
 * Export scene's content into a PDF file
 */
class PDFExporter : public Exporter
{
public:
    explicit PDFExporter(){}

    /**
     * @brief exporting
     * @see Exporter for more details
     */
    bool exporting(QGraphicsView *view, QString filepath);
};

#endif // PDFEXPORTER_H
