#ifndef DIRWALKER_H
#define DIRWALKER_H

#include <QObject>
#include <QDir>
#include <QList>
#include <QVector>
#include <QDirIterator>
#include <QThread>
#include "../constants.h"

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
    q_coll_s_t total_files;
    q_coll_s_t processed_files;
    virtual void processFilesRecursively(const QDir &rootDir);
    void processFilesRecursively();
    QList<QDir> rootDirs;
    virtual void processFile(const QString &fileName) = 0;
    void calcTotalFiles();

signals:
    void finished();
    void sayTotalFiles(q_coll_s_t total_files);
    void currentProcessedFiles(q_coll_s_t count);

public slots:
};

#endif // DIRWALKER_H
