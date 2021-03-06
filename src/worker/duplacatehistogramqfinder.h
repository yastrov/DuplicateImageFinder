#ifndef DUPLACATEHISTOGRAMQFINDER_H
#define DUPLACATEHISTOGRAMQFINDER_H

#include <QObject>
#include <QList>
#include <QFileInfo>
#include <QThread>
#include "../hashfileinfostruct.h"
#include "../worker/dirwalker.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <algorithm> // For sorting
#include "../constants.h"
#include "duplacatehistogramfinder.h"

typedef struct _Local_Hist_FInfo2 {
    QString fileName;
    qint64 size;
    uint height, width;
    cv::MatND hist[4];
    double diff[4];
} Local_Hist_FInfo2;

class DuplacateHistogramQFinder : public DirWalker
{
    Q_OBJECT
public:
    explicit DuplacateHistogramQFinder(QObject *parent = nullptr);
    virtual ~DuplacateHistogramQFinder() {}
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;
    void setSettings(const Local_Hist_Settings1 &settings) {this->settings=settings;}

private:
    Local_Hist_Settings1 settings;
    QList<Local_Hist_FInfo2> images;
    QList<Local_Hist_FInfo2 *> images_p;
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // DUPLACATEHISTOGRAMQFINDER_H
