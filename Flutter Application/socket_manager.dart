import 'dart:io';
import 'dart:convert';
import 'package:talk_brader/app_data.dart';
import 'package:talk_brader/handle_server_messages.dart';

class SocketManager{
    bool isConnected = false;
    Socket? socket;
    List<int> recieveBuffer = [];

    HandleServerMessages handleMessage = new HandleServerMessages(); 


    Future<bool> connectToServer() async { //async means fucntion can wait without freezing program
        try {
            socket = await Socket.connect("10.59.90.76", 5000).timeout(Duration(seconds: 3));
            startListeningFromServer();
            print("Connection Success");
            isConnected = true;
            AppData.connected = true;
            return true;
        } catch (e) {
            print("Connection Failed");
            AppData.connected = false;
            return false;
        }
    }

    void sendData(Map<String, String> data){
        String jsonString = jsonEncode(data);
        List<int> bytesList = utf8.encode(jsonString);

        int size = bytesList.length;
        
        List<int> fullMessageByte = [
            (size >> 24) & 0xFF,
            (size >> 16) & 0xFF,
            (size >> 8) & 0xFF,
            size & 0xFF,
        ];

        fullMessageByte.addAll(bytesList);

        socket?.add(fullMessageByte);
        socket?.flush(); 
    }

    void startListeningFromServer() {
      socket?.listen((data) {
        recieveBuffer.addAll(data);
        print("data recieved");
        handleRecieveBuffer();
      });
    }

  
    void handleRecieveBuffer(){
    while (true) {

      if (recieveBuffer.length < 4) return;

      int size = recieveBuffer[0]*256*256*256
        + recieveBuffer[1]*256*256
        + recieveBuffer[2]*256
        + recieveBuffer[3];

      if (recieveBuffer.length < 4 + size) return;

      List<int> messageBytes = recieveBuffer.sublist(4, 4 + size);

      recieveBuffer = recieveBuffer.sublist(4 + size);

      handleMessage.proccess(messageBytes);
    }
  }

  void disconnect(){
        socket?.destroy();
        isConnected = false;
        print("Disconnected");
    }    

}
