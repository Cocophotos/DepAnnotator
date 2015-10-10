#ifndef PNGEXPORTER_H
#define PNGEXPORTER_H

#include "exporter/exporter.h"

/**
 * @brief The PNGExporter class
 * Export scene's content into a PNG file
 */
class PNGExporter : public Exporter
{
public:
    explicit PNGExporter(){}

    /**
     * @brief exporting
     * @see Exporter for more details
     */
    bool exporting(QGraphicsView *view, QString filepath);
};
#endif // PNGEXPORTER_H
