#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include "mongoose.h"
#include "app.h"

// Handles user registration (POST /register)
void handle_register(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    json_t *root;
    json_error_t error;

    // Parse JSON body
    root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
    if (!root) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\n", "{\"error\": \"Invalid JSON\"}\n");
        return;
    }

    // Extract fields
    const char *first_name = json_string_value(json_object_get(root, "first_name"));
    const char *last_name = json_string_value(json_object_get(root, "last_name"));
    const char *email = json_string_value(json_object_get(root, "email"));
    const char *organization = json_string_value(json_object_get(root, "organization"));
    const char *password = json_string_value(json_object_get(root, "password"));

    if (!first_name || !last_name || !email || !organization || !password) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\n", "{\"error\": \"Missing fields\"}\n");
        json_decref(root);
        return;
    }

    // Register user
    if (register_user(first_name, last_name, email, organization, password)) {
        mg_http_reply(nc, 201, "Content-Type: application/json\r\n", "{\"message\": \"User registered\"}\n");
    } else {
        mg_http_reply(nc, 409, "Content-Type: application/json\r\n", "{\"error\": \"Registration failed\"}\n");
    }

    json_decref(root);
}

// Handles user login (POST /login)
void handle_login(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    json_t *root;
    json_error_t error;
    char token[256];

    // Parse JSON body
    root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
    if (!root) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\n", "{\"error\": \"Invalid JSON\"}\n");
        return;
    }

    // Extract fields
    const char *email = json_string_value(json_object_get(root, "email"));
    const char *password = json_string_value(json_object_get(root, "password"));

    if (!email || !password) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\n", "{\"error\": \"Missing fields\"}\n");
        json_decref(root);
        return;
    }

    // Authenticate user
    if (login_user(email, password, token)) {
        mg_http_reply(nc, 200, "Content-Type: application/json\r\n", "{\"token\": \"%s\"}\n", token);
    } else {
        mg_http_reply(nc, 401, "Content-Type: application/json\r\n", "{\"error\": \"Invalid credentials\"}\n");
    }

    json_decref(root);
}

// Handles CORS preflight requests (OPTIONS)
void handle_options(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    mg_http_reply(nc, 200,
                  "Content-Type: text/plain\r\n"
                  "Access-Control-Allow-Origin: *\r\n"
                  "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
                  "Access-Control-Allow-Headers: Content-Type, Authorization\r\n",
                  "");
}