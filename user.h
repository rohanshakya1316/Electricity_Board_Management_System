// user.h 

/* We are using #ifndef macro to prevent multiple inclusion of header files. */

#ifndef USER_H
#define USER_H

#include <stdio.h>

/* Structure which stores data of user. */

typedef struct User {

    /* Unique identifier associated with every user. */
    unsigned long user_id; 

    /* User name which is of type stirng aka 'char *'. */
    char * user_name; 

    /* User contact no. is of type string aka 'char *'. */
    char * user_contact;
} User;

/* User file name in which all user's data will be stored. */
extern const char * user_file_name; 

/* Global variable which contains all user's data in memory. */
extern User ** users; 

/* Global vairable which stores total count of users. */
extern unsigned long number_of_users; 

/* Global File pointer, so we can access user file in any function. */
extern FILE * user_fp; 

/* Variable which keeps track of last inserted user id. */
extern unsigned long last_user_id; 

/* 
    Function which give 'User' based on their 'user_id'. 
    input : user id.
    output: Pointer to 'User' which contains user's information in structure format. 
*/

extern User * get_user (unsigned long user_id);

/* 
    Function which converts 'User' data into string, so that we can sotre this o/p in file. 
    input: User information in structure format. 
    output: comma separated string representation of 'user_info'. 
*/

char * get_user_string (User * user);

/*
    Function which converts string (which we read from i/p file) into 'User' data. 
    input: comma separated string which contains user's information. 
    output: Pointer to 'User' which contains all user's information in structure format. 
*/

User * get_user_info (char * user_info);

/*
    Function which reads all lines from file, converts and returns array of all users. 
    input: FILE pointer of user file.
    output: Pointer to array of all users.
*/

User ** read_all_users_from_file (FILE *fp);

/*
    Function which replace specific user in file. 
    input: FILE pointer of user file. 
    input: User to be replaced. 
*/

void replace_user_in_file (FILE * fp, User * user);

/* Function which releases memory created by users (in heap). */

void release_users (void);

#endif // user_h