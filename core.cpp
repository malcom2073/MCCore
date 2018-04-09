#include "core.h"
#include <QDebug>
#include <QJsonDocument>

Core::Core(QObject *parent) : QObject(parent)
{
	m_server = new QTcpServer(this);
	connect(m_server,SIGNAL(newConnection()),this,SLOT(serverNewConnection()));
	m_server->listen(QHostAddress::LocalHost,12345);

	m_ipc = new MCIPC("core",this);
//	m_ipc->startServer(12345);
	connect(m_ipc,SIGNAL(si_jsonPacketReceived(QString,QJsonObject)),this,SLOT(jsonPacketReceived(QString,QJsonObject)));
	connect(m_ipc,SIGNAL(si_subscribeMessage(QString)),this,SLOT(subscribeMessage(QString)));
	connect(m_ipc,SIGNAL(si_publishMessage(QString,QByteArray)),this,SLOT(publishMessage(QString,QByteArray)));
}

void Core::jsonPacketReceived(QString sender,QJsonObject message)
{
	qDebug() << "Got message from:" << sender << message;
	if (!message.contains("type"))
	{
		return;
	}
	QString packettype = message.value("type").toString();
	if (packettype == "advertise")
	{
		//We are letting the core know that we have a channel available!
	}
	else if (packettype == "unadvertise")
	{
		//Channel is no longer available
	}
	else if (packettype == "subscribe")
	{
		//Subscribe letting the core know that we want to be notified of channel activity
	}
	else if (packettype == "unsubscribe")
	{
		//Unsubscribe, letting the core know we do not want channel updates.
	}
	else if (packettype == "list-connections")
	{
		//Get a list if all connected clients
	}
	else if (packettype == "list-channels")
	{
		//Get a list of all available channels
	}
	else if (packettype == "call-function")
	{

	}

}
void Core::subscribeMessage(QString message)
{
	MCIPC *ipcsender = qobject_cast<MCIPC*>(sender());
	if (!ipcsender)
	{
		return;
	}
	if (!m_authedConnections.contains(ipcsender))
	{
		qDebug() << "Unauthorized subscribe message";
		return;
	}

	qDebug() << "Incoming subscribe-to message:" << message;
	if (!m_dataMap.contains(message))
	{
		m_dataMap.insert(message,new DataProvider());
	}
	m_dataMap[message]->subscriberList.append(ipcsender);
}

void Core::publishMessage(QString name,QByteArray payload)
{
	MCIPC *ipcsender = qobject_cast<MCIPC*>(sender());
	if (!ipcsender)
	{
		return;
	}
	if (!m_authedConnections.contains(ipcsender))
	{
		qDebug() << "Unauthorized publish message";
		return;
	}
	qDebug() << "Incoming publish message:" << name<< payload;
	if (m_dataMap.contains(name))
	{
		for (int i=0;i<m_dataMap[name]->subscriberList.size();i++)
		{
			m_dataMap[name]->subscriberList.at(i)->publishMessage(name,payload);
//			m_dataMap[message]->subscriberList.at(i)->write()
		}
	}
}
void Core::serverNewConnection()
{
	qDebug() << "Incoming new connection";
	QTcpSocket *socket = m_server->nextPendingConnection();
	MCIPC *newipc = new MCIPC(socket,this);
	m_unAuthedConnections.append(newipc);
	connect(newipc,SIGNAL(si_publishMessage(QString,QByteArray)),this,SLOT(publishMessage(QString,QByteArray)));
	connect(newipc,SIGNAL(si_subscribeMessage(QString)),this,SLOT(subscribeMessage(QString)));
	connect(newipc,SIGNAL(si_jsonPacketReceived(QJsonObject)),this,SLOT(ipcJsonPacket(QJsonObject)));

}
void Core::ipcJsonPacket(QJsonObject object)
{
	qDebug() << "JSON PACKET:" << QJsonDocument(object).toJson();
	MCIPC *ipcsender = qobject_cast<MCIPC*>(sender());
	if (!ipcsender)
	{
		return;
	}
	if (m_unAuthedConnections.contains(ipcsender))
	{
		//We've got a JSON object from an unauthed sender! verify it
		if (object.contains("type") && object.value("type").toString() == "auth")
		{
			ipcsender->setName(object.value("key").toString());
			m_unAuthedConnections.removeOne(ipcsender);
			m_authedConnections.append(ipcsender);
		}
	}
}
