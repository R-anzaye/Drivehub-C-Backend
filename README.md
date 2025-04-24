# DriveHub Backend

## Overview
DriveHub Backend is a C-based web server application built using the Mongoose embedded web server library. It provides a RESTful API for managing users, cars, and notifications. The backend uses SQLite as its database and includes features like user authentication with JWT (JSON Web Tokens), password hashing with libsodium, and JSON parsing with Jansson.

The project is designed to interact with a frontend running on `http://localhost:5173` (CORS is configured for this origin).

## Features
- User registration, login, and profile management
- Password and email updates for users
- Car management (add, retrieve, delete cars)
- Notification system (send, retrieve, mark as read)
- Secure password hashing with libsodium
- JWT-based authentication
- SQLite database for persistent storage

## Requirements
To build and run this project, the following dependencies are required:

- GCC (or an equivalent C compiler)
- SQLite3
- Jansson (JSON parser)
- libsodium (for secure password hashing)
- pthread (POSIX threads)
- Make

## Installation Instructions

### Debian/Ubuntu (Linux)
```sh
sudo apt update
sudo apt install build-essential libsqlite3-dev libjansson-dev libsodium-dev
```

### macOS
Install dependencies using Homebrew:
```sh
brew install sqlite jansson libsodium make gcc
```

### Windows
1. **Install MSYS2** from https://www.msys2.org/ and update it:
```sh
pacman -Syu
```
2. Install the required packages:
```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-libsodium mingw-w64-x86_64-jansson make
```
3. Launch the MSYS2 MinGW 64-bit terminal and proceed with build instructions below.

## Installing Mongoose and SQLite3

### Mongoose
Mongoose is included in the repository. No extra installation is required. If needed:
```sh
# Navigate to backend/mongoose and make sure mongoose.c and mongoose.h are present.
```

### SQLite3
If SQLite3 is not installed, refer to platform-specific instructions above or download binaries directly from https://www.sqlite.org/download.html

## Project Structure
```
backend/
├── Makefile                # Build automation
├── README.md               # This file
├── mongoose/               # Mongoose library source
│   └── mongoose.c
│   └── mongoose.h
├── src/                    # Source code
│   ├── app.h               # Header file with shared declarations
│   ├── database.c          # Database logic (SQLite operations)
│   ├── routes.c            # API route handlers
│   └── server.c            # Main server logic
├── setup.sh                # Setup script (if applicable)
└── drivehub.db             # SQLite database file (created on first run)
```

## Setup and Running

1. **Clone the Repository (if applicable):**
```sh
git clone <repository-url>
cd backend
```

2. **Set Environment Variables:**
The server uses a JWT secret for token generation:
```sh
export JWT_SECRET="your-secure-jwt-secret-key-1234567890"
```
If not set, it defaults to a predefined value (not recommended for production).

3. **Build the Project:**
```sh
make
```
Creates an executable named `backend`.

4. **Run the Server:**
```sh
./backend
```
Server will start on: `http://localhost:5555`

5. **Clean Build Artifacts (Optional):**
```sh
make clean
```

## API Endpoints
All responses are in JSON. JWT must be passed in `Authorization: Bearer <token>` header for authenticated endpoints.

### Authentication

#### `POST /register`
Registers a new user.
```json
{
  "first_name": "Vennaliz",
  "last_name": "Wanjiku",
  "email": "wanjikuvennaliz@gmail.com",
  "organization": "Washiiko Enterprise",
  "password": "asecurepassword"
}
```
**Responses:**
- `201 Created`
- `409 Email already in use`
- `400 Missing fields`

#### `POST /login`
Logs in a user.
```json
{
  "email": "wanjikuvennaliz@gmail.com",
  "password": "asecurepassword"
}
```
**Responses:**
- `200 OK` with JWT token
- `401 Invalid credentials`

### User Management

#### `GET /profile`
Retrieve user profile.

#### `PUT /profile`
Update profile.
```json
{
  "first_name": "Vennah",
  "last_name": "Mashuaa",
  "organization": "The Washikooos"
}
```

#### `DELETE /profile`
Delete account.

#### `PUT /password`
Update password.
```json
{
  "password": "anewpassword"
}
```

#### `PUT /email`
Update email.
```json
{
  "email": "vennahmachua@example.com"
}
```

### Car Management

#### `GET /cars`
Retrieve user’s cars.

#### `POST /cars`
Add new car.
```json
{
  "car_name": "koenigsegg jesko",
  "year_of_manufacture": "2023",
  "car_value": "20000",
  "photo": "optional-photo-url"
}
```

#### `DELETE /cars/:id`
Delete a specific car.

### Notifications

#### `POST /notifications`
Send a notification.
```json
{
  "receiver_id": 2,
  "message": "The delivery of the Segera is Today"
}
```

#### `GET /notifications`
Retrieve notifications.

#### `POST /notifications/:id/mark_read`
Mark notification as read.

## Notes
- Server listens on `http://localhost:5555`
- CORS configured for `http://localhost:5173`
- Database file (`drivehub.db`) is created automatically

## Troubleshooting
- **Missing Dependencies:** Ensure all required libraries are installed
- **Port Conflict:** Change port in `server.c` and rebuild
- **JWT Secret:** Always set `JWT_SECRET` for security

## License
MIT License. Mongoose is under its own license.

