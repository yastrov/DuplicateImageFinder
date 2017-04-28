#include "diplicatesiftdistancefinder.h"

DiplicateSIFTDistanceFinder::DiplicateSIFTDistanceFinder(QObject *parent) :
    DirWalker(parent), detector(0.05, 5.0 ), extractor(0.3)
{

}

void DiplicateSIFTDistanceFinder::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplacateHistogramFinder::process";
#endif
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    images.reserve(total_files);
    processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    const double epsilon = settings.epsilon;
    quint64 current_group = 1;
    QMultiHash<quint64, Local_SIFT_FInfo *> store;
    Local_SIFT_FInfo *base, *second;
    store.reserve(total_files);
    for(q_coll_s_t i=0; i<total_files; ++i)
    {
        base = images_p[i];
        if(base != nullptr) {
            base->diff=1.;
            store.insert(current_group, base);
            for(q_coll_s_t j=i+1; j<total_files; ++j)
            {
                second = images_p[j];
                if(second != nullptr) {
                    const double dist = cv::norm(base->descriptors, second->descriptors, cv::NORM_L2); // l2 for surf,sift
                    second->diff = dist;
                    if(dist < epsilon) {
                        images_p[j] = nullptr;
                        store.insert(current_group, second);
                    }
                }
            }
            ++current_group;
        }
    }
    //
    // reduceToResult
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
    result.data()->reserve(store.size());
    QSetIterator<quint64> it(store.keys().toSet());
    while(it.hasNext())
    {
        const quint64 &key = it.next();
        QList<Local_SIFT_FInfo *> values = store.values(key);
        if(values.length() < 2)
            continue;

        std::sort(values.begin(), values.end(), [](Local_SIFT_FInfo *x, Local_SIFT_FInfo *y) {
            return x->height*x->width > y->height*y->width;
        });
        QListIterator<Local_SIFT_FInfo *> it2(values);
        Local_SIFT_FInfo *value = it2.next();
        HashFileInfoStruct st;
        st.checked = false;
        st.fileName = value->fileName;
        st.width = value->width;
        st.height = value->height;
        st.groupID = key;
        st.diff = value->diff;
        result->append(st);
        while(it2.hasNext())
        {
            value = it2.next();
            HashFileInfoStruct _st;
            _st.checked = true;
            _st.fileName = value->fileName;
            _st.width = value->width;
            _st.height = value->height;
            _st.groupID = key;
            _st.diff = value->diff;
            result->append(_st);
        }
    }
    // Clear all
    store.clear();
    images_p.clear();
    images.clear();
    //
    emit currentProcessedFiles(result->size());
    emit finishedWData(result);
    emit finished();
}

void DiplicateSIFTDistanceFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplacateHistogramQFinder::processFile "<<fileName;
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
        Local_SIFT_FInfo st;
        st.fileName = fileName;
        st.size = fInfo.size();
        cv::Mat img = cv::imread(fileName.toStdString());
        if(!img.data) return;

        st.width = img.cols;
        st.height = img.rows;
        std::vector<cv::KeyPoint> keypoints;
        detector.detect(img, keypoints);
        extractor.compute( img, keypoints, st.descriptors);

        images.push_back(st);
        images_p.push_back(&images.last());
        ++processed_files;
        if(processed_files % SAY_PROGRESS_EVERY == 0)
            emit currentProcessedFiles(processed_files);
    }
}
