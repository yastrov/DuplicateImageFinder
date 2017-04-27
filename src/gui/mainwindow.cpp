#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <limits>

namespace CONSTANTS {
const quint64 MAX_INT = static_cast<qint64>(std::numeric_limits<int>::max());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    maxImageHeight(86),
    dirNameForFolderDialog(QDir::current().dirName()),
    thread(new QThread(this))
{
    ui->setupUi(this);
    setWindowTitle(qApp->applicationName());
    initDirsListWidget();
    initTableWidget();
    QObject::connect(ui->actionAbout, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        QMessageBox::about(this,
                           tr("About %1").arg(qApp->applicationName()),
                           tr("<h2>DuplicateImageFinder</h2>"
                              "<p>Written by Yuriy (Yuri) Astrov<br/>"
                              "Based on QT 5<br/>"
                              "Licensed by GPLv2<br/>"
                              "Version: %1<br/>"
                              "URL: <a href='%2'>%2</a><p>").arg(APP_VERSION, APP_URL));
    });

    // Drag and Drop
    setAcceptDrops(true);
    // Progress Bar
    ui->progressBar->setVisible(false);
    ui->useFiltersBtn->setChecked(true);
    // Win Extras
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    buttonWinExtra = new QWinTaskbarButton(this);
    buttonWinExtra->setWindow(windowHandle());
    //    buttonWinExtra->setOverlayIcon(QIcon(":/loading.png"));
    progressWinExtra = buttonWinExtra->progress();
    progressWinExtra->setVisible(false);
#endif
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::on_pushButton_Save_From_Table_clicked);
    connect(ui->actionOpen, &QAction::triggered, this, [this]{
        QTableView * const table = ui->tableView;
        QString filter = "Text files (*.txt)";
        const QString fileName = QFileDialog::getOpenFileName(this,
                                                              tr("Open File"),
                                                              QDir::homePath(),
                                                              "Text files (*.txt)",
                                                              &filter, QFileDialog::DontUseNativeDialog);
        if(!fileName.isNull() && !fileName.isEmpty())
        {
            if(table->model() != nullptr) {
                table->model()->deleteLater();
            }
            DuplicatesTableModel *model = new DuplicatesTableModel(this);
            model->loadFromFileFunc(fileName);
            table->setModel(model);
            table->setSortingEnabled(true);
            table->sortByColumn(DuplicatesTableModel::Column::groupId);
            table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
            table->resizeColumnsToContents();
        }
    });
    connect(ui->actionRemove_checked, &QAction::triggered, this, &MainWindow::on_pushButton_Remove_Checked_clicked);
#if defined(Q_OS_WIN)
    connect(ui->actionRemoved_checked_to_Trash, &QAction::triggered, this, [this](bool checked){
            Q_UNUSED(checked)
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      qApp->applicationName(),
                                                                      tr("Are you sure to remove checked items?"),
                                                                      QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No) {
                return;
            }
            if(ui->tableView->model() != nullptr) {
                qobject_cast<BaseTableModel*>(ui->tableView->model())->removeCheckedToTrashFunc();
            }
        });
#else
    ui->actionRemoved_checked_to_Trash->setVisible(false);
#endif
    connect(ui->actionSelect_files_in_concrete_folder, &QAction::triggered, this, [this](){
        const QString dirName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                                  dirNameForFolderDialog,
                                                                  QFileDialog::ShowDirsOnly
                                                                  | QFileDialog::DontResolveSymlinks);
        if(!dirName.isEmpty() && !dirName.isNull())
        {
            QTableView * const table = ui->tableView;
            if(table->model()==nullptr) {
                return;
            }
            qobject_cast<BaseTableModel*>(table->model())->selectFilesInFolder(dirName);
        }
    });
    connect(ui->actionClear_all_flags, &QAction::triggered, this, [this](){
        if(ui->tableView->model()==nullptr) {
            return;
        }
        qobject_cast<BaseTableModel*>(ui->tableView->model())->unselectAll();
    });
    connect(ui->actionAbout_Qt, &QAction::triggered, this, [this](){ QMessageBox::aboutQt(this); });
    // ComboBox
    ui->AlgoComboBox->addItem(tr("dHash"), DiffSearchMethod::dHash);
    ui->AlgoComboBox->setItemData(0,
                                  tr("Very quick, but very simple algorithm.\nMany false results."),
                                  Qt::ToolTipRole);
    ui->AlgoComboBox->addItem(tr("Histogram compare"), DiffSearchMethod::Histogram);
    ui->AlgoComboBox->setItemData(1,
                                  tr("Quick, based on histogram compare algorithm.\nMay have false results."),
                                  Qt::ToolTipRole);
    ui->AlgoComboBox->addItem(tr("Histogram Four compare"), DiffSearchMethod::HistogramQ);
    ui->AlgoComboBox->setItemData(2,
                                  tr("Quick, based on histogram compare\nalgorithm (calculate 4 histogram to 1 image).\nBetter than simple histogram compare,\nneed more calculations and memory."),
                                  Qt::ToolTipRole);
    ui->AlgoComboBox->setCurrentIndex(1);
    loadSettings();
}

