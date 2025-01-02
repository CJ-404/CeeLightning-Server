#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "header_files/file_helper.h"
#include "header_files/http_helper.h"

#include "header_files/utils.h"
#include "header_files/config.h"
#include "header_files/router.h"

void build_http_response(const char *file_name, const char *file_ext, char *response, size_t *response_len)
{

    printf("Considered FILE NAME: %s \n", file_name);

    // build HTTP header
    const char *mime_type = get_mime_type(file_ext);

    // Try to open the requested file and if file not exist, response is 404 Not Found
    int file_fd = get_file_descriptor(SRC_DIR, file_name);
    if (file_fd == -1)
    {
        build_404_response(response);
        *response_len = strlen(response);
        return;
    }

    // Get file size for Content-Length
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    off_t file_size = file_stat.st_size;
    printf("FILE_DESC: %d  SIZE: %li \n", file_fd, file_size);

    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(header, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %li\r\n"
             "\r\n",
             mime_type,
             file_size
    );

    // Copy header to response buffer
    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    // Copy file to response buffer
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, response + *response_len, BUFFER_SIZE - *response_len)) > 0)
    {
        *response_len += bytes_read;
    }

    if (bytes_read == -1) {
        printf("Failed to read file\n");
        free(response);
        response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
        *response_len = 0;
        build_500_response(response);
        // server error -> 500
    }

    free(header);
    close(file_fd);
}

void build_404_json_response(char *response, char *content)
{
    // {"error": "content"}

    char content_json[50];

    snprintf(content_json,
                50,
                    "{\"error\": \"%s\"}",
                        content
                );

    snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s\r\n",
            strlen(content_json),
            content_json
    );
}

void build_404_response(char *response)
{
    // not found html response
    
    snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html>"
                "<body>"
                    "<h3>404 Not Found<h3>"
                    "<hr>"
                    "<p>we couldn't find what you want<p>"
                "</body>"
            "</html>"
    );
}

void build_200_response(char *response, char *content_json)
{
    // json data
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(content_json),
        content_json
    );
}

void build_201_response(char *response, char *content_json)
{
    // json data
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 201 Created\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(content_json),
        content_json
    );
}

void build_400_response(char *response, char *error_msg)
{
    // {"error": "error_msg"}

    char error_json[50];

    snprintf(error_json,
                50,
                    "{\"error\": \"%s\"}",
                        error_msg
                );
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(error_json),
        error_json
    );
}

void build_415_response(char *response)
{
    // {"error": "Unsupported Content-Type"}

    const char* error_json = "{\"error\": \"Unsupported Content-Type\"}";
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 415 Unsupported Media Type\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(error_json),
        error_json
    );
}

void build_501_response(char *response)
{
    // {"error": "HTTP method not implemented"}
    
    const char* error_json = "{\"error\": \"HTTP method not implemented\"}";
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 501 Not Implemented\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(error_json),
        error_json
    );
}

void build_204_response(char *response)
{
    // HTTP/1.1 204 No Content
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 204 No Content\r\n"
    );
}

void build_500_json_response(char *response)
{
    // {"error": "An unexpected error occurred"}
    
    const char* error_json = "{\"error\": \"An unexpected error occurred\"}";
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s\r\n",
        strlen(error_json),
        error_json
    );
}

void build_500_response(char *response)
{
    // 500 html response
    
    snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
            "\r\n"
            "<html>"
                "<body>"
                    "<h3>500 Internel Server Error<h3>"
                    "<hr>"
                    "<p>Sorry error occurred<p>"
                "</body>"
            "</html>"
    );
}


void *handle_client(void *arg)
{
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // receive request data from client and store into buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        router(buffer,client_fd);
    }

    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}