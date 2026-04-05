import 'package:flutter_secure_storage/flutter_secure_storage.dart';

class AppData {
  static bool? connected;
  static bool? tokenIsValid;
  static final storage = FlutterSecureStorage();

  static List<String> pendingNames = [];
  static List<int> pendingIds = []; 

  static List<String> friendNames = [];
  static List<int> friendIds = []; 

  static Future<void> saveToken(String token) async {
    await storage.write(key: "token", value: token);
    print("token saved: "+token);
  }

  static Future<String?> getToken() async {
    return await storage.read(key: "token");
  }

  static Future<void> deleteToken() async {
  await storage.delete(key: "token");
  print("token deleted");
}
}