#ifndef DIPLICATESIFTDISTANCEFINDER_H
#define DIPLICATESIFTDISTANCEFINDER_H

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
#include <opencv2/features2d.hpp>
#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4
  #include <opencv2/nonfree/features2d.hpp>
#endif
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/xfeatures2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//#include <opencv2/ml/ml.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/xfeatures2d/nonfree.hpp"
#include <algorithm> // For sorting
#include "../constants.h"

typedef struct _Local_SIFT_FInfo {
    QString fileName;
    qint64 size;
    uint height, width;
    cv::Mat descriptors;
    double diff;
} Local_SIFT_FInfo;

typedef struct _Local_SIFT_Settings {
    //int hist_compare_method;
    int minHessian;
    double epsilon;
} Local_SIFT_Settings;

class DiplicateSIFTDistanceFinder : public DirWalker
{
    Q_OBJECT
public:
    explicit DiplicateSIFTDistanceFinder(QObject *parent = nullptr);
    virtual ~DiplicateSIFTDistanceFinder() {}
    void processFile(const QString &fileName) Q_DECL_OVERRIDE;
    void setSettings(const Local_SIFT_Settings &settings) {this->settings=settings;
                                                          //this->detector.setMinHessian(settings.minHessian);
                                                          }

private:
    Local_SIFT_Settings settings;
    QList<Local_SIFT_FInfo> images;
    QList<Local_SIFT_FInfo *> images_p;
    QSharedPtrListHFIS result;
    cv::SiftFeatureDetector detector;
    cv::SiftDescriptorExtractor extractor;

signals:
    void finishedWData(QSharedPtrListHFIS itemsPtr);

public slots:
    void process();
};

#endif // DIPLICATESIFTDISTANCEFINDER_H
