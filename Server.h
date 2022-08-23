#pragma once
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/qtcpserver.h>
#include <QtCore/qobject.h>
#include <QtWidgets/qmessagebox.h>
#include "command.h"
#include <deque>
#include <QtCore/qprocess.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <stack>
#include "general.h"
#include <QtCore/qthread.h>
#include <qfilesystemmodel.h>
#include "Socket.h"
#include "QtWidgets/qtextedit.h"
#include <thread>

class Server : public QTcpServer
{
	Q_OBJECT
private:
	std::deque<Socket*> clients;
	QTextEdit* console = nullptr;
public:
	Server(int port,QObject * parent = nullptr);
	~Server();
	void incomingConnection(qintptr descriptor);
private slots:
	void slotTakeMessage(Socket* socket, QByteArray data, int command);
public slots:
	void showConsole(bool show);
	void slotSocketDeleteFromClients(int descriptor);
};