void MainWindow::showEvent(QShowEvent *e)
{
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    buttonWinExtra->setWindow(windowHandle());
#endif

    e->accept();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Dirs List Widget Start
void MainWindow::initDirsListWidget()
{
    ui->listWidget->clear();
}

QList<QDir> MainWindow::getElementsFromDirsListWidget()
{
    const int count = ui->listWidget->count();
    QListWidgetItem *item;
    QList<QDir> dirList;
    dirList.reserve(count);
    QDir dir;

    QListWidget *list = ui->listWidget;
    for(int i=0; i < count; ++i)
    {
        item = list->item(i);
        dir = QDir(item->data(Qt::UserRole).toString());
        /* Now it works in Backgrownd workers
        dir.setNameFilters(qsl);
        */
        dirList.append(dir);
    }
    return dirList;
}

void MainWindow::addToDirListWidget(const QString &dirName)
{
    QListWidgetItem *item = new QListWidgetItem(QDir(dirName).absolutePath());
    QVariant variant(dirName);
    item->setData(Qt::UserRole, variant);
    item->setToolTip(dirName);
    ui->listWidget->addItem(item);
}

// Table Widget
void MainWindow::initTableWidget()
{
    QTableView *table = ui->tableView;
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(table, &QTableView::customContextMenuRequested, this, &MainWindow::createCustomPopupMenuForTableView);
}

void MainWindow::showDuplicatesInTable(QSharedPtrListHFIS itemsPtr)
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "showDuplicatesInTable";
#endif
    QTableView *table = ui->tableView;
    if(table->model() != nullptr)
        table->model()->deleteLater();
    DuplicatesTableModel *model = new DuplicatesTableModel(itemsPtr, maxImageHeight, this);
    model->setMaxImageHeight(maxImageHeight);
    table->setModel(model);
    table->setSortingEnabled(true);
    table->sortByColumn(DuplicatesTableModel::Column::groupId);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}


void MainWindow::callBeforeBackgrowndWorkerStarted()
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(ui->progressBar->minimum());
    ui->progressBar->setStatusTip("");
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    if(progressWinExtra != nullptr) {
        progressWinExtra->setMinimum(0);
        progressWinExtra->setVisible(true);
    }
#endif

}

