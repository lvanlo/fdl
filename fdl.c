#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libsocket/libinetsocket.h>
#include <ctype.h>

FILE * connect1();
void menu(FILE *s);
char get_choice();
void list_files(FILE *s);
void download(FILE *s);
//void downloadAll(FILE *s);
void quit(FILE *s);
void error(char *s);
char listArray[20][30];
char *nameFormatter(char *str);
long size(FILE *s, char *name);

void error(char *s)
{
    fprintf(stderr, "%s \n", s);
    exit(1);
}

int main()
{
    // Connect
    FILE * s = connect1();
    
    // Menu
    menu(s);
    

}

/*
 * Connect to server. Returns a FILE pointer that the
 * rest of the program will use to send/receive data.
 */
FILE * connect1()
{
    // Connect to runwire.com
    int fd = create_inet_stream_socket("runwire.com", "1234", LIBSOCKET_IPv4, 0);
    if (fd == -1)
    {
        printf("Can't connect to service");
        exit(1);
    }
    
    // Open the socket number (fd) as a FILE
    FILE *s = fdopen(fd, "r+");
    char request[50];
    fgets(request, 50, s);    
    if (!strstr(request, "+OK Greetings")) error("Didn't get +OK");    
    
    fprintf(s, "LIST\n");
    fgets(request, 50, s);
    if (!strstr(request, "+OK")) error("Didn't get +OK for list");
    
    int i;
    int count = 0;
    char listContents[30];
    while (fgets(request, 50, s) != NULL)
    {
        int trip = 0;
        int spaces = 0;
        if (strstr(request, ".\n")) break;
        for (i = 0; i < strlen(request) - 1; i++)
        {
            if (isdigit(request[i]) && trip == 0)
            {
                spaces++;
                continue;
            }
            if (isspace(request[i]))
            {
                spaces++;
                count++;
                trip++;
                continue;
            }
            sprintf(&listContents[i - spaces], "%c", request[i]);
        }
        sprintf(&listContents[i - spaces], "\n");
        strcpy(listArray[count], listContents);
    }
    return s;
}

/*
 * Display menu of choices.
 */
void menu(FILE *s)
{
    printf("List files: L\n");
    printf("Download a file: D\n");
    //printf("Downoad all files: A\n");
    printf("Quit: Q\n");
    printf("\n");
    
    // Get choice
    char choice = get_choice();
    
    // Handle choice
    switch(choice)
    {
        case 'l':
        case 'L':
            list_files(s);
            break;
        
        case 'd':
        case 'D':
            download(s);
            break;
        /*
        case 'a':
        case 'A':
            downloadAll(s);
            break;
         */   
        case 'q':
        case 'Q':
            quit(s);
            exit(0);
            break;
            
        default:
            printf("Choice must be d, l, a, or q\n");
            menu(s);
        
    }    
}

/*
 * Get the menu choice from the user. Allows the user to
 * enter up to 100 characters, but only the first character
 * is returned.
 */
char get_choice()
{
    printf("Your choice: ");
    char buf[100];
    scanf("%s", buf);
    return buf[0];
}


 //Display a file list to the user.

void list_files(FILE *s)
{
    for (int i = 1; i < 15; i++)
    {
        if (isalpha(*listArray[i]))
        {
            printf("(%d) %s", i, listArray[i]);
        }
    }
    menu(s);
}

/*
 * Download a file.
 * Prompt the user to enter a filename.
 * Download it from the server and save it to a file with the
 * same name.
 */
 
 
 
void download(FILE *s)
{
    printf("Enter entry number of filename: ");
    char name[50];
    scanf("%s", name);
    if (!strcmp(name, "q") || !strcmp(name, "Q"))
    {
        quit(s);
        exit(0);
    }
    
    if(isdigit(name[0]))
    {
        int x;
        sscanf(name, "%d", &x);
        strcpy(name, listArray[x]);
    }
    
    char *nameF = nameFormatter(name);
    long filesize = size(s, nameF);
    long rem = filesize;
    
    char buf[1000];
    fprintf(s, "GET %s\n", nameF);
    fgets(buf, 50, s);
    if (!strstr(buf, "+OK")) printf("\%s\" file not found.", nameF);
    
    FILE *f = fopen(nameF, "w");
    if(!f)
    {
        printf("Can't open %s for writing. \n", nameF);
        exit(1);
    }
    
    int bucket = 1000;
    long recieved = 0;
    int progress = 1;
    while (rem > 0)
    {
        if (rem > 1000) bucket = 1000;
        else bucket = rem;
        
        recieved = fread(buf, 1, bucket, s);
        fwrite(buf, 1, recieved, f);
        rem -= recieved;
        if (rem < filesize * (10-progress) / 10)
        {
            printf("*");
            progress++;
            fflush(stdout);
        }
    }
    if (progress < 10) 
    {
        for ( ; progress < 10; progress++)
        {
            printf("*");
        }
    }
    printf("100%% Downloaded %s\n", nameF);
    
    menu(s);
}
/*
void downloadALL(FILE *s)
{
    char *name;
    for (int i = 1; i <20; i++)
    {
        if(!isalpha(*listArray[i])) break;
        else name = listArray[i];
    
    
        char *nameF = nameFormatter(name);
        long filesize = size(s, nameF);
        long rem = filesize;
    
        char buf[1000];
        fprintf(s, "GET %s\n", nameF);
        fgets(buf, 50, s);
        if (!strstr(buf, "+OK")) printf("\%s\" file not found.", nameF);
    
        FILE *f = fopen(nameF, "w");
        if(!f)
        {
            printf("Can't open %s for writing. \n", nameF);
            exit(1);
        }
    
        int bucket = 1000;
        long recieved = 0;
        int progress = 1;
        while (rem > 0)
        {
            if (rem > 1000) bucket = 1000;
            else bucket = rem;
        
            recieved = fread(buf, 1, bucket, s);
            fwrite(buf, 1, recieved, f);
            rem -= recieved;
            if (rem < filesize * (10-progress) / 10)
            {
                printf("*");
                progress++;
                fflush(stdout);
            }
        }
        if (progress < 10) 
        {
            for ( ; progress < 10; progress++)
            {
                printf("*");
            }
        }
        printf("100%% Downloaded %s\n", nameF);
    }
    printf("\n Downloaded all files.\n");
    quit(s);
    exit(0);
}
*/

long size (FILE *s, char *name)
{
    char buf[20];
    char *ptr;
    fprintf(s, "SIZE %s\n", name);
    fgets(buf, 20, s);
    if (!strstr(buf, "+OK"))
    {
        printf("\"%s\" file not found.\n\n", name);
        download(s);
    }
    return strtol(&buf[4], &ptr, 10);
}





/* 
 * Close the connection to the server.
 */
void quit(FILE *s)
{
    char request[50];
    fprintf(s,"QUIT\n");
    fgets(request, 50, s);
    if (!strstr(request, "+OK")) error("Didn't get +OK");    
    printf("%s", request);
    fclose(s);
}

char *nameFormatter(char *str)
{
    for (int i; i < 20; i++)
    {
        if (str[i] == '\n') 
        {
            str[i] = '\0';
        }
    }
    return str;
}