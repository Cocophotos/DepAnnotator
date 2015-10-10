#ifndef EXPORTER_H
#define EXPORTER_H

#include <QGraphicsView>
#include <QString>

/**
 * @brief The Exporter class
 * Pure virtual class that need to be reimplemented
 * It's used to export the scene's content into one particular format
 */
class Exporter
{
public:
    /**
     * @brief Exporter
     */
    Exporter(){}

    /**
     * @brief exporting is a pure virtual function
     * @param view is the QGraphicsView that holds the scene
     * @param filepath is the path of the file
     * @return true on success and false otherwise
     */
    virtual bool exporting(QGraphicsView *view, QString filepath) = 0;
};

#endif // EXPORTER_H
