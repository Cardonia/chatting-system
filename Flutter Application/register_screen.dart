import 'package:flutter/material.dart';
import 'package:talk_brader/socket_manager.dart';
import 'global.dart';


class RegisterScreen extends StatefulWidget {

  static final GlobalKey<_RegisterScreenState> registerKey = GlobalKey<_RegisterScreenState>();

  RegisterScreen({Key? key}) : super(key: registerKey);

  @override
  _RegisterScreenState createState() => _RegisterScreenState();

}
class _RegisterScreenState extends State<RegisterScreen> {
  final usernameController = TextEditingController();
  final passwordController = TextEditingController();
  final rePasswordController = TextEditingController();

  String errorText = ""; // store error message
   void setErrorText(String text) {
    setState(() {
      errorText = text;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Register")),
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            SizedBox(
              width: 250,
              child: TextField(
                controller: usernameController,
                decoration: InputDecoration(labelText: "Username"),
              ),
            ),
            SizedBox(height: 10),
            SizedBox(
              width: 250,
              child: TextField(
                controller: passwordController,
                obscureText: true,
                decoration: InputDecoration(labelText: "Password"),
              ),
            ),
            SizedBox(height: 10),
            SizedBox(
              width: 250,
              child: TextField(
                controller: rePasswordController,
                obscureText: true,
                decoration: InputDecoration(labelText: "Re-enter Password"),
              ),
            ),
            SizedBox(height: 20),
            SizedBox(
              width: 150,
              child: ElevatedButton(
                onPressed: () {
                  setState(() {
                    if (passwordController.text.length < 8) {
                      errorText = "Password must be at least 8 characters";
                    } else if (passwordController.text != rePasswordController.text) {
                      errorText = "Passwords don't match";
                    } else {
                      errorText = "";

                      // create data map
                      Map<String, String> data = {
                        "event": "REGISTER",
                        "username": usernameController.text,
                        "password": passwordController.text,
                      };
                      // send to network
                      networkManager.sendData(data);
                    
                      
                    }
                  });
                },
                child: Text("Register"),
              ),
            ),
            SizedBox(height: 10),
            Text(
              errorText,
              style: TextStyle(color: Colors.red),
            ),
          ],
        ),
      ),
    );
  }
}