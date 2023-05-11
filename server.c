/**
 * Skeleton file for server.c
 * 
 * You are free to modify this file to implement the server specifications
 * as detailed in Assignment 3 handout.
 * 
 * As a matter of good programming habit, you should break up your imple-
 * mentation into functions. All these functions should contained in this
 * file as you are only allowed to submit this file.
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
// Include necessary header files

/**
 * When recieving messages from client, may contain newline characters
 * Server must account for this
 * 
 * The method below strips the newline character from the recieved message before
 * comparing it to 'BYE'
 * 
 * @param str - array/pointer of characters (a string)
 */
void strip_newline(char *str){
    int len = strlen(str);
    if(len > 0 && str[len - 1] == '\n'){
        str[len - 1] = '\0';
    }
}

/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 * 
 * Read the assignment handout for more details about the server program
 * design specifications.
 */ 
int main(int argc, char *argv[])
{
    // Check if port number is entered into command line
    if(argc != 2){
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        return -1;
    }

    int port = atoi(argv[1]); // converts arg into integer

    // Check if specified port number is less than 1024
    if(port < 1024){
        fprintf(stderr, "Error: Port number must be greater than or equal to 1024.\n");
        return -1;
    }

    // Create TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        printf("Error creating socket");
        exit(0);
    }
  
    // Initializing for Binding the socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        printf("Error binding socket");
        exit(0);
    }

    //Listen for TCP clients at the port specified in the command line
    if(listen(fd, SOMAXCONN) < 0){
        printf("Error listening for connections");
        exit(0);
    }

    // Declaring client address struct and its size
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    
    // Main loop to accept connections indefinately
    while (1)
    {
        // Accept connection from a client
        int client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
        if (client_fd < 0)
        {
            printf("Error accepting connection");
            exit(0);
        }
        
        // Send the HELLO message to the connected client
        const char *hello_msg = "HELLO";
        ssize_t r = send(client_fd, hello_msg, strlen(hello_msg), 0);
        if (r < 0)
        {
            printf("Error sending message");
            close(client_fd);
            exit(0);
        }

        // Loop to handle communication with connected client
        while (1)
        {   
            // Recieve message from client
            char incoming[100];
            ssize_t s = recv(client_fd, incoming, 100 - 1, 0);
            if (s <= 0)
            {
                printf("Error receiving message");
                close(client_fd);
                exit(0);
            }
            incoming[s] = '\0';
            strip_newline(incoming);  // Remove newline characters from the recieved message
            printf("Received message: %s\n", incoming);

            // Check if message recieved is "BYE"
            bool bye_received = false;
            if (strcasecmp(incoming, "BYE") == 0)
            {
                bye_received = true;
            }
            
            // If "BYE" recieved, close the connection and break the loop
            if (bye_received)
            {
                close(client_fd);
                break;
            }
        }
    }
 
    return 0;
}

