#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QShowEvent>
#include <QMimeData>
#include "../model/duplicatestablemodel.h"
#include "../gui/dialog/filtersdialog.h"
#include "../worker/duplicatefinder.h"
#include "../hashfileinfostruct.h"
#include <QProcess>
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString dirNameForFolderDialog;
    QThread* thread;
    QStringList fileFilters;
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    QWinTaskbarButton *buttonWinExtra;
    QWinTaskbarProgress *progressWinExtra;
#endif
    // ComboBox
    void addItemToComboBox(const QString &text, int data);
    void initHashComboBoxWidget();
    void initDirsListWidget();
    QList<QDir> getElementsFromDirsListWidget();
    void addToDirListWidget(const QString &dirName);
    // TableWidget
    void initTableWidget();
    // Duplicate files search
    void startDuplicateSearchInBackground();
    void saveItemsToFile(const QString &fileName);
    void setUiPushButtonsEnabled(bool flag);
    // Drag Drop
    void dragEnterEvent(QDragEnterEvent *e) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *e) Q_DECL_OVERRIDE;

    void callBeforeBackgrowndWorkerStarted();
    // Filters
    bool useFilters() const;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *e);

private slots:
    void on_pushButton_Save_From_Table_clicked();
    void on_pushButton_Add_Dir_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Remove_Checked_clicked();
    void on_pushButton_Duplicate_Search_clicked();
    void on_pushButton_Remove_Dir_clicked();
    void showDuplicatesInTable(QSharedPtrListHFIS itemsPtr);
    void createCustomPopupMenuForTableView(const QPoint &pos);
    void on_setFiltersBtn_clicked();
    void maximumFilesFoProgressReceived(quint64 count);
    // Progress Bars
    void currentProcessedFilesForProgressReceived(quint64 count);
    void finishedThread();
};

#endif // MAINWINDOW_H
