#include "MainWindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qDebug() << "usage " << argv[0] << " <config file>";
        return 1;
    }

    QApplication app(argc, argv);

    MainWindow mw(argv[1]);
    mw.show();

    return app.exec();
}
