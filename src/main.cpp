#include "src/gui/mainwindow.h"
#include <QApplication>
#include "hashfileinfostruct.h"
#include "constants.h"
#include <QSharedPointer>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<QList<HashFileInfoStruct>>>("QSharedPointer<QList<HashFileInfoStruct>>");
    qRegisterMetaType<QSharedPtrListHFIS>("QSharedPtrListHFIS");
    qRegisterMetaType<q_coll_s_t>("q_coll_s_t");
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName("DuplicateImageFinder");

    MainWindow w;
    w.show();

    return a.exec();
}
