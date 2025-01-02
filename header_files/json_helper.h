#pragma once

#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100
#define USER_NUM_KEYS 3

char* obj_list_to_json(char **keys, char ***values, int num_objs, int num_entries_per_object);

char* parse_user_json(const char *json_string, char output_array[USER_NUM_KEYS][MAX_VALUE_LEN]);

char* obj_to_json(char **keys, char **values, int num_entries_per_object);