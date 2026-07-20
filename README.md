# Homework Tracker

## CSCE 3444 – Software Engineering Project

### Team Name
**Taskforce Titans**

### Project Title
**Homework Tracker**

---

## Team Members

- **Team Lead:** Nick Hinojosa
- **Member 1:**  Shamir Warsi   
- **Member 2:**  Tais Loye
- **Member 3:**  Dylan Liles

---

## Project Description

Homework Tracker is a web application designed to help students organize and manage their academic work. Students can create an account, add assignments, edit or delete existing assignments, set due dates, write notes, enable reminders, and track completed work. The system provides a simple and organized interface to help students stay on top of their coursework.

---

## Technologies Used

### Frontend
- HTML5
- CSS3
- JavaScript

### Backend
- C++17
- cpp-httplib (REST API)
- nlohmann/json

### Database
- JSON File Storage (Prototype)
- Firebase or MySQL (Planned for Future Development)

---

## Features

- User Registration
- User Login & Logout
- Create Assignments
- Edit Assignments
- Delete Assignments
- Assignment Notes
- Due Date Tracking
- Reminder Settings
- Mark Assignments as Complete
- Search Assignments
- Filter by Status
- Responsive User Interface

---

## Project Structure

```
HomeworkTracker/
│
├── backend/
│   ├── main.cpp
│   ├── CMakeLists.txt
│   └── data/
│       └── database.json
│
├── frontend/
│   ├── index.html
│   ├── styles.css
│   └── app.js
│
└── README.md
```

---

## Installation

### Requirements

- C++17 Compiler
- CMake 3.20+
- Git

### Build the Backend

```bash
cmake -S . -B build
cmake --build build
```

Run the backend:

```bash
./build/homework_tracker
```

The server will start on:

```
http://localhost:8080
```

Open your browser and navigate to:

```
http://localhost:8080
```

---

## Future Improvements

- Firebase Authentication
- Cloud Database Integration
- Email Notifications
- AI Study Schedule Generator
- Mobile Application
- Calendar Integration

---

## Team Contributions

| Team Member | Responsibilities |
|-------------|------------------|
| Team Lead | Project management, backend development, integration |
| Member 1 | Frontend UI and styling |
| Member 2 | Backend development and API testing |
| Member 3 | Database design and testing |
| Member 4 | Documentation, testing, presentation |

---

## Course Information

**Course:** CSCE 3444 – Software Engineering

**Project:** Homework Tracker

**Team:** Taskforce Titans

---

## License

This project was developed for educational purposes as part of the CSCE 3444 Software Engineering course.