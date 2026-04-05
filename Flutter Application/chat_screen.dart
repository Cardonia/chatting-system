import 'package:flutter/material.dart';
import 'global.dart';
import 'app_data.dart';


class ChatScreen extends StatefulWidget {
  final int id;
  final String name;

  static final GlobalKey<_ChatScreenState> screenKey = GlobalKey<_ChatScreenState>();

  ChatScreen({
    Key? key,
    required this.id,
    required this.name,
  }) : super(key: key);

  @override
  _ChatScreenState createState() => _ChatScreenState();
}



class _ChatScreenState extends State<ChatScreen> {

  List<Map<String, dynamic>> messages = [];
  TextEditingController controller = TextEditingController();
  

  void requestHistory() async {
    String? token = await AppData.getToken();
    if (token == null) return;

      Map<String, String> data = {
        "event": "GET_HISTORY_MESSAGES",
        "token": token,
        "friendID": widget.id.toString(),
      };
    networkManager.sendData(data);
  }


void handleHistory(Map<String, dynamic> data) {
  if (data["for_friend_id"] != widget.id) return;
  List<Map<String, dynamic>> msgs = List<Map<String, dynamic>>.from(data["messages"]);


  setState(() {
    messages = msgs;
  });
}

  void sendMessage() async {
    String text = controller.text;
    if (text.isEmpty) return;

    controller.clear();

    String? token = await AppData.getToken();
    if (token == null) return;

    // show instantly
    setState(() {
      messages.add({"fromMe": true, "text": text});
    });

    networkManager.sendData({
      "event": "USER_SEND_MESSAGE",
      "friendID": widget.id.toString(),
      "token": token,
      "text": text
    });
  }

  
  @override
  void initState() {
    super.initState();

    requestHistory();
  }

  void handleNewMessage(Map<String, dynamic> data) {
    if (data["fromID"] != widget.id) return;
    setState(() {
      messages.add({
        "fromMe": false,
        "text": data["text"]
      });
    });
  }


  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text(widget.name)),
      body: Column(
        children: [

          Expanded(
            child: ListView.builder(
              itemCount: messages.length,
              itemBuilder: (context, index) {
                bool fromMe = messages[index]["fromMe"];

                return Align(
                  alignment: fromMe
                      ? Alignment.centerRight
                      : Alignment.centerLeft,
                  child: Container(
                    margin: EdgeInsets.symmetric(vertical: 4, horizontal: 8),
                    padding: EdgeInsets.symmetric(horizontal: 14, vertical: 10),
                    constraints: BoxConstraints(maxWidth: 250),
                    decoration: BoxDecoration(
                      color: fromMe ? Colors.blue : Colors.grey[300],
                      borderRadius: BorderRadius.only(
                        topLeft: Radius.circular(16),
                        topRight: Radius.circular(16),
                        bottomLeft: fromMe ? Radius.circular(16) : Radius.circular(0),
                        bottomRight: fromMe ? Radius.circular(0) : Radius.circular(16),
                      ),
                    ),
                    child: Text(
                      messages[index]["text"],
                      style: TextStyle(
                        color: fromMe ? Colors.white : Colors.black,
                      ),
                    ),
                  ),
                );
              },
            ),
          ),

          Container(
  padding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
  decoration: BoxDecoration(
    color: Colors.white,
    boxShadow: [
      BoxShadow(
        color: Colors.black12,
        blurRadius: 5,
      )
    ],
  ),
  child: Row(
    children: [
      Expanded(
        child: Container(
          padding: EdgeInsets.symmetric(horizontal: 12),
          decoration: BoxDecoration(
            color: Colors.grey[200],
            borderRadius: BorderRadius.circular(20),
          ),
          child: TextField(
            controller: controller,
            decoration: InputDecoration(
              hintText: "Type message...",
              border: InputBorder.none,
            ),
          ),
        ),
      ),

      SizedBox(width: 8),

      Container(
        decoration: BoxDecoration(
          color: Colors.blue,
          shape: BoxShape.circle,
        ),
        child: IconButton(
          icon: Icon(Icons.send, color: Colors.white),
          onPressed: sendMessage,
        ),
      ),
    ],
  ),
)
        ],
      ),
    );
  }
}