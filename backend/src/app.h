#ifndef APP_H
#define APP_H

#include <sqlite3.h>
#include <sodium.h>
#include "mongoose.h"

// Application context to hold shared state
struct app_context {
    sqlite3 *db;          // Database handle
    const char *jwt_secret; // JWT secret key
};

// Global application context
extern struct app_context app_ctx;

// Database initialization
int init_db(sqlite3 *db);
void close_db(sqlite3 *db);

// Utility functions
void hash_password(const char *password, char *hashed_output);

// User management functions
int register_user(const char *first_name, const char *last_name, const char *email,
                  const char *organization, const char *password);
int login_user(const char *email, const char *password, char *token);
int verify_token(const char *token);
int get_user_profile(int user_id, char *profile);
int update_user_profile(int user_id, const char *first_name, const char *last_name, const char *organization);
int update_user_password(int user_id, const char *password);
int update_user_email(int user_id, const char *email);
int delete_user(int user_id);
int get_user_id_from_token(struct mg_connection *nc, struct mg_http_message *hm);

// Car management functions
int add_car(int user_id, const char *car_name, const char *year_of_manufacture,
            const char *car_value, const char *photo);
int get_cars(int user_id, char *cars_json);
int delete_car(int user_id, int car_id);

// Notification management functions
int send_notification(int sender_id, int receiver_id, const char *message);
int get_notifications(int user_id, char *notifications_json);
int mark_notification_read(int user_id, int notification_id);

// Route handlers
void handle_register(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_login(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_profile(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_password(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_email(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_options(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_cars(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);
void handle_notifications(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx);

#endif