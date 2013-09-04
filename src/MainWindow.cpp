#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDebug>
#include <QString>
#include <QTextStream>

#include <iostream>

namespace {
const char* TEXT_LABEL_RESULT = "Intrinsic Matrix:\n"
                                "----------------------------\n";
}

MainWindow::MainWindow(const QByteArray& configFile, QWidget* parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _thermo(configFile.data())
{
    _ui->setupUi(this);
    _ui->_buttonCalibrate->setDisabled(true);

    QMenuBar* menuBar = this->menuBar();
    menuBar->addAction("Config Dialog", &_dialog, SLOT(show()));
    menuBar->addAction("Save", this, SLOT(saveToFile()));
    menuBar->addAction("Undistort", &_undistortView, SLOT(show()));

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


    if (!_intrinsic.empty() && !_distortion.empty())
    {
        cv::Mat undistortedImage;
        cv::undistort(image, undistortedImage, _intrinsic, _distortion);
        _undistortView.setMat(undistortedImage);
    }


    if (!centers.size() || !_ui->_buttonCapture->isChecked())
        return;

    _points.push_back(centers);
    _ui->_buttonCapture->setChecked(false);
    _ui->_buttonCalibrate->setEnabled(true);
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

    QString out(TEXT_LABEL_RESULT);
    QTextStream stream(&out);
    stream.setRealNumberPrecision(3);
    stream.setRealNumberNotation(QTextStream::FixedNotation);

    stream << "number of points   : " << _points.size() << "\n";
    stream << "rms error intrinsic: "
           << cv::calibrateCamera(coords, _points, _imageSize, intrinsic, distortion, rvecs, tvecs)
           << "\n\n";

    stream << "intrinsic:\n";
    this->cvMatToQString(out, intrinsic);
    stream << "\n";

    stream << "distortion:\n";
    this->cvMatToQString(out, distortion);

    _ui->_labelResult->setText(out);
    intrinsic.copyTo(_intrinsic);
    distortion.copyTo(_distortion);
}

void MainWindow::cvMatToQString(QString& string, const cv::Mat& mat)
{
    QTextStream stream(&string);
    stream.setFieldAlignment(QTextStream::AlignRight);
    stream.setRealNumberPrecision(3);
    stream.setRealNumberNotation(QTextStream::FixedNotation);

    for (int col = 0; col < mat.cols; col++)
    {
        for (int row = 0; row < mat.rows; row++)
        {
            stream.setFieldWidth(8);

            switch (mat.type())
            {
            case CV_64F:
                stream << mat.at<double>(row, col);
                break;

            default:
                break;
            }

            stream.setFieldWidth(0);
            stream << " ";
        }

        stream << "\n";
    }
}

void MainWindow::saveToFile(void)
{
    if (_intrinsic.empty() || _distortion.empty())
    {
        qDebug() << __PRETTY_FUNCTION__ << ": calibration matrix is empty.";
        return;
    }

    cv::FileStorage fs("calibration.xml", cv::FileStorage::WRITE);

    fs << "intrinsic" << _intrinsic;
    fs << "distortion" << _distortion;
}
