//Routes.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "mongoose.h"
#include "app.h"

// Helper function to extract JWT from Authorization header
static char *extract_jwt(struct mg_http_message *hm) {
    struct mg_str *auth_hdr = mg_http_get_header(hm, "Authorization");
    if (!auth_hdr) {
        fprintf(stderr, "No Authorization header found\n");
        return NULL;
    }
    if (strncmp(auth_hdr->buf, "Bearer ", 7) != 0) {
        fprintf(stderr, "Authorization header does not start with Bearer: %.*s\n",
                (int)auth_hdr->len, auth_hdr->buf);
        return NULL;
    }
    char *token = malloc(auth_hdr->len - 6);
    if (!token) {
        fprintf(stderr, "Memory allocation for token failed\n");
        return NULL;
    }
    strncpy(token, auth_hdr->buf + 7, auth_hdr->len - 7);
    token[auth_hdr->len - 7] = '\0';
    fprintf(stderr, "Extracted token: %s\n", token);
    return token;
}

// Get user ID from JWT token
int get_user_id_from_token(struct mg_connection *nc, struct mg_http_message *hm) {
    char *token = extract_jwt(hm);
    if (!token) {
        mg_http_reply(nc, 401, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Missing or invalid Authorization header\"}\n");
        return 0;
    }

    int user_id = verify_token(token);
    free(token);

    if (user_id <= 0) {
        mg_http_reply(nc, 401, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Invalid or expired token\"}\n");
        return 0;
    }

    return user_id;
}

// Handles user registration (POST /register)
void handle_register(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    json_t *root;
    json_error_t error;

    root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
    if (!root) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Invalid JSON\"}\n");
        return;
    }

    const char *first_name = json_string_value(json_object_get(root, "first_name"));
    const char *last_name = json_string_value(json_object_get(root, "last_name"));
    const char *email = json_string_value(json_object_get(root, "email"));
    const char *organization = json_string_value(json_object_get(root, "organization"));
    const char *password = json_string_value(json_object_get(root, "password"));

    if (!first_name || !last_name || !email || !organization || !password) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Missing fields\"}\n");
        json_decref(root);
        return;
    }

    int result = register_user(first_name, last_name, email, organization, password);
    if (result == 1) {
        mg_http_reply(nc, 201, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"message\": \"User registered\"}\n");
    } else if (result == -1) {
        mg_http_reply(nc, 409, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Email already in use\"}\n");
    } else {
        mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Registration failed\"}\n");
    }

    json_decref(root);
}

// Handles user login (POST /login)
void handle_login(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    json_t *root;
    json_error_t error;
    char token[512];

    root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
    if (!root) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Invalid JSON\"}\n");
        return;
    }

    const char *email = json_string_value(json_object_get(root, "email"));
    const char *password = json_string_value(json_object_get(root, "password"));

    if (!email || !password) {
        mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Missing fields\"}\n");
        json_decref(root);
        return;
    }

    if (login_user(email, password, token)) {
        mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"token\": \"%s\"}\n", token);
    } else {
        mg_http_reply(nc, 401, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "{\"error\": \"Invalid credentials\"}\n");
    }

    json_decref(root);
}

// Handles CORS preflight requests (OPTIONS)
void handle_options(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    mg_http_reply(nc, 200,
                  "Content-Type: text/plain\r\n"
                  "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                  "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                  "Access-Control-Allow-Headers: Content-Type, Authorization\r\n",
                  "");
}

