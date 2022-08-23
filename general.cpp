#include "general.h"

QString deleteEmptySumbol(QString str) {
	QString return_str = str;
	if (return_str[return_str.length() - 1] == '\0') {
		return_str = return_str.remove(return_str.length() - 1, 1);
	}
	return return_str;
}

QString saveDir(QByteArray& data_file, QString path, QString dir_path)
{
	QDir().mkdir(QApplication::applicationDirPath() + "/download");
	QString dir_name = dir_path.split("/").back();
	QString pathDir = QApplication::applicationDirPath() + "/download/" + dir_name;
	QDir().mkdir(pathDir);
	QDir dir(pathDir);

	QString local_path = "";
	for (int i = dir_path.size() + 1, j = i - 1; i < path.size(); ++i, --j) {
		local_path.append(path[i]);
	}
	QStringList list_name = local_path.split("/");
	list_name.erase(list_name.end() - 1);
	QString new_path = pathDir;
	for (auto& el : list_name) {
		new_path += "/" + el;
		QDir().mkdir(new_path);
	}
	QString filePath = new_path + "/" + path.split("/").back();
	QFile file(deleteEmptySumbol(filePath));
	if (file.open(QIODevice::ReadWrite)) {
		file.write(data_file);
	}
	else {
		return filePath;
	}
	return QString("");
}

QString saveFile(QByteArray& data_file, QString path)
{
	QString pathDir = QApplication::applicationDirPath() + "/download";
	QDir().mkdir(pathDir);
	QDir dir(pathDir);
	QString filePath = pathDir + "/" + path.split("/").back();
	QFile file(deleteEmptySumbol(filePath));
	if (file.open(QIODevice::WriteOnly)) {
		file.write(data_file);
	}
	else {
		return filePath;
	}
	return QString("");
}

bool isFile(QString path)
{
	QFileInfo fileinfo(path);
	if (fileinfo.isFile()) {
		return true;
	}
	return false;
}

QString replacementText(QString path)
{
	QString result = path;
	for (int i = 0; i < path.length(); ++i) {
		if (path[i] == '\\') {
			result[i] = '/';
		}
	}
	return result;
}

QByteArray ReadFile(QString path)
{
	QByteArray data;
	QFile file(deleteEmptySumbol(path));
	if (file.open(QIODevice::ReadOnly)) {
		data = file.readAll();
		return data;
	}
	else {
		QByteArray();
	}
}

QString fromType(type command)
{
	QString result;
	switch (command)
	{
	case type::null_command:
		result = " null command";
		break;
	case type::console_command:
		result = "console command";
		break;
	case type::console_response:
		result = "console response";
		break;
	case type::download:
		result = "download";
		break;
	case type::save_file:
		result = "save file";
		break;
	case type::message:
		result = "message";
		break;
	case type::save_dir:
		result = "save dir";
		break;
	case type::tail_message:
		result = "tail message";
		break;
	default:
		break;
	}
	return result;
}
