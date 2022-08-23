#include "Client.h"

Client::Client(QString ip, int port, QObject* parent) : QObject(parent),ip(ip), port(port)
{
	socket = new QTcpSocket;
	socket->connectToHost(ip, port);
	connect(socket, &QTcpSocket::readyRead, this, &Client::slotRead, Qt::DirectConnection);
	connect(socket, &QTcpSocket::disconnected, this, &Client::slotDisconnected, Qt::DirectConnection);
}


void Client::sendData(QByteArray& data, type command)
{
	QByteArray send_data;
	QDataStream stream(&send_data, QIODevice::WriteOnly);
	int size = data.size() + sizeof(int);
	stream << size << command << data;
	socket->write(send_data);
}

void Client::disconect()
{
	socket->disconnected();
}

void Client::slotDisconnected()
{
	socket->close();
	emit signal_Disconnected();
}

 
void Client::slotRead() {
	QDataStream stream(socket);
	while (socket->bytesAvailable() > 0) {
		if (nextBlockSize == 0) {
			QByteArray arr;
			arr.append(socket->read(sizeof(int)));
			arr.append(socket->read(sizeof(int)));
			QDataStream s(arr);
			s >> nextBlockSize >> current_command;
		}
		buff_data.append(socket->read(nextBlockSize - buff_data.size()));
		if (buff_data.size() >= nextBlockSize) {
			nextBlockSize = 0;
			QByteArray send_data;
			QDataStream in(buff_data);
			in >> send_data;
			emit signal_giveMessage(send_data, current_command);
			buff_data.clear();
			current_command = type::null_command;
		}
	}
}
