#include "src/gui/mainwindow.h"
#include <QApplication>
#include "hashfileinfostruct.h"
#include <QSharedPointer>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QSharedPointer<QList<HashFileInfoStruct>>>("QSharedPointer<QList<HashFileInfoStruct>>");
    qRegisterMetaType<QSharedPtrListHFIS>("QSharedPtrListHFIS");
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);
    a.setApplicationName("DuplicateImageFinder");

    MainWindow w;
    w.show();

    return a.exec();
}
