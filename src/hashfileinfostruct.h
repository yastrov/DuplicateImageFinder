#ifndef HASHFILEINFOSTRUCT
#define HASHFILEINFOSTRUCT
#include <QString>
#include <QSharedPointer>

typedef struct {
    QString fileName;
    QByteArray hash;
    bool checked;
    uint groupID;
    qint64 size;
    uint height, width;
} HashFileInfoStruct;

typedef QSharedPointer<QList<HashFileInfoStruct>> QSharedPtrListHFIS;

#endif // HASHFILEINFOSTRUCT
