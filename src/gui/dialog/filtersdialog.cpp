#include "filtersdialog.h"

FiltersDialog::FiltersDialog(QWidget *parent) :
    QDialog(parent)
{
    setModal(true);
    initGUI();
}

FiltersDialog::~FiltersDialog()
{
//    delete ui;
    myLayout->deleteLater();
}

void FiltersDialog::initGUI()
{
    QStringList leftItems;
    leftItems
          << "*.jpg" << "*.png" << "*.gif" << "*.jpeg" << "*.bmp"
          << "*.JPG"<< "*.PNG" << "*.GIF" << "*.JPEG" << "*.BMP"
           <<"*.jpe"<<"*.jp2"<<"*.pbm"<<"*.pgm"<<"*.ppm"<<"*.tiff"<<"*.tif" ;

    myLayout = new QGridLayout;
     ListCount = new QLabel(tr("Filters:"));
     ListLabel = new QLabel(tr("Current filters:"));
     leftList = new QListView;
     leftModel = new QStringListModel(this);
     leftModel->setStringList(leftItems);
     leftList->setModel(leftModel);
     leftList->setToolTip(tr("Filters that you can choice!"));
     leftList->setMovement(QListView::Free);
     leftList->setEditTriggers(QAbstractItemView::AnyKeyPressed |
                                 QAbstractItemView::DoubleClicked);
     leftList->setDragDropMode(QAbstractItemView::DragDrop);
     leftList->setDragEnabled(true);

     rightList = new QListView;
     rightModel = new QStringListModel(this);
     rightModel->setStringList(leftItems);
     rightList->setModel(rightModel);
     rightList->setToolTip(tr("You active filters here!"));
     rightList->setMovement(QListView::Free);
     rightList->setEditTriggers(QAbstractItemView::AnyKeyPressed |
                                 QAbstractItemView::DoubleClicked);
     rightList->setDragDropMode(QAbstractItemView::DragDrop);
     rightList->setDragEnabled(true);

     QPushButton *leftListBtn = new QPushButton("<<");
     leftListBtn->setToolTip(tr("Move to left!"));
     QPushButton *rightListBtn = new QPushButton(">>");
     rightListBtn->setToolTip(tr("Move to right!"));

     QPushButton *addButton = new QPushButton(tr("Add"));
     rightListBtn->setToolTip(tr("Add filter!"));
      myLayout->addWidget(ListCount, 0, 0, 1, 2 );
      myLayout->addWidget(ListLabel, 0, 3, 1, 2);
      myLayout->addWidget(leftList, 1, 0, 4, 2);
      myLayout->addWidget(rightList, 1, 3, 4, 2);

      myLayout->addWidget(leftListBtn, 2, 2);
      myLayout->addWidget(rightListBtn, 3, 2);

      myLayout->addWidget(addButton, 5, 4);
      lineEdit = new QLineEdit;
      lineEdit->setToolTip(tr("You can type new filter here!"));
      myLayout->addWidget(lineEdit, 5, 3);

      QPushButton *okBtn = new QPushButton(tr("Ok"));
      QPushButton *cancelBtn = new QPushButton(tr("Cancel"));
      myLayout->addWidget(okBtn, 6, 3);
      myLayout->addWidget(cancelBtn, 6, 4);

setLayout(myLayout);
//      this->centralWidget()->setLayout(myLayout);
      connect(addButton, &QPushButton::clicked, this, &FiltersDialog::addNew);
      connect(leftListBtn, &QPushButton::clicked, this, &FiltersDialog::moveLeft);
      connect(rightListBtn, &QPushButton::clicked, this, &FiltersDialog::moveRight);

      connect(okBtn, &QPushButton::clicked, this, &FiltersDialog::okPressed);
      connect(cancelBtn, &QPushButton::clicked, this, &FiltersDialog::cancelPressed);
}

void FiltersDialog::moveLeft()
{
    const QModelIndex index = rightList->currentIndex();
    if(!index.isValid()) return;
    const QVariant data = rightModel->data(index, Qt::EditRole);
    const int leftRowCount = leftModel->rowCount();
    leftModel->insertRow(leftRowCount);
    leftModel->setData(leftModel->index(leftRowCount, 0), data, Qt::EditRole);

    rightModel->removeRow(index.row());
}

void FiltersDialog::moveRight()
{
    const QModelIndex index = leftList->currentIndex();
    if(!index.isValid()) return;
    const QVariant data = leftModel->data(index, Qt::EditRole);
    const int rightRowCount = rightModel->rowCount();
    rightModel->insertRow(rightRowCount);
    rightModel->setData(rightModel->index(rightRowCount, 0), data, Qt::EditRole);

    leftModel->removeRow(index.row());
}

void FiltersDialog::addNew()
{
  const int rowCount = rightModel->rowCount();
  rightModel->insertRow(rowCount);
  rightModel->setData(rightModel->index(rowCount,0), lineEdit->text());
}

QStringList FiltersDialog::getActiveFilters() const
{
    return rightModel->stringList();
}

QStringList FiltersDialog::getNonActiveFilters() const
{
    return leftModel->stringList();
}

void FiltersDialog::setActiveFilters(const QStringList& list)
{
    rightModel->setStringList(list);
    rightList->update();
}

void FiltersDialog::setNonActiveFilters(const QStringList& list)
{
    leftModel->setStringList(list);
    leftList->update();
}

void FiltersDialog::okPressed()
{
    emit activeFiltersSig(getActiveFilters());
    emit done(QDialog::Accepted);
}
void FiltersDialog::cancelPressed()
{
    emit done(QDialog::Rejected);
}
