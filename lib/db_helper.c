#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header_files/json_helper.h"
#include "../header_files/db_helper.h"

int num_users = 3;
const int num_keys_per_user = 3;
// const int field_len = 10;
char **users_list_keys = NULL;
char ***users_list_values = NULL;

void init_db()
{
    // Allocate memory for the list of users
    users_list_keys = (char **)malloc(num_users * sizeof(char *));
    users_list_values = (char ***)malloc(num_users * sizeof(char **));

    // Allocate memory for each user's list of keys
    for (int i = 0; i < num_users; i++) {
        users_list_values[i] = (char **)malloc(num_keys_per_user * sizeof(char *));
    }

    const char *keys[3] = {"id","name","email"};
    const char *values[3][3] = {{"001","Alice","alice@example.com"},{"002","Bob","bob@example.com"},{"003","Charlie","charlie@example.com"}};

    for (int j = 0; j < num_keys_per_user; j++) {
        users_list_keys[j] = strdup(keys[j]); // Allocate memory and copy string
    }
    for (int i = 0; i < num_users; i++) {
        for (int j = 0; j < num_keys_per_user; j++) {
            users_list_values[i][j] = strdup(values[i][j]); // Allocate memory and copy string
        }
    }

    // printf("user key:%s\n",users_list_keys[0][0]);
    // printf("user value:%s\n",users_list_values[0][0]);
}

char* get_users() {
    char *users_json = obj_list_to_json(users_list_keys,users_list_values,num_users,num_keys_per_user);

    return users_json;
}

char* get_user_by_id(const char *user_id){
    
    for (int i=0;i<num_users;i++)
    {
        //id -> 0
        if( strcmp(users_list_values[i][0],user_id) == 0)
        {
            char* json_user = obj_to_json(users_list_keys,users_list_values[i],num_keys_per_user);

            return json_user;
        }
    }

    return "";
}

int add_user(const char *id, const char *name, const char *email) {
    num_users++;

    // Allocate memory for the list of users
    char *** temp_users_list_values = realloc(users_list_values, (num_users) * sizeof(char **));

    if (temp_users_list_values == NULL) {
            perror("Failed to reallocate memory for users array");
            return -1;  // internal server error 500
    }

    users_list_values = temp_users_list_values;


    // Allocate memory for new user values
    users_list_values[num_users-1] = (char **)malloc(num_keys_per_user * sizeof(char *));

    const char *values[3] = {id,name,email};

    for (int j = 0; j < num_keys_per_user; j++) {
        users_list_values[num_users-1][j] = strdup(values[j]); // Allocate memory and copy string
    }

    return 1;
}

int delete_user(const char *user_id) {

    if(num_users == 0)
    {
        // user not found
        return 0;
    }
    
    // Find the index of the user to delete
    int indexToDelete = -1;
    for (int i=0;i<num_users;i++)
    {
        //id -> 0
        if( strcmp(users_list_values[i][0],user_id) == 0)
        {
            indexToDelete = i;
            break;
        }
    }

    // If the user was found
    if (indexToDelete != -1) {

        // Shift all users to the left to remove the selected user
        for (int i = indexToDelete; i < num_users - 1; ++i) {
            users_list_values[i] = users_list_values[i+1];
        }

        // reduce number of users
        num_users--;
        if(indexToDelete == 0 && num_users == 0)
        {
            //allocate null -> clear list
            users_list_values = NULL;
            return 1;
        }
        // Allocate memory for the list of users
        char *** temp_users_list_values = realloc(users_list_values, (num_users) * sizeof(char **));
        if (temp_users_list_values != NULL) {
            users_list_values = temp_users_list_values;
            return 1;
        } else {
            perror("Failed to reallocate memory for users array");
            return -1; // internal server error 500
        }
    } else {
        // user not found
        return 0;
    }
}
