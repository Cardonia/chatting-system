import 'package:talk_brader/socket_manager.dart';
import 'package:flutter/material.dart';
import 'startup_screen.dart'; 
import 'global.dart';

void main() async{
  WidgetsFlutterBinding.ensureInitialized();//wait for flutter to setup fully

 
  await networkManager.connectToServer();

  runApp(MyApp());

}

class MyApp extends StatelessWidget {

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: StartupScreen(),
    );
  }
}