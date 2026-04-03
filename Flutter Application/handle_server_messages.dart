
import 'dart:convert';

class HandleServerMessages {
   late final Map<String, Function()> eventActions;

  HandleServerMessages() {
    eventActions = {
      "TOKEN_INVALID": test,
    };
  }
/*
  final eventActions = const {
    "REGISTER_USER_EXIST": ,
    "REGISTER_SUCCESS": ,
    "TOKEN_VALID": ,
    "TOKEN_INVALID": ,
    "LOGIN_FAILED": ,
    "LOGIN_SUCCESS": ,
    "SEARCH_FRIEND_RESULT": ,
    "FRIEND_REQUEST_PENDING_LIST": ,
    "ALL_FRIENDS_LIST": ,
    "GOT_HISTORY_MESSAGES": ,
    "YOU_GOT_MESSAGE": ,
  };
  */

  void proccess(List<int> messageBytes){

    String jsonString = utf8.decode(messageBytes);

    Map<String, dynamic> jsonObject = jsonDecode(jsonString);
    String event = jsonObject["event"];
    eventActions[event]?.call() ?? unknown();
    print(jsonObject['event']); 

  }

  void unknown(){

  }

  void test(){
    print("#################");
  }
}