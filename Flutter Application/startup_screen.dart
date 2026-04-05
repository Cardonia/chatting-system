import 'package:flutter/material.dart';
import 'app_data.dart';
import 'register&login_screen.dart';
import 'global.dart';
import 'main_screen.dart';
class StartupScreen extends StatefulWidget { 
  StartupScreen({Key? key}) : super(key: startupKey);

  static GlobalKey<_StartupScreenState> startupKey = GlobalKey();

  @override
  _StartupScreenState createState() => _StartupScreenState();

}

class _StartupScreenState extends State<StartupScreen>{//screen state
  @override
  void initState() {
    super.initState();
    checkToken();
  }
void goToMain() {
  Navigator.pushReplacement(
    context,
    MaterialPageRoute(builder: (_) => MainScreen()),
  );
}

void openRegisterLogin() {
  Navigator.pushReplacement(
    context,
    MaterialPageRoute(builder: (_) => RegisterLoginScreen()),
  );
}
  void checkToken() async {
    String? token = await AppData.getToken();

    if (token == null) {
      openRegisterLogin();
      return;
    }

    // send token to server
    Map<String, String> data = {
      "event": "TOKEN_CHECK",
      "token": token,
    };
    networkManager.sendData(data);
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
