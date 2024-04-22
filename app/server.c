#include<stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4221
#define SO_REUSEPORT 15

int main(){
    int serverfiledescriptor, clientfiledescriptor;
    socklen_t clientlength;
    struct sockaddr_in server_address, client_address;
    
    memset(&server_address, 0, sizeof(server_address));
    memset(&client_address, 0, sizeof(client_address));

    char buffer[512];
    // char *message = "HEAD / HTTP/1.0\r\n\r\n";
     char *message1 ="HTTP/1.1 200 OK\r\n\r\n";
    char *message2 = "HTTP/1.1 404 Not Found\r\n\r\n";

    serverfiledescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfiledescriptor < 0){
        fprintf(stderr, "Error creating socket\n");
        return 1;
    }

     int reuse = 1;
  if (setsockopt(serverfiledescriptor, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEPORT failed:\n");
    return 1;
  }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = 0;

    if(bind(serverfiledescriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
        fprintf(stderr, "Error binding socket\n");
        return 1;
    }

    if(listen(serverfiledescriptor, 5) < 0){
        fprintf(stderr, "Error listening on socket\n");
        return 1;
    }

    clientlength = sizeof(client_address);

    clientfiledescriptor = accept(serverfiledescriptor, (struct sockaddr *)&client_address, &clientlength);
    if(clientfiledescriptor < 0){
        fprintf(stderr, "Error accepting connection\n");
        return 1;
    }

    memset(buffer, 0, 512);
    if(read(clientfiledescriptor, buffer, 511) < 0){
        fprintf(stderr, "Error reading from socket\n");
        return 1;
    }

    printf("Received: %s\n", buffer);
    int i;
    for( i=0;i<512;i++){
        // printf("%c", buffer[i]);
        if(buffer[i] == '/'){
            break;
        }
    }
    if(buffer[++i] == ' '){
        printf("HTTP/1.1 200 OK\n");
        send(clientfiledescriptor, message1, strlen(message1), 0);
    }else{
        printf("HTTP/1.1 404 Not Found\n");
        send(clientfiledescriptor, message2, strlen(message2), 0);
    }
    char *method = strtok(buffer, " ");


    close(clientfiledescriptor);
    close(serverfiledescriptor);

    return 0;
}