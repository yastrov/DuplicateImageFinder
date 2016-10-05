#include "dirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

DirWalker::DirWalker(QObject *parent) : QObject(parent),
    stopped(false), total_files(0), processed_files(0)
{

}

DirWalker::~DirWalker()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DirWalker::~DirWalker";
#endif
}

void DirWalker::setQDir(const QList<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs);
    QMutableListIterator<QDir> it(this->rootDirs);
    while(it.hasNext()){
        QDir &dir = it.next();
        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot| QDir::NoSymLinks);
    }
}

void DirWalker::setQDir(const QVector<QDir> &rootDirs)
{
    this->rootDirs = QList<QDir>(rootDirs.toList());
    QMutableListIterator<QDir> it(this->rootDirs);
    while(it.hasNext()){
    QDir &dir = it.next();
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot| QDir::NoSymLinks);
    }
}

void DirWalker::setQDir(const QStringList &rootDirs)
{
    QListIterator<QString> it (rootDirs);
    QDir dir;
    while(it.hasNext()) {
        dir = QDir(it.next());
        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot| QDir::NoSymLinks);
        this->rootDirs.append(dir);
    }
}

void DirWalker::setQDir(const QVector<QString> &rootDirs)
{
    QVectorIterator<QString> it (rootDirs);
    QDir dir;
    while(it.hasNext()) {
        dir = QDir(it.next());
        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDot | QDir::NoDotDot| QDir::NoSymLinks);
        this->rootDirs.append(dir);
    }
}

void DirWalker::processFilesRecursively()
{
    QListIterator<QDir> i(rootDirs);
    while (i.hasNext()) {
       processFilesRecursively(i.next());
    }
}

void DirWalker::processFilesRecursively(const QDir &rootDir) {
    QDirIterator it(rootDir, QDirIterator::Subdirectories);
    while(it.hasNext() && !stopped) {
        processFile(it.next());
        if(QThread::currentThread()->isInterruptionRequested())
        stopped=true;
    }
    if(stopped)
    {
        emit finished();
    }
}

void DirWalker::calcTotalFiles()
{
    QListIterator<QDir> it0(rootDirs);
    while(it0.hasNext()) {
        const QDir &dir = it0.next();
        QDirIterator it(dir, QDirIterator::Subdirectories);
        while(it.hasNext() && !stopped) {
            ++total_files;
            it.next();
            if(QThread::currentThread()->isInterruptionRequested())
            stopped=true;
        }
        if(stopped)
        {
            emit finished();
        }
    }
}

void DirWalker::setFilters(const QStringList &filters)
{
    QDir current;
    QMutableListIterator<QDir> it(rootDirs);
    while(it.hasNext())
    {
        current = it.next();
        current.setNameFilters(filters);
        it.setValue(current);
    }
}
