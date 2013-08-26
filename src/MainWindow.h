#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

#include <QMainWindow>
#include <QTimer>
#include <QByteArray>

#include "ThermoCam.h"
#include "ConfigDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QByteArray& configFile, QWidget* parent = 0);
    virtual ~MainWindow(void);

private slots:
    void tick(void);
    void calibrate(void);

private:
    void findPoints(std::vector<cv::Point2f>& centers, cv::Mat& image);

    Ui::MainWindow* _ui;
    QTimer _timer;
    ThermoCam _thermo;
    ConfigDialog _dialog;
    std::vector<std::vector<cv::Point2f> > _points;
    cv::Size _imageSize;
};

#endif
