#ifndef DUPLICATEFINDER_H
#define DUPLICATEFINDER_H
#include <QObject>
#include <QLinkedList>
#include <QList>
#include <QFileInfo>
#include <QThread>
#include "../hashfileinfostruct.h"
#include "../worker/dirwalker.h"
#include "../constants.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class DuplicateFinder : public DirWalker
{
    Q_OBJECT
public:
    DuplicateFinder(QObject *parent = nullptr);
    virtual ~DuplicateFinder();
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;

private:
    QMultiHash<QByteArray, HashFileInfoStruct> hashItems;
    void clearNoDuplicatedSize();
    void reduceToResult();
    int resultCount;
    void dHash(HashFileInfoStruct& strct, size_t hashSize=8);
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();

};

#endif // DUPLICATEFINDER_H
