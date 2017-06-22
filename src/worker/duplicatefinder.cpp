#include "duplicatefinder.h"
#ifdef MYPREFIX_DEBUG
#include <QDebug>
#endif

int powi (int base, unsigned int exp)
{
    int res = 1;
    while (exp) {
        if (exp & 1)
            res *= base;
        exp >>= 1;
        base *= base;
    }
    return res;
}

const int powTable[8] = {1, 2, 4, 8, 16, 32, 64, 128};

DuplicateFinder::DuplicateFinder(QObject *parent) :
    DirWalker(parent),
    resultCount(0)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::constructor";
#endif
}

DuplicateFinder::~DuplicateFinder()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::~DuplicateFinder";
#endif
}

void DuplicateFinder::process()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::process";
#endif
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    calcTotalFiles();
    emit sayTotalFiles(total_files);
    processFilesRecursively();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    clearNoDuplicatedSize();
    if(QThread::currentThread()->isInterruptionRequested())
    {
        emit finished();
        return;
    }
    reduceToResult();
    emit currentProcessedFiles(total_files);
    emit finishedWData(result);
    emit finished();
}

void DuplicateFinder::processFile(const QString &fileName)
{
    if(fileName.isEmpty() || fileName.isNull())
        return;
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::processFile "<<fileName;
#endif
    QFileInfo fInfo(fileName);
    if(fInfo.isFile() && fInfo.isReadable())
    {
        HashFileInfoStruct st;
        st.fileName = fileName;
        st.size = fInfo.size();
        st.checked = false;
        dHash(st);
        hashItems.insert(st.hash, st);
        ++processed_files;
        if(processed_files % SAY_PROGRESS_EVERY == 0)
            emit currentProcessedFiles(processed_files);
    }
}

void DuplicateFinder::clearNoDuplicatedSize()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::clearNoDuplicatedSize";
#endif
    for(const QByteArray &key: hashItems.uniqueKeys())
    {
        if(hashItems.count(key) < 2) {
            ++processed_files;
            hashItems.remove(key);
        }
    }
}

void DuplicateFinder::reduceToResult()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult";
#endif
    result = QSharedPtrListHFIS(new QList<HashFileInfoStruct>());
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: reserve memory for result";
#endif
    result.data()->reserve(processed_files);
    HashFileInfoStruct s;
    quint64 groupId = 0;
    bool checked = false;

    QList<HashFileInfoStruct> * const list = result.data();
    for(const QByteArray &key: hashItems.uniqueKeys())
    {
        checked = false;
        QList<HashFileInfoStruct> values = hashItems.values(key);
        QMutableListIterator<HashFileInfoStruct> vIt(values);
        while(vIt.hasNext())
        {
            s = vIt.next();
            s.groupID = groupId;
            s.checked = checked;
            list->append(std::move(s));
            checked = true;
        }
        ++groupId;
    }
    hashItems.clear();
#ifdef MYPREFIX_DEBUG
    qDebug() << "DuplicateFinder::reduceToResult:: return result";
#endif
}

void DuplicateFinder::dHash(HashFileInfoStruct& strct, size_t hashSize)
{
    cv::Mat gs = cv::imread(strct.fileName.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);

    strct.width = gs.cols;
    strct.height = gs.rows;

    cv::Mat dest;
    const cv::Size size(hashSize+1, hashSize);
    cv::resize(gs, dest, size, 0, 0, cv::INTER_AREA);

    uchar pixel_left, pixel_right;
    QByteArray difference;
    difference.reserve(hashSize*hashSize);
    for(int row=0; row<dest.rows; ++row)
    {
        for (int col=0; col < dest.cols; ++col)
        {
            pixel_left = dest.at<uchar>(row, col);
            pixel_right = dest.at<uchar>(row, col+1);
            difference.append( (pixel_left > pixel_right)? 1:0);
        }
    }

    int index=0;
    int decimal_value = 0;
    QByteArray result;
    result.reserve(hashSize);
    int hashByEight = 0;
    for(const bool& b: difference) {
        hashByEight = index % 8;
        if(b) {
            decimal_value += powTable[hashByEight];
        }
        if(hashByEight == 7) {
            QByteArray ar;
            ar.setNum(decimal_value, 16);
            result.append(ar);
        }
        ++index;
    }
    strct.hash = result;
}
