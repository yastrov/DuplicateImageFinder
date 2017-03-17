#include "duplacatehistogramfinder.h"

DuplacateHistogramFinder::DuplacateHistogramFinder(QObject *parent) :
    DirWalker(parent)
{

}

void DuplacateHistogramFinder::process()
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
    images_p.reserve(total_files);
    processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }

    // Calc difference betwen image histograms
    Local_Hist_FInfo1 *base, *second;
    double diff;
    const int compare_method = settings.hist_compare_method;
    const double epsilon = settings.epsilon;
    size_t current_group = 1;
    QMultiHash<size_t, Local_Hist_FInfo1 *> store;
    for(int i=0; i<total_files; ++i)
    {
        base = images_p[i];
        if(base != nullptr) {
            store.insert(current_group, base);
            for(int j=i+1; j<total_files; ++j)
            {
                second = images_p[j];
                if(second != nullptr) {
                    diff = compareHist( base->hist, second->hist, compare_method);
                    if(diff < epsilon) {
                        images_p[j] = nullptr;
                        store.insert(current_group, second);
                    }
                }
            }
            ++current_group;
        }
    }
    // reduceToResult
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplacateHistogramFinder::reduceToResult:: reserve memory for result";
#endif
    result.data()->reserve(store.size());
    QSetIterator<size_t> it(store.keys().toSet());
    while(it.hasNext())
    {
        const size_t &key = it.next();
        QList<Local_Hist_FInfo1 *> values = store.values(key);
        if(values.length() < 2)
            continue;

        std::sort(values.begin(), values.end(), [](Local_Hist_FInfo1 *x, Local_Hist_FInfo1 *y) {
            return x->height*x->width < y->height*y->width;
        });
        QListIterator<Local_Hist_FInfo1 *> it2(values);
        Local_Hist_FInfo1 *value = it2.next();
        HashFileInfoStruct st;
        st.checked = false;
        st.fileName = value->fileName;
        st.width = value->width;
        st.height = value->height;
        st.groupID = key;
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

void DuplacateHistogramFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplacateHistogramFinder::processFile "<<fileName;
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
        Local_Hist_FInfo1 st;
        st.fileName = fileName;
        st.size = fInfo.size();
        cv::Mat img = cv::imread(fileName.toStdString());

        st.width = img.cols;
        st.height = img.rows;

        cv::Mat hsv_img;
        /// Convert to HSV
        cv::cvtColor(img , hsv_img, cv::COLOR_BGR2HSV);
        /// Using 50 bins for hue and 60 for saturation
        int h_bins = 50; int s_bins = 60;
        int histSize[] = { h_bins, s_bins };

        // hue varies from 0 to 179, saturation from 0 to 255
        float h_ranges[] = { 0, 180 };
        float s_ranges[] = { 0, 256 };

        const float* ranges[] = { h_ranges, s_ranges };

        // Use the o-th and 1-st channels
        int channels[] = { 0, 1 };

        cv::MatND hist_img;
        cv::calcHist(&hsv_img, 1, channels, cv::Mat(), hist_img, 2, histSize, ranges, true, false);
        cv::normalize(hist_img, hist_img, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        st.hist = hist_img;
        images.push_back(st);
        images_p.push_back(&images.last());
        ++processed_files;
        if(processed_files % SAY_PROGRESS_EVERY == 0)
            emit currentProcessedFiles(processed_files);
    }
}
