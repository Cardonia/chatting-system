import 'package:flutter/material.dart';

class StartupScreen extends StatefulWidget { 
  final bool connected;
  StartupScreen({required this.connected});
  //screen widget 
  @override
  _StartupScreenState createState() => _StartupScreenState();
}

class _StartupScreenState extends State<StartupScreen> {//screen state
  late String status;

  @override
  void initState() {
    super.initState();
    status = widget.connected ? "Connection Success" : "Connection Failed";
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
            Text(status), 
          ],
        ),
      ),
    );
  }
}