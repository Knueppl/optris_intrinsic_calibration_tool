#ifndef __CONFIG_DIALOG__
#define __CONFIG_DIALOG__

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
public:
    ConfigDialog(QWidget* parent = 0);

    float threshold(void) const;
    unsigned int rows(void) const;
    unsigned int cols(void) const;

private:
    Ui::ConfigDialog* _ui;
};

#endif
