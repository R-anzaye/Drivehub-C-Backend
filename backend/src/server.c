//Server.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sqlite3.h>
#include "mongoose.h"
#include "app.h"

// Declare the global app context
struct app_context app_ctx;

// Initialize the database schema
static int init_database() {
    if (!init_db(app_ctx.db)) {
        fprintf(stderr, "Failed to initialize database\n");
        return 0;
    }
    return 1;
}

// Event handler
static void event_handler(struct mg_connection *nc, int ev, void *ev_data) {
    struct mg_http_message *hm = (struct mg_http_message *)ev_data;

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
        } else if (mg_match(hm->uri, mg_str("/password"), NULL)) {
            handle_password(nc, hm, &app_ctx);
        } else if (mg_match(hm->uri, mg_str("/email"), NULL)) {
            handle_email(nc, hm, &app_ctx);
        } else if (mg_match(hm->uri, mg_str("/cars*"), NULL)) {
            handle_cars(nc, hm, &app_ctx);
        } else if (mg_match(hm->uri, mg_str("/notifications*"), NULL)) {
            handle_notifications(nc, hm, &app_ctx);
        } else {
            mg_http_reply(nc, 404, "Content-Type: text/plain\r\n", "Not Found\n");
        }
    }
}

int main(int argc, char *argv[]) {
    struct mg_mgr mgr;

    // Initialize app context
    app_ctx.jwt_secret = getenv("JWT_SECRET") ? getenv("JWT_SECRET") : "your-secure-jwt-secret-key-1234567890";
    if (sqlite3_open("drivehub.db", &app_ctx.db) != SQLITE_OK) {
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
    struct mg_connection *nc = mg_http_listen(&mgr, "http://localhost:5555", event_handler, NULL);
    if (!nc) {
        fprintf(stderr, "Error setting up listener!\n");
        sqlite3_close(app_ctx.db);
        return 1;
    }

    printf("Starting Mongoose web server on http://localhost:5555\n");
    
    // Main event loop
    for (;;) {
        mg_mgr_poll(&mgr, 1000);  // Poll for events every 1000 milliseconds
    }

    // Free Mongoose manager and close database
    mg_mgr_free(&mgr);
    close_db(app_ctx.db);

    return 0;
}