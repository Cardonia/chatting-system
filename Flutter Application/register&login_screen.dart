import 'package:flutter/material.dart';
import 'package:talk_brader/login_screen.dart';
import 'register_screen.dart';

class RegisterLoginScreen extends StatefulWidget {

  @override
  _RegisterLoginScreenState createState() => _RegisterLoginScreenState();

}

class _RegisterLoginScreenState extends State<RegisterLoginScreen> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text("Get Started")),
      body: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
                  Text(
              "Welcome",
              style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
            ),
            SizedBox(height: 20),
            Image.asset(
              "assets/logo.jpg", // put your logo in assets folder
              width: 500,
              height: 200,
            ), //padding
            ElevatedButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => RegisterScreen()),
                );
                print("Button Register pressed");

              },
              child: Text("Register"),

            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => LoginScreen()),
                );
                print("Button Login pressed");

              },
              child: Text("Login"),

            ),
          ],
        ),
      ),
    );
  }
}