// Duplicate Files Search
void MainWindow::startDuplicateSearchInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startDuplicateSearchInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DuplicateFinder *worker = new DuplicateFinder(nullptr);
        worker->setQDir(dirs);
        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DuplicateFinder::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        /*No examples where QThread::finished connected to &QThread::quit !
         * And then we need Worker::finished. */
        QObject::connect(worker, &DuplicateFinder::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DuplicateFinder::deleteLater);//From Off documentation
        QObject::connect(worker, &DuplicateFinder::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(showDuplicatesInTable(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DuplicateFinder::finishedWData, this, &MainWindow::showDuplicatesInTable);

        connect(worker, &DuplicateFinder::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &DuplicateFinder::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
    }
}

// Duplicate Files Search
void MainWindow::startDuplicateHistogramSearchInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startDuplicateHistogramSearchInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DuplacateHistogramFinder *worker = new DuplacateHistogramFinder(nullptr);
        worker->setQDir(dirs);

        Local_Hist_Settings1 settings;
        settings.epsilon = ui->coeffEqualSpinBox->value();
        const QVariant method = ui->methodComboBox->currentData();
        settings.hist_compare_method = method.value<HistogramMethod>();
        worker->setSettings(settings);

        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DuplacateHistogramFinder::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        /*No examples where QThread::finished connected to &QThread::quit !
         * And then we need Worker::finished. */
        QObject::connect(worker, &DuplacateHistogramFinder::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DuplacateHistogramFinder::deleteLater);//From Off documentation
        QObject::connect(worker, &DuplacateHistogramFinder::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(showDuplicatesInTable(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DuplacateHistogramFinder::finishedWData, this, &MainWindow::showDuplicatesInTable);

        connect(worker, &DuplacateHistogramFinder::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &DuplacateHistogramFinder::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
    }
}
void MainWindow::startDuplicateHistogramQSearchInBackground()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "startDuplicateHistogramQSearchInBackground";
#endif
    if (thread->isRunning()) {
        thread->wait();
    }
    QList<QDir> dirs = getElementsFromDirsListWidget();
    if(!dirs.isEmpty() && !thread->isRunning())
    {
        DuplacateHistogramQFinder *worker = new DuplacateHistogramQFinder(nullptr);
        worker->setQDir(dirs);

        Local_Hist_Settings1 settings;
        settings.epsilon = ui->coeffEqualSpinBox->value();
        const QVariant method = ui->methodComboBox->currentData();
        settings.hist_compare_method = method.value<HistogramMethod>();
        worker->setSettings(settings);

        if(useFilters()) worker->setFilters(fileFilters);
        callBeforeBackgrowndWorkerStarted();
        worker->moveToThread(thread);
        //connect(thread, SIGNAL(started()), worker, SLOT(process()));
        QObject::connect(thread, &QThread::started, worker, &DuplacateHistogramQFinder::process);
        //connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
        /*No examples where QThread::finished connected to &QThread::quit !
         * And then we need Worker::finished. */
        QObject::connect(worker, &DuplacateHistogramQFinder::finished, thread, &QThread::quit);
        QObject::connect(thread, &QThread::finished, worker, &DuplacateHistogramQFinder::deleteLater);//From Off documentation
        QObject::connect(worker, &DuplacateHistogramQFinder::finished, this, &MainWindow::finishedThread);
        //connect(worker, SIGNAL(finishedWData(QList<HashFileInfoStruct> *)), this, SLOT(showDuplicatesInTable(QList<HashFileInfoStruct> *)));
        QObject::connect(worker, &DuplacateHistogramQFinder::finishedWData, this, &MainWindow::showDuplicatesInTable);

        connect(worker, &DuplacateHistogramQFinder::sayTotalFiles, this, &MainWindow::maximumFilesFoProgressReceived);
        connect(worker, &DuplacateHistogramQFinder::currentProcessedFiles, this, &MainWindow::currentProcessedFilesForProgressReceived);
#ifdef MYPREFIX_DEBUG
        qDebug() << "startThread";
#endif
        thread->start();
        setUiPushButtonsEnabled(false);
    }
}

/*
 * Open Folder Choose dialog and add Dirname to ListWidget
 */
void MainWindow::on_pushButton_Add_Dir_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_Add_Dir_clicked";
#endif
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        dirNameForFolderDialog,
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    if(!dirName.isEmpty() && !dirName.isNull())
    {
        dirNameForFolderDialog = QFileInfo(dirName).absoluteDir().absolutePath();
        addToDirListWidget(dirName);
    }
}

/*
 * Remove Seleccted item from TableWidget
 */

void MainWindow::on_pushButton_Remove_Checked_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << ":on_pushButton_Remove_Checked_clicked";
#endif
    if(ui->tableView->model()!=nullptr)
        qobject_cast<BaseTableModel*>(ui->tableView->model())->removeCheckedFunc();
}


/*
 * Slot for Start Duplicate file Search in background (other function)
 */
void MainWindow::on_pushButton_Duplicate_Search_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "QThread: " << thread << "\n";
#endif
    const QVariant variant = ui->AlgoComboBox->currentData();
    const DiffSearchMethod method = variant.value<DiffSearchMethod>();
    switch(method) {
        case DiffSearchMethod::dHash:
            startDuplicateSearchInBackground(); break;
        case DiffSearchMethod::Histogram:
            startDuplicateHistogramSearchInBackground(); break;
        case DiffSearchMethod::HistogramQ:
            startDuplicateHistogramQSearchInBackground(); break;
    }
}

