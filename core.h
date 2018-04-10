#ifndef CORE_H
#define CORE_H

#include <QObject>
#include "mcipc.h"
#include <QJsonObject>
#include <QTcpSocket>
#include <QTcpServer>

class DataProvider
{
public:
	QString origionator;
	QList<MCIPC*> subscriberList;
	quint32 messageCount;
};
class Core : public QObject
{
	Q_OBJECT
public:
	explicit Core(QObject *parent = 0);
private:
	QMap<QString,DataProvider*> m_dataMap;
	MCIPC *m_ipc;
	QTcpServer *m_server;
	QList<MCIPC*> m_unAuthedConnections;
	QList<MCIPC*> m_authedConnections;
private slots:
	void ipcJsonPacket(QJsonObject object);
	void jsonPacketReceived(QString sender,QJsonObject message);
	void subscribeMessage(QString message);
	void publishMessage(QString name,QByteArray payload);
	void serverNewConnection();
	void publishStatistics();
signals:

public slots:
};

#endif // CORE_H
