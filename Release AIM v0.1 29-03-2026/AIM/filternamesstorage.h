#ifndef FILTERNAMESSTORAGE_H
#define FILTERNAMESSTORAGE_H

#include <QMap>
#include <QObject>
#include <QString>

class FilterNamesStorage : public QObject
{
    Q_OBJECT

public:
    FilterNamesStorage(QObject* parent = nullptr);

    void set(const QList<QString>& filePaths);
    void add(const QString& filePath);
    void remove(const QString& fileName);
    QList<QString> getNames() const;
    QList<QString> getFilePaths() const;
    QString getFilePath(const QString& fileName) const;

signals:
    void updated(QList<QString> names);
    void added(QString name);

private:
    bool checkAndAdd(const QString& filePath);

private:
    QMap<QString, QString> filterNames;
};

#endif // FILTERNAMESSTORAGE_H
