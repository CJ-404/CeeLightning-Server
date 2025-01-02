#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../header_files/json_helper.h"

#define ID_LENGTH 10 // Adjust the length of the ID as needed

char* entry_to_json(char *key,char *value)
{
    //convert string into json format
    // "key":"value"

    // char *json_string = (char *)malloc((MAX_KEY_LEN+MAX_VALUE_LEN) * sizeof(char));
    char *json_string = (char *)calloc((MAX_KEY_LEN+MAX_VALUE_LEN) , sizeof(char));

    snprintf(
        json_string,
        100,
        "\"%s\":\"%s\"\0",
        key,
        value
    );

    // printf("converted json etry format: %s!\n",json_string);
    return json_string;
}

char* obj_to_json(char **keys, char **values, int num_entries_per_object)
{
    /*
        {
            "key1":"value1",
            "key2":"value2",
            "key3":"value3",
        }
    */

    // char *json_string = (char *)malloc(num_entries_per_object * (MAX_KEY_LEN+MAX_VALUE_LEN) * sizeof(char));
    char *json_string = (char *)calloc(num_entries_per_object * (MAX_KEY_LEN+MAX_VALUE_LEN), sizeof(char));


    strcat(json_string, "{");
    for (int index=0;index<num_entries_per_object;index++)
    {
        strcat(json_string, entry_to_json(keys[index],values[index]));
        if(index != (num_entries_per_object-1))
            strcat(json_string,",");
    }
    strcat(json_string,"}\0");

    // printf("converted json object format: %s!\n",json_string);
    return json_string;
}

char* obj_list_to_json(char **keys, char ***values, int num_objs, int num_entries_per_object)
{
    /*
        [
            {
                "key1":"value1",
                "key2":"value2",
                "key3":"value3",
            },
            {
                "key1":"value1",
                "key2":"value2",
                "key3":"value3",
            }   
        ]
    */

    // char *json_string = (char *)malloc(num_objs * num_entries_per_object * (MAX_KEY_LEN+MAX_VALUE_LEN) * sizeof(char));
    char *json_string = (char *)calloc(num_objs * num_entries_per_object * (MAX_KEY_LEN+MAX_VALUE_LEN), sizeof(char));


    strcat(json_string, "[");
    for (int index=0;index<num_objs;index++)
    {
        strcat(json_string, obj_to_json(keys,values[index],num_entries_per_object));
        if(index != (num_objs-1))
            strcat(json_string,",");
    }
    strcat(json_string,"]\0");

    // printf("converted json object list format: %s!\n",json_string);
    return json_string;
}

char *generate_random_id() {
    // Characters to use for generating the ID (only digits)
    const char charset[] = "0123456789";
    char *random_id = malloc(ID_LENGTH + 1); // Allocate memory for the ID (plus one for the null terminator)

    if (random_id) {
        for (size_t i = 0; i < ID_LENGTH; i++) {
            int key = rand() % (int)(sizeof(charset) - 1);
            random_id[i] = charset[key];
        }
        random_id[ID_LENGTH] = '\0'; // Null-terminate the string
    }

    return random_id;
}

char* parse_user_json(const char *json_string, char output_array[USER_NUM_KEYS][MAX_VALUE_LEN]) {
    char *keys[USER_NUM_KEYS] = {"name", "id", "email"};
    char *value_start, *value_end;
    char key[MAX_KEY_LEN];

    for (int i = 0; i < USER_NUM_KEYS; i++) {
        // Search string for each key (e.g., "\"name\":\"")
        snprintf(key, sizeof(key), "\"%s\":\"", keys[i]);

        value_start = strstr(json_string, key);
        if (value_start) {
            value_start += strlen(key);  // Move pointer to the start of the value
            value_end = strchr(value_start, '\"');  // Find the end of the value

            if (value_end) {
                strncpy(output_array[i], value_start, value_end - value_start);
                output_array[i][value_end - value_start] = '\0';
            } else{
                // invlid format
                return NULL;
            }
        } else if(strcmp(keys[i],"id")==0)
        {
            printf("missing id. Generating...\n");
            // Seed the random number generator with the current time
            srand((unsigned int)time(NULL));
            // generate id
            char *id = generate_random_id();
            if (id){
                strcpy(output_array[i], id);
                free(id);
            } else{
                printf("id generation failed\n");
                // server error -> 500
                return "-1";
            }
            
        } else{
            // keys[i] is missing
            return keys[i];
        }
    }
    return "";
}