// Handles user profile operations (GET/PUT/DELETE /profile)
void handle_profile(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Response already sent in get_user_id_from_token
    }

    if (mg_match(hm->method, mg_str("GET"), NULL)) {
        char profile[2048];
        if (get_user_profile(user_id, profile)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", profile);
        } else {
            mg_http_reply(nc, 404, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"User not found\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("PUT"), NULL)) {
        json_t *root;
        json_error_t error;

        root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
        if (!root) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid JSON\"}\n");
            return;
        }

        const char *first_name = json_string_value(json_object_get(root, "first_name"));
        const char *last_name = json_string_value(json_object_get(root, "last_name"));
        const char *organization = json_string_value(json_object_get(root, "organization"));

        if (!first_name || !last_name) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Missing required fields\"}\n");
            json_decref(root);
            return;
        }

        if (update_user_profile(user_id, first_name, last_name, organization ? organization : "")) {
            char updated_profile[2048];
            get_user_profile(user_id, updated_profile);
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", updated_profile);
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to update profile\"}\n");
        }

        json_decref(root);
    } else if (mg_match(hm->method, mg_str("DELETE"), NULL)) {
        if (delete_user(user_id)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"message\": \"Account deleted\"}\n");
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to delete account\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        handle_options(nc, hm, ctx);
    } else {
        mg_http_reply(nc, 405, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "Method Not Allowed\n");
    }
}

// Handles password update (PUT /password)
void handle_password(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Response already sent
    }

    if (mg_match(hm->method, mg_str("PUT"), NULL)) {
        json_t *root;
        json_error_t error;

        root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
        if (!root) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid JSON\"}\n");
            return;
        }

        const char *password = json_string_value(json_object_get(root, "password"));
        if (!password) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Missing password\"}\n");
            json_decref(root);
            return;
        }

        if (update_user_password(user_id, password)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"message\": \"Password updated\"}\n");
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to update password\"}\n");
        }

        json_decref(root);
    } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        handle_options(nc, hm, ctx);
    } else {
        mg_http_reply(nc, 405, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "Method Not Allowed\n");
    }
}

// Handles email update (PUT /email)
void handle_email(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Response already sent
    }

    if (mg_match(hm->method, mg_str("PUT"), NULL)) {
        json_t *root;
        json_error_t error;

        root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
        if (!root) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid JSON\"}\n");
            return;
        }

        const char *email = json_string_value(json_object_get(root, "email"));
        if (!email) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Missing email\"}\n");
            json_decref(root);
            return;
        }

        int result = update_user_email(user_id, email);
        if (result == 1) {
            char updated_profile[2048];
            get_user_profile(user_id, updated_profile);
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", updated_profile);
        } else if (result == -1) {
            mg_http_reply(nc, 409, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Email already in use\"}\n");
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to update email\"}\n");
        }

        json_decref(root);
    } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        handle_options(nc, hm, ctx);
    } else {
        mg_http_reply(nc, 405, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "Method Not Allowed\n");
    }
}

// Handles car operations (GET/POST /cars, DELETE /cars/:id)
void handle_cars(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Response already sent
    }

    if (mg_match(hm->method, mg_str("GET"), NULL) && mg_match(hm->uri, mg_str("/cars"), NULL)) {
        char cars_json[8192];
        if (get_cars(user_id, cars_json)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", cars_json);
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to fetch cars\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("POST"), NULL) && mg_match(hm->uri, mg_str("/cars"), NULL)) {
        json_t *root;
        json_error_t error;

        root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
        if (!root) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid JSON\"}\n");
            return;
        }

        const char *car_name = json_string_value(json_object_get(root, "car_name"));
        const char *year_of_manufacture = json_string_value(json_object_get(root, "year_of_manufacture"));
        const char *car_value = json_string_value(json_object_get(root, "car_value"));
        const char *photo = json_string_value(json_object_get(root, "photo"));

        if (!car_name || !year_of_manufacture || !car_value) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Missing fields\"}\n");
            json_decref(root);
            return;
        }

        if (add_car(user_id, car_name, year_of_manufacture, car_value, photo ? photo : "")) {
            char response[1024];
            snprintf(response, sizeof(response),
                     "{\"id\": %lld, \"car_name\": \"%s\", \"year_of_manufacture\": \"%s\", \"car_value\": \"%s\", \"photo\": \"%s\"}",
                     sqlite3_last_insert_rowid(ctx->db), car_name, year_of_manufacture, car_value, photo ? photo : "");
            mg_http_reply(nc, 201, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", response);
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to add car\"}\n");
        }

        json_decref(root);
    } else if (mg_match(hm->method, mg_str("DELETE"), NULL) && mg_match(hm->uri, mg_str("/cars/#"), NULL)) {
        int car_id;
        sscanf(hm->uri.buf + 6, "%d", &car_id);
        if (delete_car(user_id, car_id)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"message\": \"Car deleted\"}\n");
        } else {
            mg_http_reply(nc, 404, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Car not found or unauthorized\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        handle_options(nc, hm, ctx);
    } else {
        mg_http_reply(nc, 405, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "Method Not Allowed\n");
    }
}

