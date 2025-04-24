#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <jwt.h>
#include <sodium.h>
#include "app.h"

// Initialize sodium
__attribute__((constructor))
void init_sodium() {
    if (sodium_init() < 0) {
        fprintf(stderr, "Error initializing libsodium\n");
        exit(1);
    }
}

// Basic JSON escaping
static char *json_escape(const char *str) {
    if (!str) return strdup("");
    size_t len = strlen(str);
    char *out = malloc(len * 2 + 1);
    if (!out) return strdup("");
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '"' || str[i] == '\\') {
            out[j++] = '\\';
        }
        out[j++] = str[i];
    }
    out[j] = '\0';
    return out;
}

// Database initialization
int init_db(sqlite3 *db) {
    const char *user_sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE,"
        "organization TEXT NOT NULL,"
        "password TEXT NOT NULL"
        ");";

    const char *cars_sql = 
        "CREATE TABLE IF NOT EXISTS cars ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "car_name TEXT NOT NULL,"
        "year_of_manufacture TEXT NOT NULL,"
        "car_value TEXT NOT NULL,"
        "photo TEXT,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ");";

    const char *notifications_sql = 
        "CREATE TABLE IF NOT EXISTS notifications ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sender_id INTEGER NOT NULL,"
        "receiver_id INTEGER NOT NULL,"
        "message TEXT NOT NULL,"
        "timestamp INTEGER NOT NULL,"
        "is_read INTEGER NOT NULL DEFAULT 0,"
        "FOREIGN KEY (sender_id) REFERENCES users(id),"
        "FOREIGN KEY (receiver_id) REFERENCES users(id)"
        ");";

    if (sqlite3_exec(db, user_sql, 0, 0, 0) != SQLITE_OK ||
        sqlite3_exec(db, cars_sql, 0, 0, 0) != SQLITE_OK ||
        sqlite3_exec(db, notifications_sql, 0, 0, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to create tables: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    return 1;
}

// Closes the database
void close_db(sqlite3 *db) {
    sqlite3_close(db);
}

// Hash password using libsodium
void hash_password(const char *password, char *hashed_output) {
    if (crypto_pwhash_str(hashed_output, password, strlen(password),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE,
                          crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        fprintf(stderr, "Error hashing password\n");
        strcpy(hashed_output, "");
    }
}

// Registers a new user
int register_user(const char *first_name, const char *last_name, const char *email, const char *organization, const char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (first_name, last_name, email, organization, password) VALUES (?, ?, ?, ?, ?);";
    int rc;

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    char hashed_password[crypto_pwhash_STRBYTES];
    hash_password(password, hashed_password);
    if (strlen(hashed_password) == 0) {
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, organization, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, hashed_password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return rc == SQLITE_CONSTRAINT ? -1 : 0; // -1 for UNIQUE constraint violation
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Authenticates a user and generates a JWT
int login_user(const char *email, const char *password, char *token) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, password FROM users WHERE email = ?;";
    int user_id = 0;
    char stored_password[crypto_pwhash_STRBYTES];

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
        strncpy(stored_password, (const char *)sqlite3_column_text(stmt, 1), sizeof(stored_password) - 1);
        stored_password[sizeof(stored_password) - 1] = '\0';
    }
    sqlite3_finalize(stmt);

    if (user_id <= 0) {
        fprintf(stderr, "User not found for email: %s\n", email);
        return 0; // User not found
    }

    // Verify password with libsodium
    if (crypto_pwhash_str_verify(stored_password, password, strlen(password)) != 0) {
        fprintf(stderr, "Invalid password for user_id: %d\n", user_id);
        return 0; // Invalid password
    }

    // Generate JWT
    jwt_t *jwt = NULL;
    if (jwt_new(&jwt) != 0) {
        fprintf(stderr, "Error creating JWT\n");
        return 0;
    }

    time_t now = time(NULL);
    char user_id_str[16];
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    jwt_add_grant_int(jwt, "iat", now);
    jwt_add_grant_int(jwt, "exp", now + 3600); // 1 hour expiration
    jwt_add_grant(jwt, "sub", user_id_str);

    jwt_set_alg(jwt, JWT_ALG_HS256, (unsigned char *)app_ctx.jwt_secret, strlen(app_ctx.jwt_secret));

    char *jwt_str = jwt_encode_str(jwt);
    if (!jwt_str) {
        fprintf(stderr, "Error encoding JWT\n");
        jwt_free(jwt);
        return 0;
    }

    strncpy(token, jwt_str, 512);
    token[511] = '\0'; // Ensure null-termination
    jwt_free_str(jwt_str);
    jwt_free(jwt);
    return 1;
}

// Verifies a JWT and returns user_id
int verify_token(const char *token) {
    jwt_t *jwt = NULL;

    if (jwt_decode(&jwt, token, (unsigned char *)app_ctx.jwt_secret, strlen(app_ctx.jwt_secret)) != 0) {
        fprintf(stderr, "Error decoding JWT\n");
        return 0;
    }

    time_t now = time(NULL);
    long exp = jwt_get_grant_int(jwt, "exp");
    if (exp < now) {
        fprintf(stderr, "Token expired at %ld, current time: %ld\n", exp, now);
        jwt_free(jwt);
        return 0;
    }

    const char *sub = jwt_get_grant(jwt, "sub");
    int user_id = sub ? atoi(sub) : 0;

    jwt_free(jwt);
    if (user_id <= 0) {
        fprintf(stderr, "Invalid user_id in token\n");
        return 0;
    }
    fprintf(stderr, "Token verified, user_id: %d\n", user_id);
    return user_id;
}

// Retrieves user profile
int get_user_profile(int user_id, char *profile) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT first_name, last_name, email, organization FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        char *first_name = json_escape((const char *)sqlite3_column_text(stmt, 0));
        char *last_name = json_escape((const char *)sqlite3_column_text(stmt, 1));
        char *email = json_escape((const char *)sqlite3_column_text(stmt, 2));
        char *organization = json_escape((const char *)sqlite3_column_text(stmt, 3));
        snprintf(profile, 2048,
                 "{\"id\": %d, \"first_name\": \"%s\", \"last_name\": \"%s\", \"email\": \"%s\", \"organization\": \"%s\"}",
                 user_id, first_name, last_name, email, organization);
        free(first_name);
        free(last_name);
        free(email);
        free(organization);
        sqlite3_finalize(stmt);
        fprintf(stderr, "Profile fetched for user_id: %d\n", user_id);
        return 1;
    }

    fprintf(stderr, "User not found for user_id: %d\n", user_id);
    sqlite3_finalize(stmt);
    return 0;
}

