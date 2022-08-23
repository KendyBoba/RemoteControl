#pragma once
#include <QtNetwork/qtcpsocket.h>
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
#include <thread>

class Socket : public QObject
{
	Q_OBJECT
private:
	QByteArray buff_data;
	int nextBlockSize = 0;
	type current_command = type::null_command;
private:
	bool start_sock = false;
	qintptr descriptor;
	QProcess* proc = nullptr;
public:
	QTcpSocket *socket = nullptr;
public:
	Socket(qintptr descriptor, QObject* parent = nullptr);
	~Socket();
private:
	void interpretation(QByteArray data, type command);
	void downloadDir(QString path, type command);
	void downloadFile(QString path, type command);
	void sendMessage(QString message, QString path);
	void startProcc(QProcess* proc);
	void sendTailMessage();
public:
	void sendData(QByteArray& data, type command);
	void sendData(type command);
	int getDescriptor();
private slots:
	void slotRead();
	void slotDeleteSocket();
public slots:
	void run();
signals:
	void signal_GiveMessage(Socket* socket,QByteArray data,int command);
	void signal_SocketDeleted(int descriptor);
};

