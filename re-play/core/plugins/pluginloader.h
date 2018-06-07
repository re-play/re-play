#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QRegularExpression>
#include <QFileSystemWatcher>
#include <QCoreApplication>
#include <QPluginLoader>
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QDir>

#include <core/plugins/iplugin.h>

namespace RePlay
{
    namespace Core
    {
        namespace Plugins
        {

            class PluginLoader : public QObject
            {
                Q_OBJECT

                Q_PROPERTY(QString pluginFolder READ getPluginFolder NOTIFY initialized)

            public:

                explicit PluginLoader(QObject *parent = nullptr);

                // - Management
                // ------------------------------------------------
                Q_INVOKABLE void initialize(QString folder);

                // - Folders management
                // ------------------------------------------------
                Q_INVOKABLE QString getPluginFolder() const;
                Q_INVOKABLE QStringList getAvaiablePlugins();

                // - Loading
                // ------------------------------------------------
                Q_INVOKABLE void loadPlugin(QString path);
                Q_INVOKABLE IPlugin *getPlugin(QString path);



            signals:

                // - Triggered on initialize call
                // ------------------------------------------------
                void initialized();

                // - Triggered when a change occurs in the plugin folder
                // ------------------------------------------------
                void pluginChanged(QString file);
                void pluginFolderChanged();

            public slots:

                // - Handles directory changed
                // ------------------------------------------------
                void handleDirectoryChanged(QString dir);
                void handleFileChanged(QString dir);

            private:

                // - Plugin base folder
                QString pPlugFolder;

                // - Folder checker
                QFileSystemWatcher *pFolderWatcher;

                // - Plugin register
                QJsonDocument *pPluginRegister;

            };

        }
    }
}

#endif // PLUGINLOADER_H