/*
 * Remove Seleccted item (Dir Name) from ListWidget
 */
void MainWindow::on_pushButton_Remove_Dir_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_RemoveDir_clicked";
#endif
    // Din't work
    //ui->listWidget->removeItemWidget(ui->listWidget->currentItem());
    delete ui->listWidget->currentItem();
}

void MainWindow::on_pushButton_Cancel_clicked()
{
#ifdef MYPREFIX_DEBUG
    qDebug() << "on_pushButton_Cancel_clicked";
#endif
    thread->requestInterruption();
}

void MainWindow::finishedThread()
{
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    if(progressWinExtra != nullptr)
        progressWinExtra->setVisible(false);
#endif
    ui->progressBar->setVisible(false);
    setUiPushButtonsEnabled(true);
    QMessageBox::information(this,
                             "DuplicateImageFinder",
                             tr("Task completed successfully!"));
}

void MainWindow::on_pushButton_Save_From_Table_clicked()
{
    QString filter = "Text files (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save File"),
                                                    QDir::homePath()+"/results.txt",
                                                    "Text files (*.txt)",
                                                    &filter, QFileDialog::DontUseNativeDialog);
#ifdef MYPREFIX_DEBUG
    qDebug() << __PRETTY_FUNCTION__ << "\n";
    qDebug() << "File: " << fileName << "\n";
#endif
    if(!fileName.isNull() && !fileName.isEmpty())
    {
        QFileInfo qF(fileName);
        if(qF.exists())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                      qApp->applicationName(),
                                                                      tr("File already exists!\nOverwrite?"),
                                                                      QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No)
            {
                return;
            }
        }
        //emit saveModelDataToFile(fileName); But error with it.
        if(ui->tableView->model()!=nullptr)
            qobject_cast<BaseTableModel*>(ui->tableView->model())->saveToFileFunc(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
#ifdef MYPREFIX_DEBUG
    qDebug() << "MainWindow::closeEvent";
#endif
    if (thread->isRunning()) {
        thread->requestInterruption();
        /*QMessageBox::warning(this,
                             "DuplicateImageFinder",
                             tr("Background process already running!\nTry to exit after task would be complete."));
        event->ignore();*/
        thread->quit();
        thread->wait(250);
        event->accept();
    } else {
        storeSettings();
        event->accept();
    }
}

void MainWindow::setUiPushButtonsEnabled(bool flag)
{
    ui->pushButton_Duplicate_Search->setEnabled(flag);
    ui->pushButton_Remove_Checked->setEnabled(flag);
    ui->pushButton_Save_From_Table->setEnabled(flag);
    ui->menuFile->setEnabled(flag);
    ui->menuWork_with_results->setEnabled(flag);
}

// Drag Drop START
void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
    QMainWindow::dragEnterEvent(e);
}

void MainWindow::dropEvent(QDropEvent *e)
{
#ifdef MYPREFIX_DEBUG
    for(const QString &format: e->mimeData()->formats()){
        qDebug()<<format<<"\n";
    }
#endif
    // Check destination widget
    const QRect widgetRect = ui->listWidget->geometry();
    if(!widgetRect.contains(e->pos()))
        return;
    // Add file/folder
    for(const QUrl &url: e->mimeData()->urls()) {
        addToDirListWidget(url.toLocalFile());
    }
}
// Drag Drop END

// Custom popup menu for Table View
void MainWindow::createCustomPopupMenuForTableView(const QPoint &pos)
{
    const QTableView *table = ui->tableView;
    QMenu *menu = new QMenu(this);
    QAction *openFolder = new QAction(tr("Open Folder with selected file"), this);
    connect(openFolder, &QAction::triggered, this, [this](){
        const QTableView *table = ui->tableView;
        if(table->model() == nullptr) return;
        const QModelIndexList indexList = table->selectionModel()->selectedIndexes();
        QString dir;
        const BaseTableModel *model = qobject_cast<BaseTableModel *>(table->model());
        for(const QModelIndex &index: indexList) {
            dir = model->getFileName(index);
        }
        QStringList commands;
#if defined(Q_OS_WIN)
        commands<< "explorer.exe /select, \"" << QDir::toNativeSeparators(dir) <<"\"";
        qDebug()<<commands.join("");
        QProcess::startDetached(commands.join(""));
#elif defined(Q_OS_MAC)
        QStringList scriptArgs;
        scriptArgs << QLatin1String("-e")
                   << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                      .arg(dir);
        QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
        scriptArgs.clear();
        scriptArgs << QLatin1String("-e")
                   << QLatin1String("tell application \"Finder\" to activate");
        QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
    });
    menu->addAction(openFolder);
    menu->popup(table->viewport()->mapToGlobal(pos));
}

