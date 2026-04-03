import 'package:talk_brader/socket_manager.dart';
import 'package:flutter/material.dart';
import 'startup_screen.dart'; 

void main() async{
  WidgetsFlutterBinding.ensureInitialized();//wait for flutter to setup fully

  SocketManager networkManager= new SocketManager();
  bool connected = await networkManager.connectToServer();

  var data = {"event":"TOKEN_CHECK","token":"12345678"};
  networkManager.sendData(data);
  runApp(MyApp(connected: connected));

}

class MyApp extends StatelessWidget {
  final bool connected;
  MyApp({required this.connected});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: StartupScreen(connected: connected),
    );
  }
}