#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "mongoose.h"
#include "app.h"

// Executes an SQL query, returns 1 on success, 0 on failure
int execute_sql(const char *sql) {
    char *err_msg = 0;
    int rc = sqlite3_exec(app_ctx.db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }
    return 1;
}

// Hashes a password (placeholder implementation)
void hash_password(const char *password, char *hashed_output) {
    // Placeholder: copy password as-is (replace with real hashing, e.g., bcrypt)
    strncpy(hashed_output, password, 256);
    hashed_output[255] = '\0';
}

// Registers a new user, returns 1 on success, 0 on failure
int register_user(const char *first_name, const char *last_name, const char *email,
                  const char *organization, const char *password) {
    sqlite3_stmt *stmt;
    char hashed_password[256];
    const char *sql = "INSERT INTO users (first_name, last_name, email, organization, password) "
                      "VALUES (?, ?, ?, ?, ?);";

    hash_password(password, hashed_password);

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, organization, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, hashed_password, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    return 1;
}

// Authenticates a user and generates a token, returns 1 on success, 0 on failure
int login_user(const char *email, const char *password, char *token) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, password FROM users WHERE email = ?;";
    char hashed_password[256];

    hash_password(password, hashed_password);

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *stored_password = (const char *)sqlite3_column_text(stmt, 1);
        int user_id = sqlite3_column_int(stmt, 0);
        if (strcmp(stored_password, hashed_password) == 0) {
            // Generate token (placeholder)
            snprintf(token, 256, "token_%d_%ld", user_id, (long)time(NULL));
            sqlite3_finalize(stmt);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Verifies a token (placeholder implementation)
int verify_token(const char *token) {
    // Placeholder: assume token is valid if it starts with "token_"
    return strncmp(token, "token_", 6) == 0;
}

// Retrieves user profile as JSON string, returns 1 on success, 0 on failure
int get_user_profile(int user_id, char *profile) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT first_name, last_name, email, organization FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        snprintf(profile, 1024,
                 "{\"id\": %d, \"first_name\": \"%s\", \"last_name\": \"%s\", \"email\": \"%s\", \"organization\": \"%s\"}",
                 user_id,
                 sqlite3_column_text(stmt, 0),
                 sqlite3_column_text(stmt, 1),
                 sqlite3_column_text(stmt, 2),
                 sqlite3_column_text(stmt, 3));
        sqlite3_finalize(stmt);
        return 1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Handles user profile retrieval (GET /profile)
void handle_profile(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Error response handled in get_user_id_from_token
    }

    char profile[1024];
    if (get_user_profile(user_id, profile)) {
        mg_http_reply(nc, 200, "Content-Type: application/json\r\n", "%s\n", profile);
    } else {
        mg_http_reply(nc, 404, "Content-Type: application/json\r\n", "{\"error\": \"User not found\"}\n");
    }
}