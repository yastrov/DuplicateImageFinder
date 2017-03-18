#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::setMaxImageHeight(int value)
{
    ui->maxImageHeightSpinBox->setValue(value);
}

int SettingsDialog::getMaxImageHeight()
{
    return ui->maxImageHeightSpinBox->value();
}

void SettingsDialog::setFontC(const QFont &font)
{
    ui->fontComboBox->setCurrentFont(font);
    ui->fonSizeSpinBox->setValue(font.pointSize());
}

QFont SettingsDialog::getFontC()
{
    return ui->fontComboBox->currentFont();
}

void SettingsDialog::on_fonSizeSpinBox_valueChanged(int arg1)
{
    QFont font = getFontC();
    font.setPointSize(arg1);
    setFontC(font);
}
