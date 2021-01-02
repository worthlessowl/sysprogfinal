#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];

int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting Mouse Poll changer\n");
   fd = open("/sys/module/usbhid/parameters/mousepoll", O_RDWR);             
   if (fd < 0){
          perror("Failed to open the device...");
          return errno;
   }
   
   printf("Enter your desired mouse polling rate\n");
   scanf("%[^\n]%*c", stringToSend);                
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, stringToSend, strlen(stringToSend)); 
   if (ret < 0){
           perror("Failed to write to the device.");
           return errno;
   }

   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);        
   if (ret < 0){
          perror("Failed to read the message from the device.");
          return errno;
   }
   printf("Mouse Polling Rate Successfully Changed!\n");
   return 0;
}

