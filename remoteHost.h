#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include <stack>
#include "general.h"
#include <QtWidgets/qfiledialog.h>
#include <qfilesystemmodel.h>
#include <qtreeview.h>
#include <QtGui/qdesktopservices.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qlabel.h>
#include <qmimedata>
#include <QtGui/QDragEnterEvent>
#include <QtGui/qpainter.h>
#include <qmessagebox.h>

class remoteHost : public QWidget
{
	Q_OBJECT
private:
	QString current_path_dir = "";
	QString open_path = "";
private:
	QHBoxLayout* hlayout = nullptr;
	QVBoxLayout* vlayout = nullptr;
	QLineEdit* line_path = nullptr;
	QPushButton* button_download = nullptr;
	QPushButton* button_upload = nullptr;
	QTreeView* file_view = nullptr;
	QFileSystemModel* tree_model = nullptr;
	QMenu* menu = nullptr;
	QLabel* info = nullptr;
public:
	remoteHost(QWidget* parent = nullptr);
	~remoteHost();
	QString getPathDir();
	void buttonDisable(bool disable);
	void setInfo(QString info_text);
	void uploadDir(QString path);
	void uploadFile(QString path);
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	void paintEvent(QPaintEvent* event);
private slots:
	void slotDownload();
	void slotUpload();
	void slotClickOnView(const QModelIndex& index);
	void slotOpenFile();
signals:
	void signal_download(QString path);
	void signal_uploadFile(QString path);
	void signal_uploadDir(QString path);
};