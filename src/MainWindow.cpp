#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>

#include <iostream>

MainWindow::MainWindow(const QByteArray& configFile, QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _thermo(configFile.data())
{
    _ui->setupUi(this);
    QMenuBar* menuBar = this->menuBar();
    menuBar->addAction("Config Dialog", &_dialog, SLOT(show()));

    this->connect(&_timer, SIGNAL(timeout()), this, SLOT(tick()));
    this->connect(_ui->_buttonCalibrate, SIGNAL(clicked()), this, SLOT(calibrate()));

    _timer.start(33);
}

MainWindow::~MainWindow(void)
{

}

void MainWindow::tick(void)
{
    _thermo.grab();

    cv::Mat image(_thermo.image());
    _imageSize = image.size();
    std::vector<cv::Point2f> centers;

    this->findPoints(centers, image);
    _ui->_thermoView->setMat(image);

    if (!centers.size() || !_ui->_buttonCapture->isChecked())
        return;

    _points.push_back(centers);
    _ui->_buttonCapture->setChecked(false);
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

    const cv::Size patternSize(_dialog.cols(), _dialog.rows());

    if (cv::findCirclesGrid(tempImage, patternSize, centers, cv::CALIB_CB_SYMMETRIC_GRID))
        cv::drawChessboardCorners(image, patternSize, cv::Mat(centers), true);
    else
        centers.clear();
}

void MainWindow::calibrate(void)
{
    std::vector<std::vector<cv::Point3f> > coords(1);

    for (unsigned int row = 0; row < _dialog.rows(); row++)
        for (unsigned int col = 0; col < _dialog.cols(); col++)
            coords[0].push_back(cv::Point3f(static_cast<float>(row) * _dialog.pointDistance(),
                                            static_cast<float>(col) * _dialog.pointDistance(),
                                            0.0));

    coords.resize(_points.size(), coords[0]);
    cv::Mat intrinsic(3, 3, CV_64F);
    cv::Mat distortion(1, 8, CV_64F);
    std::vector<cv::Mat> rvecs, tvecs;

    qDebug() << "number of points = " << _points.size();
    qDebug() << "rms error intrinsic: "
             << cv::calibrateCamera(coords, _points, _imageSize, intrinsic, distortion, rvecs, tvecs);

    std::cout << "intrinsic:" << std::endl << intrinsic << std::endl;
    std::cout << "distortion:" << std::endl << distortion << std::endl;
}
