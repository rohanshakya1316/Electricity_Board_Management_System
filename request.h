/* request.h */

/* We are using #ifndef macro to prevent multiple inclusion of header files. */

#ifndef REQUEST_H
#define REQUEST_H

#include "user.h"
#include<stdio.h>

/* Type of request status can be `Pending`, `In Process` or `Rejected` */

typedef enum request_status {
    PENDING, 
    IN_PROCESS,
    REJECTED
} request_status;

/* Function will return string based on `request_status` */

char * get_request_status_string(request_status);

/* Structure which stores data of user */
typedef struct Request
{
    /* Unique identifier associated with every request */
    unsigned long request_id; 

    /* User by which connection request was created */
    User * requested_by;

    /* Current status of request */
    request_status status; 

    /* Comments on request. If operator decides to reject this request. */
    /* he can optionally add reason for rejection in comments. */

    char * comment; 

    /* Address at which electricity needs to be installed. */
    char * address;

} Request;

/* Request file name in which data of all requests will be stored. */
extern const char * request_file_name;

/* Global variable which contains data of all requests. */
extern Request ** requests; 

/* Global variable which stores total count of all requests. */
extern unsigned long number_of_requests; 

/* Global File pointers, so we can access it in any of the function */
extern FILE * request_fp; 

/* Variable which keeps track of last inserted request id. */
extern unsigned long last_request_id; 

/* 
    Function which give `Request` based on their `request_id`. 
    input: request id.
    output: Pointer to `User` which contains request information in structure format. 
*/

extern Request * get_request(unsigned long request_id);

/* 
    Function which converts `Request` data into string, so that we can store this output in file. 
    input: Request information in structure format. 
    output: comma separated string representatiaon of `request_info`. 
*/
char * get_request_string(Request * request);

/*  
    Function which converts string (which we read from input file) into `Request` data. 
    input: comma seperated string which contains request information. 
    output: Pointer to `Request` which contains request information in structure format. 
*/
Request * get_request_info(char * get_request_info_string);

/*
    Funciton which reads all lines from files, converts and returns array of all requests.
    input: FILE pointer of user file. 
    output: Pointer to array of all requests. 
*/
Request ** read_all_requests_from_file(FILE * fp);

/*
    Function which reaplce specific request of file. 
    input: FILE pointer of request file. 
    output: Request to be replaced. 
*/
void replace_request_in_file(FILE * fp, Request * request);

/* 
    Function which returns all Request details, for given user id.
    input: user id.
    input: address of variable which contains number of request. 
    output: Pointer to array of all requests of specified user. 
*/
Request ** get_request_of_user(unsigned long user_id, int * request_count);

/* Function which releases memory created by requests (in heap). */

void release_requests(void);

#endif /* request.h */