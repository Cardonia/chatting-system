import 'package:flutter/material.dart';
import 'app_data.dart';
import 'register&login_screen.dart';
class StartupScreen extends StatefulWidget { 
  
  @override
  _StartupScreenState createState() => _StartupScreenState();

}

class _StartupScreenState extends State<StartupScreen> {//screen state
  
  @override
  void initState() {
    super.initState();
    // wait 2 seconds and check connection
    if (AppData.connected == true) {
      Future.delayed(Duration(seconds: 0), () {
        Navigator.pushReplacement(
          context,
          MaterialPageRoute(builder: (_) => RegisterLoginScreen()),
        );
      });
    }
  }


  String getStatusText() {
  if (AppData.connected == null) return "Checking...";
  if (AppData.connected == true) return "Connection Success";
  return "Connection Failed";
}

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            FlutterLogo(size: 100), 
            SizedBox(height: 20),
            Text(getStatusText()), 
          ],
        ),
      ),
    );
  }
}