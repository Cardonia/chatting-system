
import 'dart:convert';
import 'package:talk_brader/login_screen.dart';

import 'register_screen.dart';

class HandleServerMessages {
   late final Map<String, Function()> eventActions;

HandleServerMessages() {
    eventActions = {
    "REGISTER_USER_EXIST": setRegisterScreenText,
    "LOGIN_FAILED":  setLoginScreenText
    };
}
    /*
    ,
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
  };*/


  void proccess(List<int> messageBytes){

    String jsonString = utf8.decode(messageBytes);

    Map<String, dynamic> jsonObject = jsonDecode(jsonString);
    String event = jsonObject["event"];
    eventActions[event]?.call() ?? unknownEvent();
    print(jsonObject['event']); 

  }

  void unknownEvent(){
      print("unknown event");
  }

  void setRegisterScreenText() {
    RegisterScreen.registerKey.currentState?.setErrorText("User already registered");
  }

  void setLoginScreenText(){
     LoginScreen.registerKey.currentState?.setErrorText("Username or password is incorrect");
  }

}