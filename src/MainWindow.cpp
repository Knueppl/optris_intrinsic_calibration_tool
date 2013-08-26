#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(const QByteArray& configFile, QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _thermo(configFile.data())
{
    _ui->setupUi(this);
    QMenuBar* menuBar = this->menuBar();
    menuBar->addAction("Config Dialog", &_dialog, SLOT(show()));

    this->connect(&_timer, SIGNAL(timeout()), this, SLOT(tick()));

    _timer.start(33);
}

MainWindow::~MainWindow(void)
{

}

void MainWindow::tick(void)
{
    _thermo.grab();

    cv::Mat image(_thermo.image());
    std::vector<cv::Point2f> centers;

    this->findPoints(centers, image);
    _ui->_thermoView->setMat(image);
}

void MainWindow::findPoints(std::vector<cv::Point2f>& centers, cv::Mat& image)
{
    const cv::Mat& temperature = _thermo.temperature();
    const unsigned short tempMin = static_cast<unsigned short>(_dialog.threshold() * 10);

    cv::Mat tempImage(temperature.rows, temperature.cols, CV_8UC1);

    for (int row = 0; row < temperature.rows; row++)
    {
        const uint16_t* dataTemperature = reinterpret_cast<const uint16_t*>(temperature.ptr(row));
        unsigned char* dataTempImage = tempImage.ptr(row);

        for (int col = 0; col < temperature.cols; col++, dataTemperature++)
        {
            const unsigned short temp = *dataTemperature - 1000;

            if (temp < tempMin)
                *dataTempImage++ = 0xff;
            else
                *dataTempImage++ = 0x00;
        }
    }

    _ui->_thresholdView->setMat(tempImage);

    const cv::Size patternSize(_dialog.rows(), _dialog.cols());

    if (cv::findCirclesGrid(tempImage, patternSize, centers, cv::CALIB_CB_SYMMETRIC_GRID))
        cv::drawChessboardCorners(image, patternSize, cv::Mat(centers), true);
    else
        centers.clear();

    _ui->_thermoView->setMat(image);
}
