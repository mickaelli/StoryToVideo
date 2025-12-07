#include "DataManager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
{
}

QString DataManager::getStoragePath(const QString &fileName)
{
    // 使用系统标准的应用程序数据目录 (例如: ~/Library/Application Support/StoryToVideoGenerator/data/)
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/data/";

    QDir dir(dirPath);
    if (!dir.exists())
        dir.mkpath(dirPath);

    return dirPath + fileName;
}

bool DataManager::saveData(const QVariantMap &storyData, const QString &fileName)
{
    QString path = getStoragePath(fileName);

    QJsonObject jsonObj = QJsonObject::fromVariantMap(storyData);
    QJsonDocument doc(jsonObj);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "保存成功:" << path;
    emit fileSaved(path);
    return true;
}

QVariantMap DataManager::loadData(const QString &fileName)
{
    QString path = getStoragePath(fileName);

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "加载失败，文件不存在:" << path;
        return QVariantMap();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVariantMap map = doc.object().toVariantMap();

    qDebug() << "加载成功:" << path;
    emit fileLoaded(path);

    return map;
}

bool DataManager::clearData(const QString &fileName)
{
    QString path = getStoragePath(fileName);

    if (QFile::exists(path)) {
        QFile::remove(path);
        qDebug() << "删除成功:" << path;
        emit fileCleared(path);
        return true;
    }

    qDebug() << "删除失败，文件不存在:" << path;
    return false;
}
