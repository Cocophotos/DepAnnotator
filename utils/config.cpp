#include "config.h"

Config *Config::_singleton = NULL;

void Config::setDefaultDirectory(QString path)
{
    settings.setValue("general/default_dir", path);
    settings.sync();
}

QString Config::getDefaultDirectory()const
{
    return settings.value("general/default_dir", "").toString();
}

void Config::setViewRevisions(bool checked)
{
    settings.setValue("interface/view_revisions", checked);
    settings.sync();
}

bool Config::getViewRevisions() const
{
    return settings.value("interface/view_revisions", false).toBool();
}

void Config::setViewOriginalParse(bool checked)
{
    settings.setValue("interface/view_original", checked);
    settings.sync();
}

bool Config::getViewOriginalParse() const
{
    return settings.value("interface/view_original", false).toBool();
}

void Config::setLastAnnotationScheme(QString filepath)
{
    settings.setValue("annotation/lastscheme", filepath);
    settings.sync();
}

QString Config::getLastAnnotationScheme() const
{
    return settings.value("annotation/lastscheme", "").toString();
}

