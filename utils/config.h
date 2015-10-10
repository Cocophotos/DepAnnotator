#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

/**
 * @brief The Config class
 * Singleton class to handle software's configuration
 */
class Config
{
public:
    /**
     * @brief config
     * @return An instance of the class
     */
    static Config *config ()
    {
        if (NULL == _singleton)
        {
            _singleton =  new Config;
        }
        return _singleton;
    }

    /**
     * @brief kill
     * Destroy the instance
     */
    static void kill ()
    {
        if (NULL != _singleton)
        {
            delete _singleton;
            _singleton = NULL;
        }
    }

    /**
     * @brief setDefaultDirectory
     * @param path
     * Change the default directory by
     * new path
     *
     * Default directory is the last
     * visited directory when we try
     * to open/import/save a file/project
     */
    void setDefaultDirectory(QString path);

    /**
     * @brief getDefaultDirectory
     * @return the default directory path
     * @see setDefaultDirectory for more details
     */
    QString getDefaultDirectory()const;

    /**
     * @brief setViewRevisions
     * @param checked
     * If checked is true, view the revisions panel
     * by default.
     */
    void setViewRevisions(bool checked);

    /**
     * @brief getViewRevisions
     * @return true if we want to view the revisions panel
     * by default.
     */
    bool getViewRevisions() const;

    /**
     * @brief setViewOriginalParse
     * @param checked
     * If checked is true, view the original parse
     * panel by default.
     */
    void setViewOriginalParse(bool checked);

    /**
     * @brief getViewOriginalParse
     * @return true if we want to view the original
     * parse panel by default
     */
    bool getViewOriginalParse() const;

    /**
     * @brief setLastAnnotationScheme
     * @param filepath
     * Set the latest path of an annotation scheme
     * so that we can reload it automatically at startup
     *
     * @see SchemeReader for more details
     */
    void setLastAnnotationScheme(QString filepath);

    /**
     * @brief getLastAnnotationScheme
     * @return get the latest path of an
     * annotation scheme
     * @see SchemeReader for more details
     */
    QString getLastAnnotationScheme()const;

private:
    Config(){}
    ~Config(){}

private:
    static Config *_singleton;
    QSettings settings;
};

#endif // CONFIG_H
