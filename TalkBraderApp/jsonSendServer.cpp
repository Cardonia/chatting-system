#include "jsonSendServer.h"
#include "socketManager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QPushButton>
#include <QJsonArray>

//main function to send data to server.
void jsonSend(QJsonObject& json) {
    //json variable holds the object for example
    /*
     {
       "event" : "LOGIN",
       "username" : "sam",
       "password" : "1234"
      }
     */
    QJsonDocument doc(json);
    // create an obj named doc from QJsonDocument class take json object as input
    // doc is a containor that wraps structured JSON data. either for converting or parsing

    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //convert the doc(containor of json obj) to raw bytes
    //doc.toJson();  convert to bytes , QJsonDocument::Compact mean remove spaces and breaks
    //data become like this {"event":"LOGIN","username":"sam","password":"1234"}
    //store in jsonData variable

    QByteArray packet;
    QDataStream stream(&packet, QIODevice::WriteOnly);
    //its like a rule. Any data you write using stream << someValue; will be encoded in binary and stored in packet.

    stream.setVersion(QDataStream::Qt_6_0);
    //make sure both side use same version

    quint32 length = jsonData.size();
    //get jsonData size in 4 bytes

    stream << length;
    //convert length into bytes then save in packet

    packet.append(jsonData);
    //put jsonData into the packet

    socketManager::socketPtr->write(packet);
    socketManager::socketPtr->flush();
}
