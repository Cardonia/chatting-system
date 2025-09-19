# Chatting System

This is a **Qt GUI Chatting App** with a **C++ server using SQLite**.  
The app requires the server to run.

## Features

- GUI for chatting with users
- Server handles messages and stores data in SQLite
- Works locally or on a network

## Setup

1. **Server**  
   - Open `Server with Sqlite` in your C++ IDE (like Visual Studio).  
   - Build and run the server first.

2. **GUI App**  
   - Open `QtGUIApp` in Qt Creator.  
   - Run the app after the server is running.

## Notes

- Make sure the server is running before using the GUI app.  

## GitHub

- Project is in a single repo with subfolders for GUI and server.  
- Push only source files, `.gitignore` prevents pushing build files.
