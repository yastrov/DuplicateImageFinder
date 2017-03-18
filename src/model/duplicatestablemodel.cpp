#include "duplicatestablemodel.h"

namespace {
const int MaxColumns = 9;
QColor firstColor = QColor(Qt::white);
QColor secondColor = QColor(Qt::green);

QString getFileNameShort(const QString &path)
{
#ifdef NATIVE_PATH_SEP
    QString fname = QDir::toNativeSeparators(path);
    return  fname.section(QDir::separator(), -1);
#else
    return  path.section('/', -1);
#endif
}
}

DuplicatesTableModel::DuplicatesTableModel(QSharedPtrListHFIS content, QObject *parent):
    DuplicatesTableModel(content, 64, parent)
{}

DuplicatesTableModel::DuplicatesTableModel(QSharedPtrListHFIS content, int maxImageHeight, QObject *parent)
        : BaseTableModel(content, parent), maxImageHeight(maxImageHeight) {
    QList<HashFileInfoStruct> * const list = content.data();
    for(const HashFileInfoStruct &s: qAsConst(*list)){
        MyCacheModelData data;
        data.short_fname = getFileNameShort(s.fileName);

        QImage image(s.fileName);
        data.pixmap = QPixmap::fromImage(image).scaledToHeight(maxImageHeight);
        cache.insert(s.fileName, data);
    }
}

int DuplicatesTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : items->count();
}


int DuplicatesTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MaxColumns;
}

QVariant DuplicatesTableModel::headerData(int section,
                                          Qt::Orientation orientation, int role) const
{
    if(role == Qt::ToolTipRole && section==Column::difference)
        return tr("Defference between this and base group image.");
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case Column::checked: return tr("Remove?"); break;
        case Column::fileName: return tr("FileName"); break;
        case Column::hash: return tr("Hash");  break;
        case Column::groupId: return tr("groupId"); break;
        case Column::size: return tr("Size"); break;
        case Column::width: return tr("Width"); break;
        case Column::height: return tr("Height"); break;
        case Column::icon: return tr("icon"); break;
        case Column::difference:  return tr("Difference"); break;
        default: Q_ASSERT(false);
        }
    }
    return section + 1;
}

QVariant DuplicatesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() >= MaxColumns)
        return QVariant();
    const HashFileInfoStruct &item = items->at(index.row());
    switch(role) {
    case Qt::SizeHintRole: {
        QStyleOptionComboBox option;
        switch (index.column()) {
        case Column::checked: option.currentText = QString(""); break;
        case Column::fileName: option.currentText = cache.value(item.fileName).short_fname; break;
        case Column::hash: option.currentText = item.hash.toHex(); break;
        case Column::groupId: option.currentText = item.groupID; break;
        case Column::size: option.currentText = QString::number(item.size); break;
        case Column::width: option.currentText = QString::number(item.width); break;
        case Column::height: option.currentText = QString::number(item.height); break;
        case Column::icon: return cache.value(item.fileName).pixmap.size(); break;
        case Column::difference: option.currentText = QString::number(item.diff); break;
        default: Q_ASSERT(false);
        }
        QFontMetrics fontMetrics(data(index, Qt::FontRole)
                                 .value<QFont>());
        option.fontMetrics = fontMetrics;
        QSize size(fontMetrics.width(option.currentText),
                   fontMetrics.height());
        return qApp->style()->sizeFromContents(QStyle::CT_ComboBox,
                                               &option, size);
    }
    case Qt::DisplayRole: {
        switch (index.column()) {
        case Column::checked: return QVariant();
        case Column::fileName: return cache.value(item.fileName).short_fname;
        case Column::hash: return item.hash.toHex();
        case Column::groupId: return item.groupID;
        case Column::size: return item.size;
        case Column::width: return item.width;
        case Column::height: return item.height;
        case Column::icon: return "";
        case Column::difference: return item.diff;
        default: Q_ASSERT(false);
        }
    }
    case Qt::EditRole: {
        return QVariant();
    }
    case Qt::CheckStateRole: {
        if(index.column() == Column::checked) {
            return item.checked==true ? Qt::Checked : Qt::Unchecked;
        }
        return QVariant();
    }
    case Qt::ToolTipRole: {
        switch (index.column()) {
        case Column::checked: return tr("Remove?");
        case Column::fileName: return item.fileName;
        case Column::hash: return item.hash.toHex();
        case Column::groupId: return item.groupID;
        case Column::size: return item.size;
        case Column::width: return item.width;
        case Column::height: return item.height;
        case Column::difference: return item.diff;
        default: Q_ASSERT(false);
        }
    }
    case Qt::BackgroundColorRole: {
        QColor color = item.groupID % 2 == 0? firstColor : secondColor;
        return QVariant(color);
    }
    case Qt::DecorationRole: {
        if(index.column() == Column::icon) {
            return cache.value(item.fileName).pixmap;
        }
        return QVariant();
    }
    default: return QVariant();
    }
    return QVariant();
}

