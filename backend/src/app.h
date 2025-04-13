
#ifndef APP_H
#define APP_H

#include <sqlite3.h>
#include "mongoose.h"

// Application context to hold shared state
struct app_context {
    sqlite3 *db;  // Database handle
};

// Global application context
extern struct app_context app_ctx;

// Utility functions
// Hashes a password and stores result in hashed_output
void hash_password(const char *password, char *hashed_output);

// Executes an SQL query, returns 1 on success, 0 on failure
int execute_sql(const char *sql);

// User management functions
// Registers a new user, returns 1 on success, 0 on failure
int register_user(const char *first_name, const char *last_name, const char *email,
                  const char *organization, const char *password);

// Authenticates a user and generates a token, returns 1 on success, 0 on failure
int login_user(const char *email, const char *password, char *token);

// Verifies a token, returns 1 if valid, 0 if invalid
int verify_token(const char *token);

// Retrieves user profile as JSON string, returns 1 on success, 0 on failure
int get_user_profile(int user_id, char *profile);

// Extracts user ID from Authorization header, returns ID or 0 on failure
int get_user_id_from_token(struct mg_connection *nc, struct mg_http_message *hm);

// Route handlers
// Handles user registration (POST /register)
void handle_register(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);

// Handles user login (POST /login)
void handle_login(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);

// Handles user profile retrieval (GET /profile)
void handle_profile(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);

// Handles CORS preflight requests (OPTIONS)
void handle_options(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);

#endif