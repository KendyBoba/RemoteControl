#pragma once
#include <QtNetwork/QTcpSocket.h>
#include <QtCore/qthread.h>
#include "command.h"
#include <QtCore/qdatastream.h>
#include <vector>

class Client : public QObject
{
	Q_OBJECT
private:
	QString ip = "";
	int port = 0;
private:
	QTcpSocket* socket = nullptr;
	int nextBlockSize = 0;
	QByteArray buff_data;
	type current_command = type::null_command;
public:
	Client(QString ip, int port, QObject* parent = nullptr);
	void sendData(QByteArray& data, type command);
	void disconect();
private slots:
	void slotRead();
	void slotDisconnected();
signals:
	void signal_giveMessage(QByteArray data,type command);
	void signal_Disconnected();
};