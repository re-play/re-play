#include "core/plugins/pluginloader.h"


namespace RePlay
{
    namespace Core
    {
        namespace Plugins
        {

            PluginLoader::PluginLoader(QObject *parent) : QObject(parent), pFolderWatcher(nullptr)
            {
                connect(pFolderWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged(QString)));
            }

            void PluginLoader::initialize(QString folder)
            {
                if (pFolderWatcher)
                    delete pFolderWatcher;

                pFolderWatcher = new QFileSystemWatcher();

                if (!QDir(folder).exists())
                {
                    QDir().mkdir(folder);
                }

                pPlugFolder = QDir(folder).absoluteFilePath("");

                // - Watch folder
                pFolderWatcher->addPath(pPlugFolder);

                emit initialized();
            }

            QString PluginLoader::getPluginFolder() const
            {
                return pPlugFolder;
            }

            QStringList PluginLoader::getAvaiablePlugins()
            {
                QDir directory(pPlugFolder);

                return directory.entryList(QStringList("*.dll"));
            }

            void PluginLoader::loadPlugin(QString path)
            {
                if (QFile::exists(path))
                {
                    QFile::copy(path, pPlugFolder);
                }

                auto plug = getPlugin(path);
            }

            IPlugin *PluginLoader::getPlugin(QString path)
            {
                QPluginLoader plugin(path);

                qDebug(plugin.errorString().toStdString().c_str());

                return reinterpret_cast<IPlugin*>(plugin.instance());
            }

            void PluginLoader::handleDirectoryChanged(QString dir)
            {

            }

        }
    }
}
