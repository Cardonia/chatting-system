
import 'dart:convert';
import 'package:talk_brader/login_screen.dart';

import 'register_screen.dart';
import 'app_data.dart';
import 'package:flutter/material.dart';
import 'main_screen.dart';
import 'startup_screen.dart';
import 'user_search_screen.dart';
import 'friend_requets_screen.dart';
import 'chat_screen.dart';

class HandleServerMessages {
   late final Map<String, Function(Map<String, dynamic>)> eventActions;

HandleServerMessages() {
    eventActions = {
    "REGISTER_USER_EXIST": setRegisterScreenText,
    "LOGIN_FAILED":  setLoginScreenText,
    "REGISTER_SUCCESS": (data) => registerSuccess(RegisterScreen.registerKey.currentContext!, data),
    "LOGIN_SUCCESS": (data) => loginSuccess(LoginScreen.loginKey.currentContext!, data),
    "TOKEN_VALID": tokenValid,
    "TOKEN_INVALID": tokenInvalid,
    "SEARCH_FRIEND_RESULT": searchFriendResult,
    "FRIEND_REQUEST_PENDING_LIST": friendRequestPendingList,
    "ALL_FRIENDS_LIST": allFriendsList,
    "GOT_HISTORY_MESSAGES": handleHistory,
    "YOU_GOT_MESSAGE": handleNewMessage,
    };
}
    /*
  
   
    
  };*/


  void proccess(List<int> messageBytes){

    String jsonString = utf8.decode(messageBytes);

    Map<String, dynamic> jsonObject = jsonDecode(jsonString);
    String event = jsonObject["event"];
    eventActions[event]?.call(jsonObject) ?? unknownEvent();
    print(jsonObject['event']); 

  }

  void unknownEvent(){
      print("unknown event");
  }

  void setRegisterScreenText(Map<String, dynamic> _) {
    RegisterScreen.registerKey.currentState?.setErrorText("User already registered");
  }

  void setLoginScreenText(Map<String, dynamic> _){
     LoginScreen.loginKey.currentState?.setErrorText("Username or password is incorrect");
  }

 void registerSuccess(BuildContext context, Map<String, dynamic> jsonObject) {
  AppData.saveToken(jsonObject['token']);

  // Navigate and replace register screen with MainScreen
  Navigator.pushReplacement(
    context,
    MaterialPageRoute(builder: (_) => MainScreen()),
  );
}

  void loginSuccess(BuildContext context, Map<String, dynamic> jsonObject){
     AppData.saveToken(jsonObject['token']);

    Navigator.pushReplacement(
      context,
      MaterialPageRoute(builder: (_) => MainScreen()),
    );
  }

  void tokenValid(Map<String, dynamic> _) {
  AppData.tokenIsValid = true;
  StartupScreen.startupKey.currentState?.goToMain();
}

void tokenInvalid(Map<String, dynamic> _) {
  StartupScreen.startupKey.currentState?.openRegisterLogin();
}

void searchFriendResult(Map<String, dynamic> jsonObject){
  UserSearchScreen.screenKey.currentState?.handleSearchResult(jsonObject);
}

void friendRequestPendingList(Map<String, dynamic> jsonObject){
  AppData.pendingNames = List<String>.from(jsonObject["names"]);
  AppData.pendingIds = List<int>.from(jsonObject["names_id"]);

  FriendRequetsScreen.screenKey.currentState?.handleRequestsList(jsonObject);
}


void allFriendsList(Map<String, dynamic> jsonObject){
  AppData.friendNames = List<String>.from(jsonObject["names"]);
  AppData.friendIds = List<int>.from(jsonObject["names_id"]);
  MainScreen.screenKey.currentState?.handleRequestsList(jsonObject);
}

void handleHistory(Map<String, dynamic> data) {

  print("/////////////called hande new messages in th nanlde server ");
  ChatScreen.screenKey.currentState?.handleHistory(data);
}

void handleNewMessage(Map<String, dynamic> data) {
  ChatScreen.screenKey.currentState?.handleNewMessage(data);
}

}