// Handles notification operations (GET/POST /notifications, POST /notifications/:id/mark_read)
void handle_notifications(struct mg_connection *nc, struct mg_http_message *hm, struct app_context *ctx) {
    if (mg_match(hm->method, mg_str("OPTIONS"), NULL)) {
        handle_options(nc, hm, ctx);
        return;
    }

    int user_id = get_user_id_from_token(nc, hm);
    if (user_id <= 0) {
        return; // Response already sent
    }

    if (mg_match(hm->method, mg_str("POST"), NULL) && mg_match(hm->uri, mg_str("/notifications"), NULL)) {
        json_t *root;
        json_error_t error;
        root = json_loadb(hm->body.buf, hm->body.len, 0, &error);
        if (!root) {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid JSON\"}\n");
            return;
        }

        const char *message = json_string_value(json_object_get(root, "message"));
        json_t *receiver_id_json = json_object_get(root, "receiver_id");
        if (!message || !json_is_integer(receiver_id_json)) {
            json_decref(root);
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Missing or invalid message or receiver_id\"}\n");
            return;
        }

        int receiver_id = json_integer_value(receiver_id_json);
        if (send_notification(user_id, receiver_id, message)) {
            json_decref(root);
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"message\": \"Notification sent\"}\n");
        } else {
            json_decref(root);
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to send notification\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("GET"), NULL) && mg_match(hm->uri, mg_str("/notifications"), NULL)) {
        char notifications_json[8192];
        if (get_notifications(user_id, notifications_json)) {
            mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "%s\n", notifications_json);
        } else {
            mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Failed to retrieve notifications\"}\n");
        }
    } else if (mg_match(hm->method, mg_str("POST"), NULL) && mg_match(hm->uri, mg_str("/notifications/#id/mark_read"), NULL)) {
        struct mg_str id_str;
        if (mg_match(hm->uri, mg_str("/notifications/#id/mark_read"), &id_str)) {
            char id_buf[32];
            if (id_str.len > 0 && id_str.len < sizeof(id_buf)) {
                strncpy(id_buf, id_str.buf, id_str.len);
                id_buf[id_str.len] = '\0';
                int notification_id = atoi(id_buf);
                if (notification_id > 0) {
                    if (mark_notification_read(user_id, notification_id)) {
                        mg_http_reply(nc, 200, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                                      "{\"message\": \"Notification marked as read\"}\n");
                    } else {
                        mg_http_reply(nc, 500, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                                      "{\"error\": \"Failed to mark notification as read\"}\n");
                    }
                } else {
                    mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                                  "{\"error\": \"Invalid notification ID\"}\n");
                }
            } else {
                mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                              "{\"error\": \"Notification ID too long or empty\"}\n");
            }
        } else {
            mg_http_reply(nc, 400, "Content-Type: application/json\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                          "{\"error\": \"Invalid URI format\"}\n");
        }
    } else {
        mg_http_reply(nc, 405, "Content-Type: text/plain\r\nAccess-Control-Allow-Origin: http://localhost:5173\r\n",
                      "Method Not Allowed\n");
    }
}