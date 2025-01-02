#include <stdlib.h>
#include <sys/socket.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "../header_files/router.h"
#include "../header_files/http_helper.h"

#include "../header_files/utils.h"
#include "../header_files/config.h"
#include "../header_files/routes.h"

void router(const char *buffer, const int client_fd)
{
    regex_t regex;
    int result;
    regmatch_t matches[3];

    // Regular expression to match GET, POST, and DELETE requests
    const char *pattern = "^(GET|POST|DELETE) /([^ ]*) HTTP/1\\.[01]";

    // Compile the regular expression
    regcomp(&regex, pattern, REG_EXTENDED);

    char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
    size_t response_len=0;

    if (regexec(&regex, buffer, 3, matches, 0) == 0)
    {
        // Extract the method and resource from the matches
        char method[10];
        char url_encoded_resource[256];

        // Extract method
        snprintf(method, matches[1].rm_eo - matches[1].rm_so + 1, "%s", buffer + matches[1].rm_so);

        // Extract resource
        snprintf(url_encoded_resource, matches[2].rm_eo - matches[2].rm_so + 1, "%s", buffer + matches[2].rm_so);

        // Get the file name from encoded string
        char *resource_name = url_decode(url_encoded_resource);

        // Remove trailing slash if present
        size_t len = strlen(resource_name);
        if (len > 0 && resource_name[len - 1] == '/') {
            resource_name[len - 1] = '\0';
        }
        printf("%s -> %s\n", method,resource_name);

        // Handle the request based on the method
        handle_routes(buffer,method,resource_name,response,&response_len);

        free(resource_name);
        
    } else {
        printf("request does not match known methods.\n");
        build_501_response(response);
    }

    // send HTTP response to client
    send(client_fd, response, (response_len == 0)? strlen(response) : response_len, 0);
    free(response);
    printf("======\n");

    regfree(&regex);
}