#pragma once
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>
#include "general.h"
#include <QtWidgets/qfiledialog.h>
#include <QtGui/qdesktopservices.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qtextedit.h>
#include <qevent>
#include <qkeyevent>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qstyleoption.h>
#include <QtGui/qpainter.h>
#include <vector>
#include <QtGui/qicon.h>
#include <qlistwidget.h>
#include <qcheckbox.h>

class RemoteConsole : public QWidget
{
	Q_OBJECT
private:
	QHBoxLayout* hlayout = nullptr;
	QVBoxLayout* vlayout = nullptr;
	QPushButton* button_send = nullptr;
	QLineEdit* command_line = nullptr;
	QTextEdit* console = nullptr;

	std::vector<QString> commands_history;
	std::vector<QString>::iterator it_command;
public:
	RemoteConsole(QWidget* parent = nullptr);
	void consoleAddText(QString text);
	bool event(QEvent* event);
private slots:
	void slotSendCommand();
	void slotTextPaste();
signals:
	void signal_sendCommand(QString command);
};