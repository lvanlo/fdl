#include <stdio.h>
#include <libsocket/libinetsocket.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    
    // Connect to aerisapi.com
    int fd = create_inet_stream_socket("api.aerisapi.com", "80", LIBSOCKET_IPv4, 0);
    if (fd == -1)
    {
        printf("Can't connect to service");
        exit(1);
    }
    
    // Open the socket number (fd) as a FILE
    FILE *f = fdopen(fd, "rb+");
    
    // Send GET request to web server
    char request[1000];
    char *client_id = "wQhXMMnxoRV4HNKoRLZrL ";
    char *client_secret = "rUOW0GEyf5bT9JhUzro2WQAuUpj3A7nFHgVCRGEK";
    fprintf(f, "GET /observations/%s?client_id=%s&client_secret=%s HTTP/1.0\n", argv[1], client_id, client_secret);
    fprintf(f, "Host: api.aerisapi.com\n");
    fprintf(f, "\n");
    
    char line[10000];
    while (fgets(line, 10000, f) != NULL)
    {
        // The quote is between <p> and <\/p> tags. Search for them.
        char * start = strstr(line, "tempF");
        char * start1 = strstr(line, "name");
        char * start2 = strstr(line, "dity");
        char * start3 = strstr(line, "dDir\"");
        char * start4 = strstr(line, "dMPH");
        if (start4 == NULL)
            continue;   // Not interested in lines that don't have <p>
        char * end = strstr(start, ",");
        char * end1 = strstr(start1, "\",");
        char * end2 = strstr(start2, ",");
        char * end3 = strstr(start3, "\",");
        char * end4 = strstr(start4, ",");
        *end = '\0';
        *end1 = '\0';
        *end2 = '\0';
        *end3 = '\0';
        *end4 = '\0';

        printf("Temperature: %s F\n", start+7);
        printf("Location: %s\n", start1+7);
        printf("Humidity: %s \n", start2+6);
        printf("Wind Direction: %s %s MPH\n", start3+7, start4+6);
    }
    fclose(f);
}