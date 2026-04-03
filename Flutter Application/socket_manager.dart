import 'dart:io';
import 'dart:convert';
import 'package:talk_brader/handle_server_messages.dart';

class SocketManager{
    bool isConnected = false;
    Socket? socket;
    List<int> recieveBuffer = [];

    HandleServerMessages handleMessage = new HandleServerMessages(); 


    Future<bool> connectToServer() async { //async means fucntion can wait without freezing program
        try {
            socket = await Socket.connect("127.0.0.1", 5000).timeout(Duration(seconds: 3));
            startListeningFromServer();
            print("Connection Success");
            isConnected = true;
            return true;
        } catch (e) {
            print("Connection Failed");
            return false;
        }
    }

    void sendData(var data){
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
        print(fullMessageByte);

        socket?.add(fullMessageByte);

    }

    void startListeningFromServer() {
      socket?.listen((data) {
        recieveBuffer.addAll(data);
        print("data recieved");
        handleRecieveBuffer();
      });
    }

    void disconnect(){
        socket?.destroy();
        isConnected = false;
        print("Disconnected");
        handleRecieveBuffer();
    }    

    void handleRecieveBuffer(){

        if(recieveBuffer.length<4) return;

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
