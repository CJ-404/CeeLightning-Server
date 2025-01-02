#pragma once

#define ROOT_ROUTE = ""

#define USERS_ROUTE "users"

#define USER_ROUTE "user?id="

#define USER_ROUTE_LEN 8

void handle_routes(const char *buffer,const char *method,char *resource_name, char *response,size_t *response_len);