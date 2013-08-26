#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow(void);

private:
    Ui::MainWindow* _ui;
};

#endif
