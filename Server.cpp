#include "Server.h"
Server::Server(int port,QObject * parent) : QTcpServer(parent) {
	if (!listen(QHostAddress::Any, port)) {
		QMessageBox::critical(0, "Server Error", "Unable to start the server - " + errorString());
		close();
		return;
	}
	
	console = new QTextEdit;

}

Server::~Server()
{
	for (Socket* el : clients) {
		el->thread()->quit();
	}
	clients.clear();
}

void Server::showConsole(bool show) {
	if (show) {
		console->show();
	}
	else {
		console->hide();
	}
}


void Server::incomingConnection(qintptr descriptor)
{
	QThread* thread_socket = new QThread(this);
	Socket *socket = new Socket(descriptor);
	socket->moveToThread(thread_socket);
	connect(thread_socket, &QThread::started, socket, &Socket::run);
	connect(thread_socket, &QThread::finished, socket, &Socket::deleteLater);
	connect(socket, &Socket::signal_GiveMessage, this, &Server::slotTakeMessage);
	connect(socket, &Socket::signal_SocketDeleted, this, &Server::slotSocketDeleteFromClients);
	thread_socket->start();
	clients.push_back(socket);
}

void Server::slotSocketDeleteFromClients(int descriptor)
{
	auto it = clients.begin();
	for (it; it != clients.end(); std::advance(it, 1)) {
		if ((*it)->getDescriptor() == descriptor) {
			clients.erase(it,it+1);
			break;
		}
	}
}


void Server::slotTakeMessage(Socket* socket, QByteArray data, int command) {
	for (Socket* el : clients) {
		if (el->socket->socketDescriptor() == socket->socket->socketDescriptor()) {
			QDataStream stream(data);
			QString descriptor = QString::number(el->socket->socketDescriptor());
			QString command_int = fromType((type)command);
			switch ((type)command)
			{
			case type::console_command: {
				QString command_str;
				stream >> command_str;
				QString text = descriptor + "--" + command_str + "--" + command_int;
				console->append(text);
			}; break;
			case type::download: {
				QString path;
				stream >> path;
				QString text = descriptor + "--" + path + "--" + command_int;
				console->append(text);
			}; break;
			case type::save_file: {
				QString path;
				stream >> path;
				QString text = descriptor + "--" + path + "--" + command_int;
				console->append(text);
			}; break;
			case type::save_dir: {
				QString dir_path;
				QString file_path;
				stream >> dir_path;
				QString text = descriptor + "--" + "folder: " + dir_path + "--" + "file: " + file_path + "--" + command_int;
				console->append(text);
			}; break;
			default: ; break;
			}
			break;
		}
	}
}
