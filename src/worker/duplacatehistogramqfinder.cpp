#include "duplacatehistogramqfinder.h"


DuplacateHistogramQFinder::DuplacateHistogramQFinder(QObject *parent) :
    DirWalker(parent)
{

}

void DuplacateHistogramQFinder::process()
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
    Local_Hist_FInfo2 *base, *second;
    bool flag=true;
    double diff;
    const int compare_method = settings.hist_compare_method;
    const double epsilon = settings.epsilon;
    quint64 current_group = 1;
    QMultiHash<quint64, Local_Hist_FInfo2 *> store;
    store.reserve(total_files);
    for(q_coll_s_t i=0; i<total_files; ++i)
    {
        base = images_p[i];
        if(base != nullptr) {
            base->diff[0]=1.;base->diff[1]=1.;base->diff[2]=1.;base->diff[3]=1.;
            store.insert(current_group, base);
            for(q_coll_s_t j=i+1; j<total_files; ++j)
            {
                second = images_p[j];
                if(second != nullptr) {
                    for(int ii=0; ii<4; ++i) {
                        diff = compareHist( base->hist[ii], second->hist[ii], compare_method);
                        second->diff[ii] = diff;
                        if(diff < epsilon) flag=false;
                    }
                    if(flag) {
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
    qDebug() << "DuplacateHistogramQFinder::reduceToResult:: reserve memory for result: "<<store.size();
#endif
    result.data()->reserve(store.size());
    QSetIterator<quint64> it(store.keys().toSet());
    while(it.hasNext())
    {
        const quint64 &key = it.next();
        QList<Local_Hist_FInfo2 *> values = store.values(key);
        if(values.length() < 2)
            continue;

        std::sort(values.begin(), values.end(), [](Local_Hist_FInfo2 *x, Local_Hist_FInfo2 *y) {
            return x->height*x->width > y->height*y->width;
        });
        QListIterator<Local_Hist_FInfo2 *> it2(values);
        Local_Hist_FInfo2 *value = it2.next();
        HashFileInfoStruct st;
        st.checked = false;
        st.fileName = value->fileName;
        st.width = value->width;
        st.height = value->height;
        st.groupID = key;
        st.diff = (value->diff[0]+value->diff[1]+value->diff[2]+value->diff[3])/4.;
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
            _st.diff = (value->diff[0]+value->diff[1]+value->diff[2]+value->diff[3])/4.;;
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

void DuplacateHistogramQFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplacateHistogramQFinder::processFile "<<fileName;
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
        Local_Hist_FInfo2 st;
        st.fileName = fileName;
        st.size = fInfo.size();
        cv::Mat img = cv::imread(fileName.toStdString());

        st.width = img.cols;
        st.height = img.rows;

        cv::Mat hsv_img;
        /// Convert to HSV
        cv::cvtColor(img , hsv_img, cv::COLOR_BGR2HSV);
        // Split
        cv::Mat top_left
           = hsv_img(cv::Range(0, hsv_img.rows / 2 - 1), cv::Range(0, hsv_img.cols / 2 - 1));
        cv::Mat top_right
           = hsv_img(cv::Range(0, hsv_img.rows / 2 - 1), cv::Range(hsv_img.cols / 2, hsv_img.cols - 1));
        cv::Mat bottom_left
           = hsv_img(cv::Range(hsv_img.rows / 2, hsv_img.rows - 1), cv::Range(0, hsv_img.cols / 2 - 1));
        cv::Mat bottom_right
           = hsv_img(cv::Range(hsv_img.rows / 2, hsv_img.rows - 1), cv::Range(hsv_img.cols / 2, hsv_img.cols - 1));
        /// Using 50 bins for hue and 60 for saturation
        int h_bins = 50; int s_bins = 60;
        int histSize[] = { h_bins, s_bins };

        // hue varies from 0 to 179, saturation from 0 to 255
        float h_ranges[] = { 0, 180 };
        float s_ranges[] = { 0, 256 };

        const float* ranges[] = { h_ranges, s_ranges };

        // Use the o-th and 1-st channels
        int channels[] = { 0, 1 };

        cv::MatND hist_top_left ,
        hist_top_right ,
        hist_bottom_left ,
        hist_bottom_right ;
        cv::calcHist(&top_left, 1, channels, cv::Mat(), hist_top_left, 2, histSize, ranges, true, false);
        cv::normalize(hist_top_left, hist_top_left, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        cv::calcHist(&top_right, 1, channels, cv::Mat(), hist_top_right, 2, histSize, ranges, true, false);
        cv::normalize(hist_top_right, hist_top_right, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        cv::calcHist(&bottom_left, 1, channels, cv::Mat(), hist_bottom_left, 2, histSize, ranges, true, false);
        cv::normalize(hist_bottom_left, hist_bottom_left, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        cv::calcHist(&bottom_right, 1, channels, cv::Mat(), hist_bottom_right, 2, histSize, ranges, true, false);
        cv::normalize(hist_bottom_right, hist_bottom_right, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        st.hist[0] = hist_top_left;
        st.hist[1] = hist_top_right;
        st.hist[2] = hist_bottom_left;
        st.hist[3] = hist_bottom_right;
        images.push_back(st);
        images_p.push_back(&images.last());
        ++processed_files;
        if(processed_files % SAY_PROGRESS_EVERY == 0)
            emit currentProcessedFiles(processed_files);
    }
}
