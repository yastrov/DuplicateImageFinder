#include "src/settingshelper.h"

namespace SettingsNames {
static const QString MainWindowGroup = "MainWindow";
static const QString MainWindowSize = "size";
static const QString MainWindowPosition = "position";
static const QSize MainWindowSizeDefault = QSize(800, 600);
static const QPoint MainWindowPositionDefault = QPoint(0,0);

static const QString FileFiltersArray = "FileFilters";
static const QString FileFiltersExt = "ext";

static const QString AlgoGroup = "Algorithm";
static const QString AlgoIndex = "index";
static const int AlgoIndexDefault = 1;
static const QString EqCoeff = "EqCoeff";
static const double EqCoeffDefault = 0.8;

static const QString FontGroup = "Font";
static const QString FontSize = "size";
static const QString FontFamily = "family";
static const QString FontFamilyDefault = "Arial";
static const int     FontSizeDefault = 16;

static const QString TableGroup = "Table";
static const QString TableMaxImageHeight = "MaxImageHeight";
static const int     TableMaxImageHeightDefault = 84;
}

SettingsHelper::SettingsHelper()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<< "Settings ctor";
#endif
    QString fname;
#if defined(Q_OS_WIN)
    if(QCoreApplication::applicationDirPath().contains("Program Files")) {
        fname = QString("%1/%2.ini").arg(QDir::currentPath()).arg(QCoreApplication::applicationName());
    } else {
        fname = QString("%1/%2.ini").arg(QCoreApplication::applicationDirPath()).arg(QCoreApplication::applicationName());
    }
#elif defined(Q_OS_UNIX) || defined(Q_OS_LINUX)
    fname = QString("%1/.config/%2.ini").arg(QDir::homePath()).arg(QCoreApplication::applicationName());
#endif

#ifdef MYPREFIX_DEBUG
    qDebug()<< "Settings File name: " << fname;
#endif
    createSettings(fname);
}

SettingsHelper::~SettingsHelper()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<< "Settings dtor";
#endif
    _settings->deleteLater();
}

void SettingsHelper::createSettings(const QString &fileName)
{
#ifdef MYPREFIX_DEBUG
    qDebug()<< "SettingsHelper::createSettings";
#endif
//    if(_settings) {
//        _settings->deleteLater();
//    }
    if (!fileName.isEmpty()) {
        _settings = new QSettings(fileName, QSettings::IniFormat);
    } else {
        _settings = new QSettings(QSettings::IniFormat,
                                  QSettings::UserScope,
                                  QCoreApplication::organizationName(),
                                  QCoreApplication::applicationName());
    }
}

void SettingsHelper::saveMainWindowPosition(const QSize &size, const QPoint& pos)
{
    _settings->beginGroup(SettingsNames::MainWindowGroup);
    _settings->setValue(SettingsNames::MainWindowSize, size);
    _settings->setValue(SettingsNames::MainWindowPosition, pos);
    _settings->endGroup();
}

void SettingsHelper::loadMainWindowPosition(QSize &size, QPoint& pos)
{
    _settings->beginGroup(SettingsNames::MainWindowGroup);
    size = _settings->value(SettingsNames::MainWindowSize, SettingsNames::MainWindowSizeDefault).toSize();
    pos = _settings->value(SettingsNames::MainWindowPosition, SettingsNames::MainWindowPositionDefault).toPoint();
    _settings->endGroup();
}

void SettingsHelper::saveFileExtFilters(QStringList fileFilters)
{
    const int size = fileFilters.count();
    _settings->beginWriteArray(SettingsNames::FileFiltersArray);
    for (int i = 0; i < size; ++i) {
        _settings->setArrayIndex(i);
        _settings->setValue(SettingsNames::FileFiltersExt, fileFilters.at(i));
    }
    _settings->endArray();
}

