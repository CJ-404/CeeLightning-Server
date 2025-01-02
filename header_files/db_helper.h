#pragma once

extern char **users_list_keys;
extern char ***users_list_values;

extern int num_users;
extern const int num_keys_per_user;
extern const int field_len;

void init_db();

char* get_users();

char* get_user_by_id(const char *user_id);

int add_user(const char *id, const char *name, const char *email);

int delete_user(const char *user_id);
