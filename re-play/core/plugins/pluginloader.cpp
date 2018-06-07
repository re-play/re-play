#include "core/plugins/pluginloader.h"


namespace RePlay
{
    namespace Core
    {
        namespace Plugins
        {

            PluginLoader::PluginLoader(QObject *parent)
                : QObject(parent),
                  pFolderWatcher(nullptr),
                  pPluginRegister(nullptr),
                  pPlugFolder("plugins")
            {
                connect(pFolderWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(handleDirectoryChanged(QString)));
                connect(pFolderWatcher, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChanged(QString)));
            }

            void PluginLoader::initialize(QString folder)
            {
                { // - Initialize plugin folder

                    if (pFolderWatcher)
                        delete pFolderWatcher;

                    pFolderWatcher = new QFileSystemWatcher();

                    if (!QDir(folder).exists())
                    {
                        QDir().mkdir(folder);
                    }

                    QDir dir(QCoreApplication::applicationDirPath());
                    pPlugFolder = dir.absoluteFilePath(folder);

                    // - Watch folder
                    pFolderWatcher->addPath(pPlugFolder);

                }

                { // - Initialize plugins.json

                    QFile jsonFile(pPlugFolder + "/plugins.json");

                    jsonFile.open(QIODevice::ReadOnly);

                    if (pPluginRegister)
                        delete pPluginRegister;

                    pPluginRegister = new QJsonDocument(QJsonDocument::fromJson(jsonFile.readAll()));

                    jsonFile.close();
                }

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
                    QFile from(path);
                    QFileInfo fileInfo(from.fileName());
                    from.copy("C:\\Users\\PizzaKun\\Documents\\Projects\\build-re-play-Desktop_Qt_5_10_0_MSVC2017_64bit-Debug\\re-play\\plugins\\" + fileInfo.fileName());
                    if (from.error() != QFile::NoError)
                        qDebug(from.errorString().toStdString().c_str());

                    //path = pPlugFolder + QFile(path).fileName();
                }

                auto plug = getPlugin(path);
                auto plugName = plug->getName();
                auto version = plug->getVersion();

                QJsonObject row = pPluginRegister->object();
                row[plugName] = version;

                QFile jsonFile("C:\\Users\\PizzaKun\\Documents\\Projects\\build-re-play-Desktop_Qt_5_10_0_MSVC2017_64bit-Debug\\re-play\\plugins\\plugins.json");

                delete pPluginRegister;
                pPluginRegister = new QJsonDocument(row);
                jsonFile.open(QIODevice::WriteOnly);
                jsonFile.write(pPluginRegister->toJson());
                jsonFile.close();
            }

            IPlugin *PluginLoader::getPlugin(QString path)
            {
                QPluginLoader plugin(path);

                qDebug(plugin.errorString().toStdString().c_str());

                return dynamic_cast<IPlugin*>(plugin.instance());
            }

            void PluginLoader::handleDirectoryChanged(QString dir)
            {
                emit pluginFolderChanged();
            }

            void PluginLoader::handleFileChanged(QString dir)
            {
                emit pluginChanged(dir);
            }

        }
    }
}
