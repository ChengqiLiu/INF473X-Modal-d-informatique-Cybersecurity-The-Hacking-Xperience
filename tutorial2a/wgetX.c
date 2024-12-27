/**
 *  Jiazi Yi
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 *
 * Updated by Pierre Pfister
 * Cisco Systems
 * ppfister@cisco.com
 *
 * Updated by Kevin Jiokeng
 * LIX, Ecole Polytechnique
 * kevin.jiokeng@polytechnique.edu
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "url.h"
#include "wgetX.h"

int main(int argc, char* argv[]) {
    url_info info;
    const char * file_name = "received_page";
    if (argc < 2) {
        fprintf(stderr, "Missing argument. Please enter URL.\n");
        return 1;
    }

    char *url = argv[1];

    // Get optional file name
    if (argc > 2) {
        file_name = argv[2];
    }

    // First parse the URL
    int ret = parse_url(url, &info);
    if (ret) {
        fprintf(stderr, "Could not parse URL '%s': %s\n", url, parse_url_errstr[ret]);
        return 2;
    }

    //If needed for debug
    //print_url_info(&info);

    // Download the page
    struct http_reply reply;

    ret = download_page(&info, &reply);
    if (ret) {
        return 3;
    }

    // Now parse the responses
    int length;
    char *response = read_http_reply(&reply, &length);
    if (response == NULL) {
        fprintf(stderr, "Could not parse http reply\n");
        return 4;
    }

    // Write response to a file
    write_data(file_name, response, length);

    // Free allocated memory
    free(reply.reply_buffer);

    // Just tell the user where is the file
    fprintf(stderr, "the file is saved in %s.", file_name);
    return 0;
}

int receive(int socket_fd, http_reply *reply){
    int total_len=0;
    ssize_t len=recv(socket_fd, reply->reply_buffer, reply->reply_buffer_length, 0);
    while (len>0) {

        if (total_len+len>=reply->reply_buffer_length){
            //realloc
            char *new_buff=(char*)realloc(reply->reply_buffer,reply->reply_buffer_length+SIZE);

            if(!new_buff){
                //fail to realloc
                free(reply->reply_buffer);
                free(reply);
                fprintf(stderr, "Could not allocate memory %s\n", strerror(errno));
                return errno;
            }

            //update reply buffer
            reply->reply_buffer=new_buff;
            reply->reply_buffer_length+=SIZE;
        }

        //receive
        total_len+=len;
        len=recv(socket_fd, reply->reply_buffer+total_len, reply->reply_buffer_length-total_len, 0);
    }
    if (len<0){
        //receive error
        fprintf(stderr, "recv returned error: %s\n", strerror(errno));
        return errno;
    }
    return 0;
}

int download_page(url_info *info, http_reply *reply) {

    /*
     * To be completed:
     *   You will first need to resolve the hostname into an IP address.
     *
     *   Option 1: Simplistic
     *     Use gethostbyname function.
     *
     *   Option 2: Challenge
     *     Use getaddrinfo and implement a function that works for both IPv4 and IPv6.
     *
     */
    struct hostent *host_p;
    host_p=gethostbyname(info->host);

    //buffer to store new url in 301 situation
    http_reply url_buff;
    url_buff.reply_buffer=(char*)malloc(SIZE*sizeof(char *));
    url_buff.reply_buffer_length=SIZE;
    int i; //number of redirections
    for(i=0; i<MAX_REDIRECTION;++i){
        /*
        * To be completed:
        *   Next, you will need to send the HTTP request.
        *   Use the http_get_request function given to you below.
        *   It uses malloc to allocate memory, and snprintf to format the request as a string.
        *
        *   Use 'write' function to send the request into the socket.
        *
        *   Note: You do not need to send the end-of-string \0 character.
        *   Note2: It is good practice to test if the function returned an error or not.
        *   Note3: Call the shutdown function with SHUT_WR flag after sending the request
        *          to inform the server you have nothing left to send.
        *   Note4: Free the request buffer returned by http_get_request by calling the 'free' function.
        *
        */
        //initialize request buffer
        char *request_buff=http_get_request(info);
        if (!request_buff){
            fprintf(stderr, "Could not allocate memory %s\n", strerror(errno));
            return errno;
        }

        //create socket
        int socket_fd=0;
        socket_fd=socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd==-1){
            fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
            return errno;
        }

        //define destination address
        struct sockaddr_in dest;
        dest.sin_family=AF_INET;
        dest.sin_addr.s_addr=*(in_addr_t*)host_p->h_addr_list[0];
        dest.sin_port = htons(info->port);

        //connect
        if (connect(socket_fd, (struct sockaddr *)&dest, sizeof(struct sockaddr))) {
            fprintf(stderr, "Could not connect: %s\n", strerror(errno));
            return errno;
        }

        //write
        if (write(socket_fd,request_buff,strlen(request_buff))==-1){
            fprintf(stderr, "Could not write: %s\n", strerror(errno));
            return errno;
        }

        //shutdown
        if (shutdown(socket_fd,SHUT_WR)==-1){
            fprintf(stderr, "Could not shutdown %s\n", strerror(errno));
            return errno;
        }

        free(request_buff);


        /*
        * To be completed:
        *   Now you will need to read the response from the server.
        *   The response must be stored in a buffer allocated with malloc, and its address must be save in reply->reply_buffer.
        *   The length of the reply (not the length of the buffer), must be saved in reply->reply_buffer_length.
        *
        *   Important: calling recv only once might only give you a fragment of the response.
        *              in order to support large file transfers, you have to keep calling 'recv' until it returns 0.
        *
        *   Option 1: Simplistic
        *     Only call recv once and give up on receiving large files.
        *     BUT: Your program must still be able to store the beginning of the file and
        *          display an error message stating the response was truncated, if it was.
        *
        *   Option 2: Challenge
        *     Do it the proper way by calling recv multiple times.
        *     Whenever the allocated reply->reply_buffer is not large enough, use realloc to increase its size:
        *        reply->reply_buffer = realloc(reply->reply_buffer, new_size);
        *
        *
        */
        //initialize reply buffer
        reply->reply_buffer=(char *)realloc(reply->reply_buffer,SIZE*sizeof(char *));
        reply->reply_buffer_length=SIZE;

        //receive
        int ret=receive(socket_fd,reply);
        if(ret !=0){
            //error in receiving
            return ret;
        }

        close(socket_fd);

        //read the status
        int status;
        double http_version;
        int rv = sscanf(reply->reply_buffer, "HTTP/%lf %d", &http_version, &status);
        if (rv != 2) {
            fprintf(stderr, "Could not parse http response first line (rv=%d, %s)\n", rv, reply->reply_buffer);
            return errno;
        }
        if(status!=301){
            //201 (normal) or other errors
            break;
        }

        //get new host by name
        char *ptr_start=strstr(reply->reply_buffer,"Location: ");
        char *ptr_end=strstr(reply->reply_buffer,"Content-Length: ");
        int hostname_len=ptr_end-ptr_start-strlen("Location: ")-2;

        //realloc if necessary
        if(url_buff.reply_buffer_length<hostname_len+1){
            char *new_buff=(char*)realloc(url_buff.reply_buffer,hostname_len+1);

            if(!new_buff){
                //fail to realloc
                free(url_buff.reply_buffer);
                fprintf(stderr, "Could not allocate memory %s\n", strerror(errno));
                return errno;
            }

            url_buff.reply_buffer=new_buff;
            url_buff.reply_buffer_length=hostname_len+1;
        }
        
        strncpy(url_buff.reply_buffer,ptr_start+strlen("Location: "),hostname_len);

        //parse the url
        ret = parse_url(url_buff.reply_buffer, info);
        if (ret) {
            fprintf(stderr, "Could not parse URL '%s': %s\n", url_buff.reply_buffer, parse_url_errstr[ret]);
            return errno;
        }
        host_p=gethostbyname(info->host);
    }
    if(i==MAX_REDIRECTION-1){
        //ERR_TOO_MANY_REDIRECT
        fprintf(stderr, "Too many redirections '%s'\n", url_buff.reply_buffer);
        return errno;
    }

    free(url_buff.reply_buffer);
    return 0;
}

