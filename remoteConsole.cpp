#include "remoteConsole.h"

RemoteConsole::RemoteConsole(QWidget* parent) :QWidget(parent)
{
	hlayout = new QHBoxLayout;
	vlayout = new QVBoxLayout;
	QIcon arrow_icon(QApplication::applicationDirPath() + "/" + "arrow_image.png");
	button_send = new QPushButton(arrow_icon,"");
	connect(button_send, &QPushButton::clicked, this, &RemoteConsole::slotSendCommand);
	console = new QTextEdit;
	command_line = new QLineEdit;

	hlayout->addWidget(command_line);
	hlayout->addWidget(button_send);
	vlayout->addWidget(console);
	vlayout->addLayout(hlayout);
	setLayout(vlayout);
}

void RemoteConsole::consoleAddText(QString text)
{
	console->append(text);
}

bool RemoteConsole::event(QEvent* event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent* key_event = dynamic_cast<QKeyEvent*>(event);
		if (key_event->key() + 1 == Qt::Key_Enter) {
			button_send->clicked();
		}
		else if(key_event->key() == Qt::Key_Up) {
			if (commands_history.size() <= 0) {
				return false;
			}
			if (it_command != commands_history.begin())
				std::advance(it_command, -1);
			command_line->setText(*it_command);
		}
	}
	else if (event->type() == QEvent::Paint) {
		QStyleOption opt;
		opt.init(this);
		QPainter p(this);
		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
	}
	return true;
}

void RemoteConsole::slotTextPaste()
{
	QAction* action = static_cast<QAction*>(sender());
	command_line->setText(action->text());
}

void RemoteConsole::slotSendCommand() {
	commands_history.push_back(replacementText(command_line->text()));
	emit signal_sendCommand(replacementText(command_line->text()));
	it_command = commands_history.end();
	command_line->clear();
}