#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fancy-hello-world.h"
#define SIZE 64 //input buffer size

int main(void) {
    char* name=NULL;
    char* output=NULL;
    hello_string(name,output); 
    return 0;
}

void hello_string(char* name, char* output) {
    //Input buffer
    char buffer[SIZE];

    //Input
    unsigned int i=1; //rounds of using buffer
    printf("Please enter your name:\n");
    while(fgets(buffer,SIZE,stdin)!=NULL){
        if(i==1){
            //Allocate space for input name.
            name=(char*)malloc(i*SIZE*sizeof(char));
            name[0]='\0';
        }
        else{
            //Store the name of the last round into old_name.
            char* old_name=(char*)malloc((i-1)*SIZE*sizeof(char));
            strncpy(old_name,name,(i-1)*SIZE*sizeof(char));
            //Reallocate larger space for input name.
            free(name);
            name=(char*)malloc(i*SIZE*sizeof(char));
            name[0]='\0';
            strcat(name,old_name);
            free(old_name);
        }

        //name += buffer.     
        strcat(name,buffer);
        ++i;

        //Break after read to '\n'.
        unsigned int input_length=strlen(buffer);
        if(buffer[input_length-1]=='\n'){
            break;
        }
    }
    //Output. output= "Hello World, hello "+name.
    unsigned int length=strlen(name);
    output=(char *)malloc((length+SIZE)*sizeof(char));
    output[0]='\0';
    strcat(output,"Hello World, hello ");
    strcat(output,name);
    printf("%s", output);

    free(name);
    free(output);
    return;
}