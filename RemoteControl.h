#pragma once
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qtabwidget>
#include <QtCore/qtextcodec.h>
#include "Server.h"
#include "Client.h"
#include "remoteHost.h"
#include "remoteConsole.h"
#include <QtGui/QContextMenuEvent>
#include <QtCore/qsettings.h>
#include <qsystemtrayicon.h>
#include <QtGui/qpainter.h>

class RemoteControl : public QWidget
{
    Q_OBJECT
public:
    RemoteControl(QWidget *parent = nullptr);
    ~RemoteControl();
private:
    void interpretation(QByteArray data, type command);
private:
    QTabWidget* tabBox = nullptr;
private:
    remoteHost* remote_host_widget = nullptr;
    RemoteConsole* console_widget = nullptr;
    Server* server = nullptr;
    Client* file_client = nullptr;
    Client* console_client = nullptr;
private:
    QHBoxLayout* top_layout = nullptr;
    QHBoxLayout* center_layout = nullptr;
    QHBoxLayout* bottom_layout = nullptr;
    QVBoxLayout* menu_layout = nullptr;

    QPushButton* button_startServer = nullptr;
    QPushButton* button_connect = nullptr;
    QPushButton* button_downloads = nullptr;

    QLineEdit* ip_line = nullptr;

    QLabel* label_startServer = nullptr;
    QLabel* label_connect = nullptr;
    QLabel* label_ip = nullptr;


    QSystemTrayIcon* tray = nullptr;
    QMenu *menu_tray = nullptr;

private:
    bool isStopServer = true;

private:
    void sendTailMessage();
    void enableTab(bool enable);
    virtual void contextMenuEvent(QContextMenuEvent* event);
    void closeEvent(QCloseEvent* event);
    void paintEvent(QPaintEvent* event);
private slots:
    void slotDownload(QString path);
    void slotUploadFile(QString path);
    void slotUploadDir(QString path);
    void slotSendCommand(QString command);
    bool startServer();
    bool connectToServer();
    void slotOpenDownloads();
    void slot_SetDisableWidget();
    void slotAutoLoad();
    void slotShowLogs();
    void slotShowHideWindow();
};
