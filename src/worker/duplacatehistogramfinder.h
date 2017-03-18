#ifndef DUPLACATEHISTOGRAMFINDER_H
#define DUPLACATEHISTOGRAMFINDER_H

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

typedef struct _Local_Hist_FInfo1 {
    QString fileName;
    qint64 size;
    uint height, width;
    cv::MatND hist;
    double diff;
} Local_Hist_FInfo1;

typedef struct _Local_Hist_Settings1 {
    int hist_compare_method;
    double epsilon;
} Local_Hist_Settings1;

class DuplacateHistogramFinder : public DirWalker
{
    Q_OBJECT
public:
    explicit DuplacateHistogramFinder(QObject *parent = nullptr);
    virtual ~DuplacateHistogramFinder() {}
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;
    void setSettings(const Local_Hist_Settings1 &settings) {this->settings=settings;}

private:
    Local_Hist_Settings1 settings;
    QList<Local_Hist_FInfo1> images;
    QList<Local_Hist_FInfo1 *> images_p;
    QSharedPtrListHFIS result;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // DUPLACATEHISTOGRAMFINDER_H