QStringList SettingsHelper::loadFileExtFilters()
{
    QStringList result;
    const int size = _settings->beginReadArray(SettingsNames::FileFiltersArray);
    result.reserve(size);
    for (int i = 0; i < size; ++i) {
        _settings->setArrayIndex(i);
        result << _settings->value(SettingsNames::FileFiltersExt).toString();
    }
    _settings->endArray();
    if(size < 1) {
        result.reserve(17);
        result << "*.jpg" << "*.png" << "*.gif" << "*.jpeg" << "*.bmp"
                    << "*.JPG"<< "*.PNG" << "*.GIF" << "*.JPEG" << "*.BMP"
                    <<"*.jpe"<<"*.jp2"<<"*.pbm"<<"*.pgm"<<"*.ppm"<<"*.tiff"<<"*.tif" ;
    }
    return result;
}

void SettingsHelper::saveDiffAlgoIndex(int index)
{
    _settings->beginGroup(SettingsNames::AlgoGroup);
    _settings->setValue(SettingsNames::AlgoIndex, index);
    _settings->endGroup();
}

int SettingsHelper::loadDiffAlgoIndex()
{
    _settings->beginGroup(SettingsNames::AlgoGroup);
    const int index = _settings->value(SettingsNames::AlgoIndex, SettingsNames::AlgoIndexDefault).toInt();
    _settings->endGroup();
    return index;
}

void SettingsHelper::saveEqCoeff(double value)
{
    _settings->beginGroup(SettingsNames::AlgoGroup);
    _settings->setValue(SettingsNames::EqCoeff, value);
    _settings->endGroup();
}

double SettingsHelper::loadEqCoeff()
{
    _settings->beginGroup(SettingsNames::AlgoGroup);
    const double index = _settings->value(SettingsNames::EqCoeff, SettingsNames::EqCoeffDefault).toDouble();
    _settings->endGroup();
    return index;
}

void SettingsHelper::saveMaxImageHeight(int value)
{
    _settings->beginGroup(SettingsNames::TableGroup);
    _settings->setValue(SettingsNames::TableMaxImageHeight, value);
    _settings->endGroup();
}

int SettingsHelper::loadMaxImageHeight()
{
    _settings->beginGroup(SettingsNames::TableGroup);
    const int val = _settings->value(SettingsNames::TableMaxImageHeight, SettingsNames::TableMaxImageHeightDefault).toInt();
    _settings->endGroup();
    return val;
}

void SettingsHelper::saveFontSize(int size)
{
    _settings->beginGroup(SettingsNames::FontGroup);
    _settings->setValue(SettingsNames::FontSize, size);
    _settings->endGroup();
}

int SettingsHelper::loadFontSize()
{
    _settings->beginGroup(SettingsNames::FontGroup);
    const int size = _settings->value(SettingsNames::FontSize, 14).toInt();
    _settings->endGroup();
    return size;
}

void SettingsHelper::saveFontFamily(const QString& fontFamily)
{
    _settings->beginGroup(SettingsNames::FontGroup);
    _settings->setValue(SettingsNames::FontFamily, fontFamily);
    _settings->endGroup();
}

QString SettingsHelper::loadFontFamily()
{
    _settings->beginGroup(SettingsNames::FontGroup);
    const QString fontFamily = _settings->value(SettingsNames::FontFamily, "Ariel").toString();
    _settings->endGroup();
    return fontFamily;
}

void SettingsHelper::saveFont(int size, const QString& fontFamily)
{
    _settings->beginGroup(SettingsNames::FontGroup);
    _settings->setValue(SettingsNames::FontSize, size);
    _settings->setValue(SettingsNames::FontFamily, fontFamily);
    _settings->endGroup();
}

void SettingsHelper::loadFont(int& size, QString& fontFamily)
{
    _settings->beginGroup(SettingsNames::FontGroup);
    fontFamily = _settings->value(SettingsNames::FontFamily, SettingsNames::FontFamilyDefault).toString();
    size = _settings->value(SettingsNames::FontSize, SettingsNames::FontSizeDefault).toInt();
    _settings->endGroup();
}

QString SettingsHelper::loadFontStyleSheet()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<< "SettingsHelper::loadFontStyleShee";
#endif
    QString fontFamily;
    int fontSize;
    loadFont(fontSize, fontFamily);
    return QString("font-size:%1pt;font-family:%2;").arg(fontSize).arg(fontFamily);
}
