#pragma once

void build_http_response(const char *file_name, const char *file_ext, char *response, size_t *response_len);

void *handle_client(void *arg);

void build_404_response(char *response);

void build_404_json_response(char *response, char *content);

void build_200_response(char *response, char *content_json);

void build_201_response(char *response, char *content_json);

void build_400_response(char *response, char *error_msg);

void build_415_response(char *response);

void build_501_response(char *response);

void build_204_response(char *response);

void build_500_response(char *response);

void build_500_json_response(char *response);