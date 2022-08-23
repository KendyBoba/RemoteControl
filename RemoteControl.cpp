#include "RemoteControl.h"

RemoteControl::RemoteControl(QWidget *parent)
    : QWidget(parent)
{
	tray = new QSystemTrayIcon();
	menu_tray = new QMenu(this);
	menu_tray->addAction("show/hide", this, &RemoteControl::slotShowHideWindow);
	menu_tray->addAction("quit", qApp, &QApplication::quit);

	tray->setContextMenu(menu_tray);
	tray->setToolTip("RemoteControl");
	tray->setIcon(QIcon(QApplication::QApplication::applicationDirPath() + "/" + "RemoteControlIcon.png"));
	tray->show();

    remote_host_widget = new remoteHost();
    connect(remote_host_widget,&remoteHost::signal_download,this,&RemoteControl::slotDownload);
    connect(remote_host_widget,&remoteHost::signal_uploadFile,this,&RemoteControl::slotUploadFile);
    connect(remote_host_widget,&remoteHost::signal_uploadDir,this,&RemoteControl::slotUploadDir);

	console_widget = new RemoteConsole();
	connect(console_widget, &RemoteConsole::signal_sendCommand, this, &RemoteControl::slotSendCommand);

	top_layout = new QHBoxLayout;
	button_startServer = new QPushButton("Start Server");
	connect(button_startServer, &QPushButton::clicked, this, &RemoteControl::startServer);
	label_startServer = new QLabel;
	label_startServer->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkred");
	top_layout->addWidget(label_startServer);
	top_layout->addWidget(button_startServer);

	center_layout = new QHBoxLayout;
	label_ip = new QLabel;
	label_ip->setText("IP");
	ip_line = new QLineEdit;
	button_downloads = new QPushButton("Downloads");
	connect(button_downloads, &QPushButton::clicked, this, &RemoteControl::slotOpenDownloads);
	center_layout->addWidget(label_ip);
	center_layout->addWidget(ip_line);
	center_layout->addWidget(button_downloads);

	bottom_layout = new QHBoxLayout;
	label_connect = new QLabel;
	label_connect->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkred");
	button_connect = new QPushButton("Connect");
	connect(button_connect, &QPushButton::clicked, this, &RemoteControl::connectToServer);
	bottom_layout->addWidget(label_connect);
	bottom_layout->addWidget(button_connect);

	menu_layout = new QVBoxLayout;
	menu_layout->setContentsMargins(this->size().width() / 4, 0, this->size().width() / 4, 0);
	menu_layout->addLayout(top_layout);
	menu_layout->addLayout(center_layout);
	menu_layout->addLayout(bottom_layout);
	this->setLayout(menu_layout);

	tabBox = new QTabWidget;
	tabBox->addTab(this, "Menu");
	tabBox->addTab(remote_host_widget, "Remote host");
	tabBox->addTab(console_widget, "Remote console");
	tabBox->setWindowIcon(QIcon(QApplication::QApplication::applicationDirPath() + "/" + "RemoteControlIcon.png"));
	tabBox->show();
	enableTab(false);

	QSettings settings("HKEY\_CURRENT\_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	settings.setValue("RemoteControl", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
}

RemoteControl::~RemoteControl()
{
	if(!isStopServer)
	server->thread()->quit();
}

void RemoteControl::interpretation(QByteArray data, type command)
{
	switch (command)
	{
	case type::console_response: {
		console_widget->consoleAddText(QTextCodec::codecForName("CP866")->toUnicode(data.constData()));
	}; break;
	case type::save_dir: {
		QDataStream stream(data);
		QByteArray data_file;
		QString path;
		stream >> path >> data_file;
		QString unsave_file_path = saveDir(data_file, path, remote_host_widget->getPathDir());
		if (unsave_file_path != "") {
			QMessageBox::information(this, "information", "file was not created " + unsave_file_path);
		}
	}; break;
	case type::save_file: {
		QDataStream stream(data);
		QByteArray data_file;
		QString path;
		stream >> path >> data_file;
		QString unsave_file_path = saveFile(data_file, path);
		if (unsave_file_path != "") {
			QMessageBox::information(this, "information", "file was not created " + unsave_file_path);
		}
		else {
			
		}
	}; break;
	case type::message: {
		QDataStream stream(data);
		QString message;
		stream >> message;
		QMessageBox::information(this, "information", message);
	}; break;
	case type::tail_message: {
		remote_host_widget->setInfo("downloaded/uploaded");
		remote_host_widget->buttonDisable(false);
	}; break;
	case type::connection_successful: {
		static int call = 0;
		++call;
		if (call % 2 != 0) {
			enableTab(true);
			button_connect->setText("Disconnect");
			label_connect->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkgreen");
		}
	}; break;
	default:
		break;
	}
}

bool RemoteControl::startServer()
{
	if (isStopServer) {
		QThread* thread_server = new QThread(this);
		server = new Server(5000);
		server->moveToThread(thread_server);
		connect(thread_server, &QThread::finished, server, &Server::deleteLater);
		thread_server->start();

		button_startServer->setText("Stop Server");
		label_startServer->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkgreen");
		isStopServer = false;
	}
	else {
		server->thread()->quit();
		server->deleteLater();
		isStopServer = true;
		label_startServer->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkred");
		button_startServer->setText("Start Server");
	}
    return isStopServer;
}

bool RemoteControl::connectToServer()
{
	static bool start = true;
	if (start) {
		QString ip = ip_line->text();
		file_client = new Client(ip, 5000);
		connect(file_client, &Client::signal_giveMessage, this, &RemoteControl::interpretation);
		connect(file_client, &Client::signal_Disconnected, this, &RemoteControl::slot_SetDisableWidget);
		console_client = new Client(ip, 5000);
		connect(console_client, &Client::signal_giveMessage, this, &RemoteControl::interpretation);
		connect(console_client, &Client::signal_Disconnected, this, &RemoteControl::slot_SetDisableWidget);
		start = false;
	}
	else {
		file_client->disconect();
		delete file_client;
		console_client->disconnect();
		delete console_client;
		button_connect->setText("Connect");
		start = true;
	}
    return start;
}

void RemoteControl::slotOpenDownloads()
{
	QString path_to_downloads = QApplication::applicationDirPath() + "/download";
	QString urlPath = "file:///" + path_to_downloads;
	if (!QDesktopServices::openUrl(QUrl(urlPath, QUrl::TolerantMode))) {
		QMessageBox::warning(this, "Warning", "Unable to open downloads");
	}
}

void RemoteControl::slot_SetDisableWidget()
{
	enableTab(false);
	button_connect->setText("Connect");
	label_connect->setStyleSheet("border-radius:5px;max-height:10px;min-height:10px;max-width:10px;min-width:10px;background-color:darkred");
}

void RemoteControl::slotAutoLoad()
{
	QAction* action = (QAction*)sender();
	if (!action->isChecked()) {
		#ifdef Q_OS_WIN32
			QSettings settings("HKEY\_CURRENT\_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
			if (settings.value("RemoteControl").toString() != "") {
				settings.setValue("RemoteControl", "");
			}
		#endif
	}
	else {
		#ifdef Q_OS_WIN32
			QSettings settings("HKEY\_CURRENT\_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
			settings.setValue("RemoteControl", QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
		#endif
	}
	action->setChecked(action->isChecked());
}

void RemoteControl::slotShowLogs()
{
	static bool show = true;
	if (show) {
		server->showConsole(show);
		show = false;
	}
	else {
		server->showConsole(show);
		show = true;
	}
}

void RemoteControl::slotShowHideWindow()
{
	tabBox->setVisible(!tabBox->isVisible());
}

void RemoteControl::sendTailMessage()
{
	QByteArray send_data;
	QDataStream stream(&send_data, QIODevice::WriteOnly);
	stream << "tail messaage";
	file_client->sendData(send_data, type::tail_message);
}

void RemoteControl::enableTab(bool enable)
{
	int i = tabBox->tabBar()->count();
	while (i > 0) {
		tabBox->tabBar()->setTabEnabled(i, enable);
		--i;
	}
}

void RemoteControl::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu *context_menu = new QMenu;
	QAction* auto_load = context_menu->addAction("Auto load", this, &RemoteControl::slotAutoLoad);
	auto_load->setCheckable(true);
	auto_load->setChecked(true);
	if (!isStopServer) {
		QIcon show_icon(QApplication::applicationDirPath() + "/" + "show_image.png");
		context_menu->addAction(show_icon,"Show logs", this, &RemoteControl::slotShowLogs);
	}
	QIcon close_icon(QApplication::applicationDirPath() + "/" + "close_image.png");
	context_menu->addAction(close_icon,"Close");
	context_menu->exec(event->globalPos());
}

void RemoteControl::closeEvent(QCloseEvent* event)
{
	setVisible(false);
}

void RemoteControl::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void RemoteControl::slotUploadFile(QString path)
{
	remote_host_widget->setInfo("");
	remote_host_widget->buttonDisable(true);
	QByteArray send_data;
	QDataStream stream(&send_data, QIODevice::WriteOnly);
	QByteArray data_file = ReadFile(path);
	if (data_file.size() == 0) {
		QMessageBox::information(this, "information", "wrong path or empty file " + remote_host_widget->getPathDir() + "/" + path);
		return;
	}
	stream << path << data_file;
	file_client->sendData(send_data, type::save_file);
	sendTailMessage();
}

void RemoteControl::slotUploadDir(QString path)
{
	remote_host_widget->setInfo("");
	remote_host_widget->buttonDisable(true);
	QByteArray send_data;
	QDataStream stream(&send_data, QIODevice::WriteOnly);
	QByteArray data_file = ReadFile(path);
	if (data_file.size() == 0) {
		QMessageBox::information(this, "information", "wrong path or empty file " + remote_host_widget->getPathDir() + "/" + path);
		return;
	}
	stream << remote_host_widget->getPathDir() << path << data_file;
	file_client->sendData(send_data, type::save_dir);
	sendTailMessage();
}

void RemoteControl::slotSendCommand(QString command) 
{
	QByteArray send_data;
	QDataStream stream(&send_data, QIODevice::WriteOnly);
	stream << command;
	console_client->sendData(send_data, type::console_command);
}

void RemoteControl::slotDownload(QString path) {
	remote_host_widget->setInfo("");
	remote_host_widget->buttonDisable(true);
    QByteArray send_data;
    QDataStream stream(&send_data, QIODevice::WriteOnly);
    stream << path;
    file_client->sendData(send_data, type::download);
	sendTailMessage();
}
