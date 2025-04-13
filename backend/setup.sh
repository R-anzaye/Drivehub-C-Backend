#!/bin/bash

echo "🚀 Setting up project structure..."

# Create necessary directories
mkdir -p instance migrations src frontend

# Create SQLite database if not exists
touch instance/users.db

# Create migration file for user table
cat > migrations/init.sql <<EOL
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    organisation TEXT,
    password TEXT NOT NULL
);
EOL

# Apply the migration
sqlite3 instance/users.db < migrations/init.sql

# Create header file
cat > src/app.h <<EOL
#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <microhttpd.h>
#include <openssl/sha.h>

extern sqlite3 *db;

void init_db();
int register_user(const char *first_name, const char *last_name, const char *email, 
                  const char *organisation, const char *password);
int login_user(const char *email, const char *password);

int request_handler(void *cls, struct MHD_Connection *connection, 
                    const char *url, const char *method, 
                    const char *version, const char *upload_data, 
                    size_t *upload_data_size, void **con_cls);

void hash_password(const char *password, char *hashed);
int execute_sql(const char *sql);

#endif
EOL

# Create empty source files
touch src/app.c src/server.c src/routes.c src/database.c

# Create Makefile for compiling the backend
cat > Makefile <<EOL
CC = gcc
CFLAGS = -Wall -lsqlite3 -lmicrohttpd -lssl -lcrypto
SRCS = src/app.c src/server.c src/routes.c src/database.c
OBJS = \$(SRCS:.c=.o)
TARGET = backend

all: \$(TARGET)

\$(TARGET): \$(OBJS)
	\$(CC) \$(OBJS) -o \$(TARGET) \$(CFLAGS)

clean:
	rm -f \$(OBJS) \$(TARGET)
EOL

echo "✅ Project structure created!"
echo "✅ Database initialized with users table!"
