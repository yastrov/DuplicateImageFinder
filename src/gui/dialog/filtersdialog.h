#ifndef FILTERSDIALOG_H
#define FILTERSDIALOG_H

#include <QDialog>
#include <QListView>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QStringList>
#include <QPushButton>

namespace Ui {
class FiltersDialog;
}

class FiltersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FiltersDialog(QWidget *parent = nullptr);
    virtual ~FiltersDialog();
    QStringList getActiveFilters() const;
    QStringList getNonActiveFilters() const;
    void setActiveFilters(const QStringList& list);
    void setNonActiveFilters(const QStringList& list);

private:
    Ui::FiltersDialog *ui;
    QGridLayout *myLayout;
    QLabel *ListCount, *ListLabel;
    QStringListModel *leftModel;
    QStringListModel *rightModel;
    QListView *leftList;
    QListView *rightList;
    QLineEdit *lineEdit;
    void initGUI();

private slots:
  void moveLeft();
  void moveRight();
  void addNew();
  void okPressed();
  void cancelPressed();

signals:
  void activeFiltersSig(QStringList list);
};

#endif // FILTERSDIALOG_H