bool MainWindow::useFilters() const
{
    return ui->useFiltersBtn->isChecked();
}

void MainWindow::on_setFiltersBtn_clicked()
{
    FiltersDialog f;
    f.setActiveFilters(fileFilters);
    f.setStyleSheet(this->styleSheet());
    if(f.exec() == QDialog::Accepted){
        fileFilters = f.getActiveFilters();
    }
}

void MainWindow::maximumFilesFoProgressReceived(q_coll_s_t count)
{
    //if(count > CONSTANTS::MAX_INT) return;
    ui->progressBar->setMaximum(count);
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    progressWinExtra->setMaximum(count);
#endif
}

void MainWindow::currentProcessedFilesForProgressReceived(q_coll_s_t count)
{
    //if(count > CONSTANTS::MAX_INT) return;
    ui->progressBar->setValue(count);
#if defined(USE_WIN_EXTRAS) && defined(Q_OS_WIN)
    progressWinExtra->setValue(count);
#endif
    const QString s = QString::number(count) + " / " + QString::number(ui->progressBar->maximum());
    ui->progressBar->setStatusTip(s);
    ui->statusBar->showMessage(s);
}

void MainWindow::on_methodComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
}

void MainWindow::on_AlgoComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    ui->methodComboBox->clear();
    const QVariant variant = ui->AlgoComboBox->currentData();
    const DiffSearchMethod method = variant.value<DiffSearchMethod>();
    if(method == DiffSearchMethod::Histogram || method == DiffSearchMethod::HistogramQ) {
        ui->methodComboBox->addItem(tr("Correlation"), HistogramMethod::CV_COMP_CORREL);
        ui->methodComboBox->addItem(tr("Chi-Square"), HistogramMethod::CV_COMP_CHISQR);
        ui->methodComboBox->addItem(tr("Intersection"), HistogramMethod::CV_COMP_INTERSECT);
        ui->methodComboBox->addItem(tr("Bhattacharyya distance"), HistogramMethod::CV_COMP_BHATTACHARYYA);
    }
}

void MainWindow::storeSettings()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<<"MainWindow::storeSettings";
#endif
    _settingsHelper.saveFileExtFilters(fileFilters);
    _settingsHelper.saveDiffAlgoIndex(ui->AlgoComboBox->currentIndex());
    _settingsHelper.saveMainWindowPosition(size(), pos());
    _settingsHelper.saveEqCoeff(ui->coeffEqualSpinBox->value());
    _settingsHelper.saveMaxImageHeight(maxImageHeight);
}

void MainWindow::loadSettings()
{
#ifdef MYPREFIX_DEBUG
    qDebug()<<"MainWindow::loadSettings";
#endif
    fileFilters = _settingsHelper.loadFileExtFilters();
    this->setStyleSheet(_settingsHelper.loadFontStyleSheet());
    ui->AlgoComboBox->setCurrentIndex(_settingsHelper.loadDiffAlgoIndex());
    ui->coeffEqualSpinBox->setValue(_settingsHelper.loadEqCoeff());
    maxImageHeight = _settingsHelper.loadMaxImageHeight();
    // Load MainWindow position and size
    QSize size;
    QPoint pos;
    _settingsHelper.loadMainWindowPosition(size, pos);
    resize(size);
    move(pos);
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog dialog;
    dialog.setStyleSheet(this->styleSheet());

    QString fontFamily;
    int fontSize;
    _settingsHelper.loadFont(fontSize, fontFamily);
    QFont font(fontFamily, fontSize);

    dialog.setMaxImageHeight(maxImageHeight);
    dialog.setFontC(font);
    if(dialog.exec() == QDialog::Accepted){
        maxImageHeight = dialog.getMaxImageHeight();
        font = dialog.getFontC();
        _settingsHelper.saveFont(font.pointSize(), font.family());
        this->setStyleSheet(_settingsHelper.loadFontStyleSheet());
    }
}
