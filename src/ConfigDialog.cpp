#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget* parent)
    : QDialog(parent),
      _ui(new Ui::ConfigDialog)
{
    _ui->setupUi(this);
}

float ConfigDialog::threshold(void) const
{
    return _ui->_spinThreshold->value();
}

unsigned int ConfigDialog::rows(void) const
{
    return static_cast<unsigned int>(_ui->_spinRows->value());
}

unsigned int ConfigDialog::cols(void) const
{
    return static_cast<unsigned int>(_ui->_spinCols->value());
}
