import 'package:flutter/material.dart';
import 'global.dart';
import 'app_data.dart';
class UserSearchScreen extends StatefulWidget {
  
   static GlobalKey<_UserSearchScreenState> screenKey = GlobalKey();
UserSearchScreen({Key? key}) : super(key: screenKey);
  @override
  _UserSearchScreenState createState() => _UserSearchScreenState();
}

class _UserSearchScreenState extends State<UserSearchScreen> {
  TextEditingController searchController = TextEditingController();
  String infoText = "";
  List<String> resultNames = [];
  List<int> resultIds = [];

  // This will be called when user clicks a result
  void onUserClick(int id, String name) async {
    String? token = await AppData.getToken();

    if (token == null) return;

     Map<String, String> data = {
        "event": "SEND_FRIEND_REQUEST",
        "token": token,
        "toId": id.toString()
      };
      networkManager.sendData(data);


     ScaffoldMessenger.of(context)
      ..hideCurrentSnackBar()
      ..showSnackBar(
        SnackBar(
          content: Text("Friend request sent to $name"),
          duration: Duration(seconds: 3),
          behavior: SnackBarBehavior.floating,
          margin: EdgeInsets.fromLTRB(10, 10, 10, 0),
        ),
      );
      Future.delayed(Duration(seconds: 2), () {
        setState(() {
          infoText = "";
        });
      });

    print("Clicked user: $name, id: $id");
  }

  // This function will be called from outside when server sends search results
  void handleSearchResult(Map<String, dynamic> data) {
    if (data["event"] == "SEARCH_FRIEND_RESULT") {
      setState(() {
        resultNames = List<String>.from(data["names"]);
        resultIds = List<int>.from(data["names_id"]);
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Search Users"),
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
                
                Expanded(
                  child: TextField(
                    controller: searchController,
                    decoration: InputDecoration(
                      hintText: "Enter username",
                      border: OutlineInputBorder(),
                    ),
                  ),
                ),
                SizedBox(width: 8),
                ElevatedButton(
                  onPressed: () {
                    if(searchController.text.isEmpty)return;
                    Map<String, String> data = {
                      "event": "SEARCH_FRIEND",
                      "name": searchController.text,
                    };
                  
                    networkManager.sendData(data);
                  },
                  child: Text("Search"),
                ),
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
                          Icon(Icons.person_add),
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