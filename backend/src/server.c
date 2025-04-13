#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sqlite3.h>
#include "mongoose.h"
#include "app.h"

// Fallback for mg_str_starts_with
static bool my_str_starts_with(struct mg_str str, struct mg_str prefix) {
    if (str.len < prefix.len) return false;
    return memcmp(str.buf, prefix.buf, prefix.len) == 0;
}

// Declare the global app context
struct app_context app_ctx;

// Initialize the database schema
static int init_database() {
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "first_name TEXT NOT NULL,"
                      "last_name TEXT NOT NULL,"
                      "email TEXT UNIQUE NOT NULL,"
                      "organization TEXT NOT NULL,"
                      "password TEXT NOT NULL);";
    return execute_sql(sql);
}

// Middleware: extracts user_id from token
int get_user_id_from_token(struct mg_connection *nc, struct mg_http_message *hm) {
    struct mg_str *auth_header = mg_http_get_header(hm, "Authorization");
    if (!auth_header || !my_str_starts_with(*auth_header, mg_str("Bearer "))) {
        mg_http_reply(nc, 401, "Content-Type: application/json\r\n", "{ \"error\": \"Invalid or missing token\" }\n");
        return 0;
    }

    const char *token = auth_header->buf + 7;
    return verify_token(token) ? atoi(token + 6) : 0; // Assuming token format "token_<id>_<timestamp>"
}

// Event handler
static void event_handler(struct mg_connection *nc, int ev, void *ev_data) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

    // Access the global context
    if (ev == MG_EV_HTTP_MSG) {
        if (mg_match(hm->uri, mg_str("/profile"), NULL)) {
            handle_profile(nc, hm, &app_ctx);
        } else if (mg_match(hm->uri, mg_str("/register"), NULL)) {
            if (mg_match(hm->method, mg_str("POST"), NULL)) {
                handle_register(nc, hm, &app_ctx);
            } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
                handle_options(nc, hm, &app_ctx);
            } else {
                mg_http_reply(nc, 405, "Content-Type: text/plain\r\n", "Method Not Allowed\n");
            }
        } else if (mg_match(hm->uri, mg_str("/login"), NULL)) {
            if (mg_match(hm->method, mg_str("POST"), NULL)) {
                handle_login(nc, hm, &app_ctx);
            } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
                handle_options(nc, hm, &app_ctx);
            } else {
                mg_http_reply(nc, 405, "Content-Type: text/plain\r\n", "Method Not Allowed\n");
            }
        } else {
            mg_http_reply(nc, 404, "Content-Type: text/plain\r\n", "Not Found\n");
        }
    }
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;

    // Initialize the database
    if (sqlite3_open("instance/users.db", &app_ctx.db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(app_ctx.db));
        return 1;
    }

    if (!init_database()) {
        fprintf(stderr, "Failed to initialize database schema\n");
        sqlite3_close(app_ctx.db);
        return 1;
    }

    mg_mgr_init(&mgr);  // Initialize the manager

    // Set up the HTTP listener for the server
    struct mg_connection *nc = mg_http_listen(&mgr, "http://localhost:8080", event_handler, NULL);
    if (!nc) {
        fprintf(stderr, "Error setting up listener!\n");
        sqlite3_close(app_ctx.db);
        return 1;
    }

    printf("Starting Mongoose web server on http://localhost:8080\n");
    
    // Main event loop
    for (;;) {
        mg_mgr_poll(&mgr, 1000);  // Poll for events every 1000 milliseconds
    }

    // Free Mongoose manager and close database
    mg_mgr_free(&mgr);
    sqlite3_close(app_ctx.db);

    return 0;
}