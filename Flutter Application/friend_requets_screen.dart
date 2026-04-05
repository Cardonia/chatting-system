import 'package:flutter/material.dart';
import 'global.dart';
import 'app_data.dart';

class FriendRequetsScreen extends StatefulWidget {
  
   static GlobalKey<_FriendRequetsScreenState> screenKey = GlobalKey();
FriendRequetsScreen({Key? key}) : super(key: screenKey);
  @override
  _FriendRequetsScreenState createState() => _FriendRequetsScreenState();
}

class _FriendRequetsScreenState extends State<FriendRequetsScreen> {
  TextEditingController searchController = TextEditingController();
  String infoText = "";
  
  
  static List<String> resultNames = [];
  static List<int> resultIds = [];

  void handleRequestsList(Map<String, dynamic> data) {
    setState(() {
      resultNames = List<String>.from(data["names"]);
      resultIds = List<int>.from(data["names_id"]);
    });
  }

void onUserClick(int id, String name) async {
  String? token = await AppData.getToken();
  if (token == null) return;


  int index = resultIds.indexOf(id);
  if (index != -1) {
    setState(() {
      resultIds.removeAt(index);
      resultNames.removeAt(index);
    });
  }

  Map<String, String> data = {
    "event": "ACCEPT_FRIEND_REQUEST",
    "token": token,
    "toId": id.toString()
  };
  networkManager.sendData(data);

  ScaffoldMessenger.of(context)
    ..hideCurrentSnackBar()
    ..showSnackBar(
      SnackBar(
        content: Text("$name accepted"),
      ),
    );
}

  @override
  void initState() {
    super.initState();

    resultNames = AppData.pendingNames;
    resultIds = AppData.pendingIds;
  }




  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Accept Friend Request"),
        leading: IconButton(
          icon: Icon(Icons.arrow_back),
          onPressed: () {
            Navigator.pop(context);
          },
        ),
      ),
      body: Column(
        children: [
          Padding(
            padding: EdgeInsets.all(12),
            child: Row(
              children: [
                SizedBox(width: 8),
              ],
            ),
          ),
          if (infoText.isNotEmpty)
            Padding(
              padding: EdgeInsets.all(8),
              child: Text(infoText, style: TextStyle(color: const Color.fromARGB(255, 33, 107, 35))),
          ),
          Expanded(
            child: ListView.builder(
              itemCount: resultNames.length,
              itemBuilder: (context, index) {
                return Padding(
                  padding: EdgeInsets.symmetric(horizontal: 10, vertical: 6),
                  child: InkWell(
                    borderRadius: BorderRadius.circular(15),
                    onTap: () => onUserClick(resultIds[index], resultNames[index]),
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
                            child: Text(resultNames[index][0].toUpperCase()),
                          ),
                          SizedBox(width: 12),
                          Expanded(
                            child: Text(
                              resultNames[index],
                              style: TextStyle(
                                fontSize: 16,
                                fontWeight: FontWeight.bold,
                              ),
                            ),
                          ),
                          Icon(Icons.check, color: Colors.green),
                        ],
                      ),
                    ),
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}


