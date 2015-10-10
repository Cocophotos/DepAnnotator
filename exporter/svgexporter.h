#ifndef SVGEXPORTER_H
#define SVGEXPORTER_H

#include "exporter/exporter.h"

/**
 * @brief The SVGExporter class
 * Export scene's content into SVG file
 */
class SVGExporter: public Exporter
{
public:
    SVGExporter(){}

    /**
     * @brief exporting
     * @see Exporter for more details
     */
    bool exporting(QGraphicsView *view, QString filepath);
private:
};

#endif // SVGEXPORTER_H
