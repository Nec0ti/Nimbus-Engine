#include "MainWindow.h"
#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[]) {
    // Wayland/XCB uyumluluk sorunları için platformu manuel olarak ayarla
    qputenv("QT_QPA_PLATFORM", QByteArray("xcb"));

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}