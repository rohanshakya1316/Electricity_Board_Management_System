// helpers.c 

#include "helpers.h"
#include "application.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

char * tokenize_string(char * source, char const * delimiter, char ** context){
    /* Example 'source' string is "A;B;C\n\0". */
    /* '\n' will be at end of the 'source' as we are reading from file. */

    if (source == NULL)
    {
        return NULL;
    }

    /* String after 1st 'delimiter' occurs will be stored in this variable (";B;C\0"). */
    char * rest_string = strpbrk(source, delimiter);

    /* Creating a copy of above variable, as it will get modified. */
    char copy_rest_string[STR_MAX_LENGTH];

    /* Token is string before 'delimiter' ("A\0") */
    char * token = NULL;

    /* 'rest_string' will be NULL when there is not any 'delimiter' (example string "C\n\0") */
    if (rest_string != NULL)
    {
        /* Make copy of 'rest_string' so 'copy_rest_string' will be ";B;C\n\0" */
        strcpy(copy_rest_string, rest_string);

        /* 1st character of 'rest_string' is our 'delimiter' so we need to replace it by "\0" */
        *rest_string = 0;

        /* With above operation 'source' string will also get modified. */
        /* and becomes "A\0B;C\n\0" i.e. "A\0" (string in C language is NULL terminated) */

        /* Create copy of modified 'source' string which "A\0" */
        /* As this copy is created with 'strdup', memory in heap will be created */
        /* which we will clear later. */

        token = strdup(source);

        /* '*context' is pointing to 'source' */
        /* i.e. we are clearing memory pointed to by 'source' */
        /* Otherwise memory will be leaked as, 'source' string is getting modified */

        free(*context);

        /* point '*context' (i.e. 'source') to copied string which is ";B;C\0" */
        /* Notice +1 here, this is because we want to skip 1st 'delimiter' which is at 1st 
             character of 'copy_rest_string' */
        /* Do not need worry about memory created by below operation */
        /* because in next iteration, it will be freed */

        *context = strdup(copy_rest_string + 1);
    } else
    {
        /* If we can not find any 'delimiter' then 'source' should be like "C\n\0" */
        /* So first replace last "\n" with "\0" */

        source[strlen(source) - 1] =  '\0';

        /* Create token from 'source' which is now "C\0\0" i.e. "C\0" */
        token = strdup(source);

        /* Free memory pointed to by '*context' */
        free(*context);

        /* And set '*context' i.e. 'source' to NULL so that we can break loop (exit condition) */
        *context = NULL; 
    }

    return token;
     
}

void flush_buffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);  // Clears the input buffer

    //while (getchar() != '\n');
}