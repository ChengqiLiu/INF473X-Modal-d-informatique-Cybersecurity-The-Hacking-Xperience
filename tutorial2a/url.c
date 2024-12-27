/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 *
 * Updated by Pierre Pfister
 *
 * Cisco Systems
 * ppfister@cisco.com
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"url.h"

/**
 * parse a URL and store the information in info.
 * return 0 on success, or an integer on failure.
 *
 * Note that this function will heavily modify the 'url' string
 * by adding end-of-string delimiters '\0' into it, hence chunking it into smaller pieces.
 * In general this would be avoided, but it will be simpler this way.
 */

int parse_url(char* url, url_info *info)
{
	// url format: [http://]<hostname>[:<port>]/<path>
	// e.g. https://www.polytechnique.edu:80/index.php

	char *column_slash_slash, *host_name_path, *protocol;

	column_slash_slash = strstr(url, "://");
	// it searches :// in the string. if exists,
	//return the pointer to the first char of the match string

	if(column_slash_slash != NULL){ //protocol type exists
		*column_slash_slash = '\0'; //end of the protocol type
		host_name_path = column_slash_slash + 3; //jump to the host name
		protocol = url;
	} else {	//no protocol type: using http by default.
		host_name_path = url;
		protocol = "http";
	}

	/*
	 * To be completed:
	 * 	 store info->protocol
	 */

	info->protocol = protocol;

	/*
     * To be completed:
	 * 	 Return an error (PARSE_URL_PROTOCOL_UNKNOWN) if the protocol is not 'http' using strcmp.
	 */

	if (strcmp(info->protocol, "http")) {
		fprintf(stderr, "%s\n", parse_url_errstr[PARSE_URL_PROTOCOL_UNKNOWN]);
		return PARSE_URL_PROTOCOL_UNKNOWN;
	}

	/*
	 * To be completed:
	 * 	 Look for the first '/' in host_name_path using strchr
	 * 	 Return an error (PARSE_URL_NO_SLASH) if there is no such '/'
	 * 	 Store the hostname and the queried path in the structure.
	 *
	 * 	 Note: The path is stored WITHOUT the first '/'.
	 * 	       It simplifies this function, but I'll let you understand how. :-)
	 */

	char *slash = strchr(host_name_path, '/');
	if (slash == NULL) {
		return PARSE_URL_NO_SLASH;
	}
	*slash = '\0';
	info->host = host_name_path;
	info->path = slash + 1;

	/*
	 * To be completed:
	 * 	 Look for ':' in the hostname
	 * 	 If ':' is not found, the port is 80 (store it)
	 * 	 If ':' is found, split the string and use sscanf to parse the port.
	 * 	 Return an error if the port is not a number, and store it otherwise.
	 */

	char *port = strchr(host_name_path, ':');
	if (port) {
		*port = '\0';
		port = port + 1;
		if (sscanf(port, "%d", &info->port) != 1) {
			return PARSE_URL_INVALID_PORT;
		}
	} else {
		info->port = 80;
	}

	// If everything went well, return 0.
	return 0;
}

/**
 * print the url info to std output
 */
void print_url_info(url_info *info){
	printf("The URL contains following information: \n");
	printf("Protocol:\t%s\n", info->protocol);
	printf("Host name:\t%s\n", info->host);
	printf("Port No.:\t%d\n", info->port);
	printf("Path:\t\t/%s\n", info->path);
}