// Updates user profile
int update_user_profile(int user_id, const char *first_name, const char *last_name, const char *organization) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET first_name = ?, last_name = ?, organization = ? WHERE id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, organization, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, user_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Updates user password
int update_user_password(int user_id, const char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET password = ? WHERE id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    char hashed_password[crypto_pwhash_STRBYTES];
    hash_password(password, hashed_password);
    if (strlen(hashed_password) == 0) {
        sqlite3_finalize(stmt);
        return 0;
    }
    sqlite3_bind_text(stmt, 1, hashed_password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Updates user email
int update_user_email(int user_id, const char *email) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE users SET email = ? WHERE id = ?;";
    int rc;

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, user_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return rc == SQLITE_CONSTRAINT ? -1 : 0; // -1 for UNIQUE constraint violation
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Deletes a user
int delete_user(int user_id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM users WHERE id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Adds a car
int add_car(int user_id, const char *car_name, const char *year_of_manufacture, const char *car_value, const char *photo) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO cars (user_id, car_name, year_of_manufacture, car_value, photo) VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, car_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, year_of_manufacture, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, car_value, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, photo, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Retrieves cars for a user
int get_cars(int user_id, char *cars_json) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, car_name, year_of_manufacture, car_value, photo FROM cars WHERE user_id = ?;";
    char temp[8192] = "[";
    int first = 1;

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char *car_name = json_escape((const char *)sqlite3_column_text(stmt, 1));
        char *year = json_escape((const char *)sqlite3_column_text(stmt, 2));
        char *value = json_escape((const char *)sqlite3_column_text(stmt, 3));
        char *photo = json_escape((const char *)sqlite3_column_text(stmt, 4));
        char entry[1024];
        snprintf(entry, sizeof(entry),
                 "%s{\"id\": %d, \"car_name\": \"%s\", \"year_of_manufacture\": \"%s\", \"car_value\": \"%s\", \"photo\": \"%s\"}",
                 first ? "" : ",", sqlite3_column_int(stmt, 0), car_name, year, value, photo ? photo : "");
        free(car_name);
        free(year);
        free(value);
        free(photo);
        strncat(temp, entry, sizeof(temp) - strlen(temp) - 1);
        first = 0;
    }

    sqlite3_finalize(stmt);
    strncat(temp, "]", sizeof(temp) - strlen(temp) - 1);
    strncpy(cars_json, temp, 8192);
    return 1;
}

// Deletes a car
int delete_car(int user_id, int car_id) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM cars WHERE id = ? AND user_id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, car_id);
    sqlite3_bind_int(stmt, 2, user_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    int changes = sqlite3_changes(app_ctx.db);
    sqlite3_finalize(stmt);
    return changes > 0;
}

// Sends a notification
int send_notification(int sender_id, int receiver_id, const char *message) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO notifications (sender_id, receiver_id, message, timestamp, is_read) VALUES (?, ?, ?, ?, 0);";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, sender_id);
    sqlite3_bind_int(stmt, 2, receiver_id);
    sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 4, (sqlite3_int64)time(NULL));

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Retrieves notifications for a user
int get_notifications(int user_id, char *notifications_json) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, sender_id, receiver_id, message, timestamp, is_read FROM notifications WHERE receiver_id = ?;";
    char temp[8192] = "[";
    int first = 1;

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char *message = json_escape((const char *)sqlite3_column_text(stmt, 3));
        char entry[1024];
        snprintf(entry, sizeof(entry),
                 "%s{\"id\": %d, \"sender_id\": %d, \"receiver_id\": %d, \"message\": \"%s\", \"timestamp\": %lld, \"is_read\": %d}",
                 first ? "" : ",",
                 sqlite3_column_int(stmt, 0),
                 sqlite3_column_int(stmt, 1),
                 sqlite3_column_int(stmt, 2),
                 message,
                 (long long)sqlite3_column_int64(stmt, 4),
                 sqlite3_column_int(stmt, 5));
        free(message);
        strncat(temp, entry, sizeof(temp) - strlen(temp) - 1);
        first = 0;
    }

    sqlite3_finalize(stmt);
    strncat(temp, "]", sizeof(temp) - strlen(temp) - 1);
    strncpy(notifications_json, temp, 8192);
    return 1;
}

// Marks a notification as read
int mark_notification_read(int user_id, int notification_id) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE notifications SET is_read = 1 WHERE id = ? AND receiver_id = ?;";

    if (sqlite3_prepare_v2(app_ctx.db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(app_ctx.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, notification_id);
    sqlite3_bind_int(stmt, 2, user_id);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(app_ctx.db));
        sqlite3_finalize(stmt);
        return 0;
    }

    int changes = sqlite3_changes(app_ctx.db);
    sqlite3_finalize(stmt);
    return changes > 0;
}