bool DuplicatesTableModel::setData(const QModelIndex &index,
                                   const QVariant &value, int role)
{
    const int column = index.column();
    const int row = index.row();
    if (!index.isValid() ||
            row < 0 || row >= items->count() ||
            column < 0 || column > MaxColumns)
        return false;
    HashFileInfoStruct &item = (*items.data())[row];
    if (role == Qt::CheckStateRole)
    {
        if (value.toInt() == Qt::Checked)
        {
            item.checked = true;
        }
        else {
            item.checked = false;
        }
        emit dataChanged(index, index);
    }
    return false;
}

void DuplicatesTableModel::sort(int column, Qt::SortOrder order)
{
#ifdef MYPREFIX_DEBUG
    if(order == Qt::AscendingOrder) {
        qDebug()<< "DuplicatesTableModel::sort Qt::AscendingOrder";
    } else {
        qDebug()<< "DuplicatesTableModel::sort Qt::DescendingOrder";
    }
#endif
    switch(column) {
    case Column::checked:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            if(v1.checked && !v2.checked)
                return true;
            return false;
        }); break;
    case Column::fileName:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.fileName > v2.fileName;
        });
        break;
    case Column::hash:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.hash > v2.hash;
        });
        break;
    case Column::groupId:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.groupID > v2.groupID;
        });
        break;
    case Column::size:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.size > v2.size;
        });
        break;
    case Column::height:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.height > v2.height;
        });
        break;
    case Column::width:
        std::sort(items->begin(), items->end(), [](const HashFileInfoStruct &v1, const HashFileInfoStruct &v2)->bool {
            return v1.width > v2.width;
        });
        break;
    default: return;
    }
    if(order != Qt::AscendingOrder)
        std::reverse(items->begin(), items->end());
    emit dataChanged(QModelIndex(), QModelIndex());
}

QString DuplicatesTableModel::getFileName(const QModelIndex &index) const
{
    if (!index.isValid() ||
            index.row() < 0 || index.row() >= items->count() ||
            index.column() < 0 || index.column() >= MaxColumns)
        return "";
    const HashFileInfoStruct &item = items->at(index.row());
    return item.fileName;
}

bool DuplicatesTableModel::checkOneInGroupUnChecked()
{
    quint64 currentGroup = 0;
    quint64 numOfAll = 0;
    quint64 numOfChecked = 0;
    QList<HashFileInfoStruct> * const list = items.data();
    for(const HashFileInfoStruct &s: qAsConst(*list)){
        if(s.groupID != currentGroup) {
            currentGroup = s.groupID;
            if(numOfAll == numOfChecked) {
                return false;
            }
            numOfAll = 0;
            numOfChecked = 0;
        } else {
            if(s.checked) ++numOfChecked;
        }
        ++numOfAll;
    }
    if(numOfAll == numOfChecked) {
        return false;
    }
    return true;
}

void DuplicatesTableModel::removeCheckedFunc()
{
    if(!checkOneInGroupUnChecked()) {
        QMessageBox::StandardButton reply = QMessageBox::question(nullptr,
                                                                  qApp->applicationName(),
                                                                  tr("In one group all files have been checked!.\nDo you want to continue?"),
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }
    BaseTableModel::removeCheckedFunc();
}

#if defined(Q_OS_WIN)
void DuplicatesTableModel::removeCheckedToTrashFunc()
{
    if(!checkOneInGroupUnChecked()) {
        QMessageBox::StandardButton reply = QMessageBox::question(nullptr,
                                                                  qApp->applicationName(),
                                                                  tr("In one group all files have been checked!.\nDo you want to continue?"),
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }
    BaseTableModel::removeCheckedToTrashFunc();
}
#endif

