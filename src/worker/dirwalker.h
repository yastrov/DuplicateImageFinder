#ifndef DIRWALKER_H
#define DIRWALKER_H

#include <QObject>
#include <QDir>
#include <QList>
#include <QVector>
#include <QDirIterator>
#include <QThread>

class DirWalker : public QObject
{
    Q_OBJECT
public:
    explicit DirWalker(QObject *parent = nullptr);
    virtual ~DirWalker();
    void setQDir(const QList<QDir> &rootDirs);
    void setQDir(const QVector<QDir> &rootDirs);
    void setQDir(const QStringList &rootDirs);
    void setQDir(const QVector<QString> &rootDirs);
    void setFilters(const QStringList &filters);

protected:
    bool stopped;
    quint64 total_files;
    quint64 processed_files;
    virtual void processFilesRecursively(const QDir &rootDir);
    void processFilesRecursively();
    QList<QDir> rootDirs;
    virtual void processFile(const QString &fileName) = 0;
    void calcTotalFiles();

signals:
    void finished();
    void sayTotalFiles(quint64 total_files);
    void currentProcessedFiles(quint64 count);

public slots:
};

#endif // DIRWALKER_H
