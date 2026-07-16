# Homework Tracker Full Program

This project contains:

- HTML frontend
- CSS styling
- JavaScript interactions
- C++ REST API backend
- JSON file storage

## 1. Build the C++ backend
## Windows PowerShell

```powershell
cd backend
cmake -S . -B build
cmake --build build --config Release
.\build\Release\homework_tracker_backend.exe
```

Depending on the CMake generator, the executable may be:

```powershell
.\build\homework_tracker_backend.exe
```

### macOS/Linux

```bash
cd backend
cmake -S . -B build
cmake --build build
./build/homework_tracker_backend
```

The backend runs on:

```text
http://localhost:8080
```

## 2. Open the frontend

Open:

```text
frontend/index.html
```

in Chrome or Edge.


## Completed features

- Login and logout
- Add assignments
- Edit assignments
- Delete assignments
- Add notes
- Due dates
- Reminder settings
- Mark assignments complete
- Filter active/completed assignments
- JSON data persistence
- Authenticated API routes