void write_data(const char *path, const char * data, int len) {
    /*
     * To be completed:
     *   Use fopen, fwrite and fclose functions.
     */
    FILE *fp;
    fp=fopen(path, "w");
    if(!fp){
        fprintf(stderr, "Fail to open the folder '%s'\n",path);
        return;
    }
    size_t size=fwrite(data,sizeof(char),len,fp);
    if(size!=len){
        fprintf(stderr, "Fail to write the folder '%s'\n",path);
        return;
    }
    int ret=fclose(fp);
    if (ret==EOF){
        fprintf(stderr, "Fail to close the folder '%s'\n",path);
        return;
    }
}

char* http_get_request(url_info *info) {
    char * request_buffer = (char *) malloc(100 + strlen(info->path) + strlen(info->host));  
    snprintf(request_buffer, 1024, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
	    info->path, info->host);
    return request_buffer;
}

char *next_line(char *buff, int *left_len) {
    if (*left_len == 0) {
	    return NULL;
    }

    char *last = buff + *left_len - 1;
    while (buff != last) {
        if (*buff == '\r' && *(buff+1) == '\n') {
            return buff;
        }
        buff++;
        //update left length to facilitate multiple calls
        (*left_len)--; 
    }
    return NULL;
}

char *read_http_reply(struct http_reply *reply, int* length) {  
    int left_len=reply->reply_buffer_length;

    // Let's first isolate the first line of the reply
    char *status_line = next_line(reply->reply_buffer, &left_len);
    if (status_line == NULL) {
        fprintf(stderr, "Could not find status\n");
        return NULL;
    }
    *status_line = '\0'; // Make the first line is a null-terminated string

    // Now let's read the status (parsing the first line)
    int status;
    double http_version;
    int rv = sscanf(reply->reply_buffer, "HTTP/%lf %d", &http_version, &status);
    if (rv != 2) {
        fprintf(stderr, "Could not parse http response first line (rv=%d, %s)\n", rv, reply->reply_buffer);
        return NULL;
    }

    if (status != 200) {
        fprintf(stderr, "Server returned status %d (should be 200)\n", status);
        return NULL;
    }

    char *buf = status_line + 2;

    /*
     * To be completed:
     *   The previous code only detects and parses the first line of the reply.
     *   But servers typically send additional header lines:
     *     Date: Mon, 05 Aug 2019 12:54:36 GMT<CR><LF>
     *     Content-type: text/css<CR><LF>
     *     Content-Length: 684<CR><LF>
     *     Last-Modified: Mon, 03 Jun 2019 22:46:31 GMT<CR><LF>
     *     <CR><LF>
     *
     *   Keep calling next_line until you read an empty line, and return only what remains (without the empty line).
     *
     *   Difficul challenge:
     *     If you feel like having a real challenge, go on and implement HTTP redirect support for your client.
     *
     */
    while(1){
        status_line=next_line(status_line, &left_len);

        if(status_line==NULL){
            //error
            fprintf(stderr, "Could not find status\n");
            return NULL;
        }

        if (*(status_line+2) == '\r' && *(status_line+3) == '\n'){
            //empty line checked
            break;
        }
        status_line+=1;
    }
     
    //buf stores the remaining contents
    buf=status_line+4;
    

    //delete the large number of '\0'  at the end
    for(char* ptr=buf+(left_len-1);ptr!=buf;--ptr){
        if(*ptr=='\0'){
            --left_len;
        }else{
            break;
        }
    }

    *length=left_len;
    return buf;
}
