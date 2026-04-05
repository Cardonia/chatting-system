import 'package:flutter/material.dart';
import 'user_search_screen.dart'; // import your screen
import 'friend_requets_screen.dart';
import 'app_data.dart';
import 'chat_screen.dart';
import 'dart:io';

class MainScreen extends StatefulWidget {
  static GlobalKey<_MainScreenState> screenKey = GlobalKey();
  MainScreen({Key? key}) : super(key: screenKey);

  @override
  _MainScreenState createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {

  List<String> friendNames = [];
  List<int> friendIds = [];

  void handleRequestsList(Map<String, dynamic> data) {
    setState(() {
      friendNames = List<String>.from(data["names"]);
      friendIds = List<int>.from(data["names_id"]);
    });
  }

    @override
  void initState() {
    super.initState();

    friendNames = AppData.friendNames;
    friendIds = AppData.friendIds;
  }


  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: IconButton(
          icon: Icon(Icons.logout),
          onPressed: () async {
            bool? confirm = await showDialog(
              context: context,
              builder: (_) => AlertDialog(
                title: Text("Logout"),
                content: Text("Are you sure you want to log out?"),
                actions: [
                  TextButton(
                    onPressed: () => Navigator.pop(context, false),
                    child: Text("No"),
                  ),
                  TextButton(
                    onPressed: () => Navigator.pop(context, true),
                    child: Text("Yes"),
                  ),
                ],
              ),
            );

            if (confirm == true) {
              await AppData.deleteToken();
               exit(0);
            }
          },
        ),
        title: Text("welcome"),
        actions: [
          IconButton(
            icon: Icon(Icons.search),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (_) => UserSearchScreen()),
              );
            },
          ),
          IconButton(
            icon: Icon(Icons.person_add),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (_) => FriendRequetsScreen()),
              );
            },
          ),
        ],
      ),
      body: ListView.builder(
  itemCount: friendNames.length,
  itemBuilder: (context, index) {
   return Padding(
    padding: EdgeInsets.symmetric(horizontal: 10, vertical: 6),
    child: InkWell(
      borderRadius: BorderRadius.circular(15),
      onTap: () {
        Navigator.push(
          context,
          MaterialPageRoute(
            builder: (_) => ChatScreen(
              key: ChatScreen.screenKey,
              id: friendIds[index],
              name: friendNames[index],
            ),
          ),
        );
      },
      child: Container(
        padding: EdgeInsets.all(12),
        decoration: BoxDecoration(
          color: Colors.white,
          borderRadius: BorderRadius.circular(15),
          boxShadow: [
            BoxShadow(
              color: Colors.black12,
              blurRadius: 4,
            )
          ],
        ),
        child: Row(
          children: [
            CircleAvatar(
              radius: 22,
              child: Text(friendNames[index][0].toUpperCase()),
            ),

            SizedBox(width: 12),

            Expanded(
              child: Text(
                friendNames[index],
                style: TextStyle(
                  fontSize: 16,
                  fontWeight: FontWeight.bold,
                ),
              ),
            ),

            Icon(Icons.arrow_forward_ios, size: 16)
          ],
        ),
      ),
    ),
  );
  },
),
    );
  }
}