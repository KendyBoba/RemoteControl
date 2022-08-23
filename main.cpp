#include "RemoteControl.h"
#include <QtWidgets/QApplication>

void initStyle() {
    QString path = QCoreApplication::applicationDirPath() + "/appStyle.css";
    QFile styleFile(path);
    if (styleFile.open(QIODevice::ReadOnly)) {
        QString style = styleFile.readAll();
        qApp->setStyleSheet(style);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteControl w;
    QApplication::setQuitOnLastWindowClosed(false);
    initStyle();
    w.show();
    return a.exec();
}
