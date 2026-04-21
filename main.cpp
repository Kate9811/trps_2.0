#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(R"(
QMainWindow {
    background-color: #f4f6f8;
}

QPushButton {
    background-color: #4a90e2;
    color: white;
    border-radius: 8px;
    padding: 6px 12px;
    font-weight: bold;
}

QPushButton:hover {
    background-color: #357abd;
}

QComboBox {
    padding: 5px;
    border-radius: 6px;
    background: white;
}

QProgressBar {
    border-radius: 8px;
    background: #ddd;
}

QProgressBar::chunk {
    background-color: #4a90e2;
    border-radius: 8px;
}
)");

    MainWindow window;
    window.show();

    return app.exec();
}