#pragma once
#include <QtCore/qstring.h>
#include <QtCore/qdatastream.h>
#include <QtCore/QDir>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qlineedit.h>
#include "command.h"
QString deleteEmptySumbol(QString str);
QString saveDir(QByteArray& data_file, QString path, QString dir_path);
QString saveFile(QByteArray& data_file, QString path);
bool isFile(QString path);
QString replacementText(QString path);
QByteArray ReadFile(QString path);
QString fromType(type command);