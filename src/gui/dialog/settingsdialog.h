#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    virtual ~SettingsDialog();
    void setMaxImageHeight(int value);
    int getMaxImageHeight();
    void setFontC(const QFont &font);
    QFont getFontC();

private slots:
    void on_fonSizeSpinBox_valueChanged(int arg1);

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
