#include "remoteHost.h"

remoteHost::remoteHost(QWidget* parent) : QWidget(parent)
{
	hlayout = new QHBoxLayout;
	vlayout = new QVBoxLayout;

	line_path = new QLineEdit;
	info = new QLabel(this);
	QIcon download_icon(QApplication::applicationDirPath() + "/" + "download_image.png");
	button_download = new QPushButton(download_icon,"Download");
	connect(button_download, &QPushButton::clicked, this, &remoteHost::slotDownload);
	QIcon upload_icon(QApplication::applicationDirPath() + "/" + "upload_image.png");
	button_upload = new QPushButton(upload_icon,"Upload");
	connect(button_upload, &QPushButton::clicked, this, &remoteHost::slotUpload);
	file_view = new QTreeView;
	connect(file_view, &QTreeView::clicked, this, &remoteHost::slotClickOnView);
	tree_model = new QFileSystemModel(this);
	tree_model->setRootPath(QApplication::applicationDirPath() + "/download");
	file_view->setModel(tree_model);

	hlayout->addWidget(line_path);
	hlayout->addWidget(button_download);
	hlayout->addWidget(button_upload);

	vlayout->addLayout(hlayout);
	vlayout->addWidget(file_view);
	vlayout->addWidget(info);
	this->setLayout(vlayout);

	menu = new QMenu(this);
	QIcon send_icon(QApplication::applicationDirPath() + "/" + "upload_image.png");
	QIcon open_icon(QApplication::applicationDirPath() + "/" + "open_image.png");
	QIcon close_icon(QApplication::applicationDirPath() + "/" + "close_image.png");
	menu->addAction(send_icon,"Send", this, &remoteHost::slotUpload);
	menu->addAction(open_icon,"Open", this, &remoteHost::slotOpenFile);
	menu->addAction(close_icon,"Close");

	setAcceptDrops(true);
}

remoteHost::~remoteHost() {
	
}

QString remoteHost::getPathDir()
{
	return replacementText(current_path_dir);
}

void remoteHost::buttonDisable(bool disable)
{
	button_download->setEnabled(!disable);
	button_upload->setEnabled(!disable);
}

void remoteHost::remoteHost::setInfo(QString info_text)
{
	info->setText(info_text);
}

void remoteHost::uploadDir(QString path)
{
	QString mask = "*";
	std::stack<QString> stack;
	stack.push(path);
	while (!stack.empty()) {
		QApplication::processEvents();
		QDir dir(stack.top());
		QStringList fileList = dir.entryList(mask.split(" "), QDir::Files);
		dir.setSorting(QDir::Size);
		for (QString file : fileList) {
			QString path = dir.absoluteFilePath(file);
			emit signal_uploadDir(path);
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

void remoteHost::uploadFile(QString path)
{
	emit signal_uploadFile(path);
}

void remoteHost::dragEnterEvent(QDragEnterEvent* event)
{
	if(event->mimeData()->hasUrls())
	event->acceptProposedAction();
}

void remoteHost::dropEvent(QDropEvent* event)
{
	QList<QUrl> list = event->mimeData()->urls();
	QString path = list.last().toString();
	path.remove(0, 8);
	line_path->setText(path);
}

void remoteHost::paintEvent(QPaintEvent* event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void remoteHost::slotDownload() {
	current_path_dir = replacementText(line_path->text());
	emit signal_download(current_path_dir);
}

void remoteHost::slotUpload() {
	current_path_dir = replacementText(line_path->text());
	QFileInfo info(current_path_dir);
	if (info.isDir()) {
		uploadDir(current_path_dir);
	}
	else {
		uploadFile(current_path_dir);
	}
}

void remoteHost::slotOpenFile()
{
	QFileInfo info(open_path);
	if (info.isFile()) {
		QString url_path = "file:///" + info.absoluteFilePath();
		if (!QDesktopServices::openUrl(QUrl(url_path, QUrl::TolerantMode))) {
			QMessageBox::warning(this, "Warning", "Unable to open file");
		}
	}
}

void remoteHost::slotClickOnView(const QModelIndex& index)
{
	QFileInfo info = tree_model->fileInfo(index);
	if (info.isFile()) {
		open_path = info.absoluteFilePath();
		line_path->setText(info.absoluteFilePath());
		menu->popup(QCursor::pos());
		menu->show();
	}
	else {
		line_path->setText(info.absoluteFilePath());
	}
}
