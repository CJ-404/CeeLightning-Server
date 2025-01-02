#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../header_files/file_helper.h"
#include "../header_files/http_helper.h"

#include "../header_files/utils.h"
#include "../header_files/db_helper.h"
#include "../header_files/routes.h"
#include "../header_files/json_helper.h"
#include "../header_files/config.h"

void handle_routes(const char *buffer,const char *method,char *resource_name, char *response, size_t *response_len)
{
    if (strcmp(method, "GET") == 0) {
        // Handle GET request
        // response_len = strlen(response);

        char *user_id_start;

        // char* user_list_json = obj_list_to_json(users_list_keys,users_list_values,num_users,num_keys_per_user);
        // printf("user list json:\n%s\n",user_list_json);
        
        //routes
        if(strcmp(resource_name, USERS_ROUTE) == 0){
            // all users
            printf("get all users\n");
            // char* users = get_users();
            char* users = get_users();
            printf("%s\n",users);
            build_200_response(response,users);
        } else if(user_id_start = strstr(buffer, USER_ROUTE)){
            // an user
            user_id_start += USER_ROUTE_LEN;
            const char *user_id_end = strstr(user_id_start, " HTTP/1.1");
            char id[20];
            strncpy(id,user_id_start,user_id_end-user_id_start);
            id[user_id_end-user_id_start] = '\0';
            
            if(strlen(id)==0)
            {
                //user id not provided
                printf("user id not provided\n");
                build_400_response(response, "user id parameter is missing");
                
            } else{
                //get user data
                printf("get user data:%s!\n",id);
                char *user_json = get_user_by_id(id);
                if (strcmp(user_json,"") == 0)
                {
                    // user not found
                    printf("user not found\n");
                    build_404_json_response(response, "user not found");
                }
                else{
                    printf("%s!\n",user_json);
                    build_200_response(response, user_json);
                }
            }
        }
        // html pages routes
        else {
            
            // If the root route is requested in GET, then respond the index page.
            if (strlen(resource_name) == 0)    //|| strcmp(resource_name, ""))
            {
                strcpy(resource_name, "index.html");
            }
            // Get file extension
            printf("File resource request\n");
            char file_ext[32];
            strcpy(file_ext, get_file_extension(resource_name));
            build_http_response(resource_name, file_ext, response,response_len);
        }
    } else if (strcmp(method, "POST") == 0) {

        // printf("buffer:\n%s\n",buffer);
        // Handle POST request

        // check for application/json content type
        char *content_type_json = strstr(buffer, "Content-Type: application/json");
        if(content_type_json)
        {
            // Find the start of the JSON body in the request
            char *json_start = strstr(buffer, "\r\n\r\n");
            if (json_start) {
                json_start += 4;  // Skip past the "\r\n\r\n" to the start of the JSON
                printf("body:%s\n",json_start);
            }

            //define routes
            if (strcmp(resource_name, USERS_ROUTE) == 0)
            {
                //users
                char output_data[USER_NUM_KEYS][MAX_VALUE_LEN];
                char* result = parse_user_json(json_start,output_data);
                if (result == NULL)
                {
                    //invalid json format
                    printf("invalid json format\n");
                    build_400_response(response, "Invalid CJson format");
                } else if(strcmp(result, "-1") == 0)
                {
                    // random number egenration failed -> 500
                    build_500_json_response(response);
                } else if(strlen(result) != 0)
                {
                    //missing some params
                    printf("missing some params -> %s\n",result);
                    // build HTTP response
                    char content[40];
                    snprintf(content,BUFFER_SIZE,"user %s is missing",result);
                    build_400_response(response, content);
                } else{
                    //success
                    printf("name:%s\n",output_data[0]);
                    printf("id:%s\n",output_data[1]);
                    printf("email:%s\n",output_data[2]);

                    // add data to the database
                    int result = add_user(output_data[1],output_data[0],output_data[2]);
                    if(result == -1)
                    {
                        //500 error
                        build_500_json_response(response);
                    } else {
                        // added
                        printf("user added\n");
                        char content_json[200];
                        snprintf(content_json,
                            200,
                            "{\"id\":\"%s\",\"name\":\"%s\",\"email\":\"%s\"}",
                            output_data[1], output_data[0], output_data[2]
                        );
                        // build HTTP response
                        build_201_response(response, content_json);
                    }
                }

            } else {
                // 404 not found
                printf("route not defined\n");
                build_404_json_response(response, "url Not Found");
            }
        } else {
            // content type is not supported
            // build HTTP response
            printf("Content Type Not Supported\n");
            build_415_response(response);
        }
    } else if (strcmp(method, "DELETE") == 0) {
        // Handle DELETE request
        printf("DELETE request\n");
        char *user_id_start;

        if(user_id_start = strstr(buffer, USER_ROUTE)){
            // an user
            user_id_start += USER_ROUTE_LEN;
            const char *user_id_end = strstr(user_id_start, " HTTP/1.1");
            char id[20];
            strncpy(id,user_id_start,user_id_end-user_id_start);
            id[user_id_end-user_id_start] = '\0';
            
            if(strlen(id)==0)
            {
                //user id not provided
                printf("user id not provided\n");
                build_400_response(response, "user id parameter is missing");
                
            } else{
                //get user data
                printf("get user data:%s!\n",id);
                char *user_json = get_user_by_id(id);
                if (strcmp(user_json,"") == 0)
                {
                    // user not found
                    printf("user not found\n");
                    build_404_json_response(response, "User not found");
                }
                else{
                    printf("%s!\n",user_json);
                    //delete user
                    int result = delete_user(id);
                    if (result == -1)
                    {
                        //500 error
                        build_500_json_response(response);
                    }
                    else if(result == 0)
                    {
                       // user not found
                        printf("user not found\n");
                        build_404_json_response(response, "User not found");
                    }
                    else if(result == 1)
                    {
                        printf("deleted user:%s!\n",id);
                        build_204_response(response);
                    }
                }
            }
        }else{
            // 404 not found
            printf("route not defined\n");
            build_404_json_response(response, "url Not Found");
        }
    }
}