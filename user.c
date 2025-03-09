/* user.c */
#include "user.h"
#include "helpers.h"
#include "application.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const char * user_file_name = "users.txt";

User ** users = NULL;

unsigned long last_user_id = 0;

FILE * user_fp = NULL;

unsigned long number_of_users = 0; 

User * get_user(unsigned long user_id) {
    for (int i = 0; i < number_of_users; i++)
    {
        if (users[i]->user_id == user_id)
        {
            return users[i];
        }
    }
    return NULL;
};

char * get_user_string(User * user_info){
    char user_id_string[ID_LENGTH];

    char * user_info_string = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);

    /* Convert user id into string */
    sprintf(user_id_string, "%lu", user_info->user_id);

    /* Conversion of multiple strings into single delimited string. */
    strcpy(user_info_string, strcat(user_id_string, DELIMITER));
    user_info_string = strcat(user_info_string, user_info->user_name);
    user_info_string = strcat(user_info_string, DELIMITER);
    user_info_string = strcat(user_info_string, user_info->user_contact);

    /* We need to release memory created by `user_info_string` */
    return user_info_string;
};

User * get_user_info(char * user_info) {
    char * token = NULL;

    /* Set `COLUMN_TAG` to 0 so that we can read new values. */
    /* `COLUMN_TAG will help us to track which column we are going to read. `*/

    int COLUMN_TAG = 0;

    /* Keep copy of original `user_info` because this string will be modified */

    char * user_info_copy = strdup(user_info);

    User * read_user = (User *)malloc(sizeof(User));

    while ((token = tokenize_string(user_info_copy, DELIMITER, &user_info_copy)) != NULL)
    {
        switch (COLUMN_TAG)
        {
        case 0:
            read_user->user_id = atoi(token);
            COLUMN_TAG++;
            free(token);
            break;
        
        case 1:
            read_user->user_name = strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        
        case 2:
            read_user->user_contact = strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        default:
            close_application(EXIT_FAILURE, "Error while parsing user database...");
            break;
        }
    }

    free(user_info_copy);
    
    /* Check if we have read anything or not. */
    if (COLUMN_TAG != 3)
    {
        /* If not then just release memory created by `read_user`. */
        free(read_user);
    }
    return read_user;
}

User ** read_all_users_from_file(FILE * fp) {

    /* All users (pointer to `User`) will be stored in this array. */
    User ** all_users = NULL;

    /* Read each line from file and prepare user details. */
    User * read_user = NULL;

    /* Total number of users */
    unsigned long user_count = 0; 

    /* Read line from file and store in this variable. */
    char * user_info = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);

    /* Set file pointer to beginning of the file. */
    rewind(fp);

    /* Start reading file. */

    while (fgets(user_info, STR_MAX_LENGTH, fp) != NULL)
    {
        if (strcmp(user_info, "\n") == 0)
        {
            continue;
        }
        /* Get User structure from string, which we have read from file. */

        /* We do not need to release memory occupied by `read_user` because, */
        /* We are going to store address of this variable into `all_users` array, */
        /* which will be realeased at end of the program */

        read_user = get_user_info(user_info);

        /* If we cannot create user successfully, then just continue to next cycle. */
        if (read_user == NULL)
        {
            continue;
        }

        /* Allocate memory for `all_users` array. */
        all_users = (User **)realloc(all_users, sizeof(User *) * (1 + user_count));

        /* store address of user into `all_users` array. */
        all_users[user_count++] = read_user;
    }

    /* Release memory occupied by `user_info` */
    free(user_info);

    /* Keep track of number of users in global variable. */
    last_user_id = number_of_users = user_count;

    return all_users;
}

void replace_user_in_file(FILE * fp, User * updated_user) {
    User * read_user = NULL;

    /* Below function can leak memory, so clear memory before returning from this function. */
    char * updated_user_string = get_user_string(updated_user);

    char line[STR_MAX_LENGTH];

    /* Open temp file in write mode. */
    FILE * write_fp = fopen("temp.txt", "w");

    /* Set file pointer to the beginning of file. */
    rewind(fp);
    while (fgets(line, STR_MAX_LENGTH, fp) != NULL)
    {
        /* Create user from string, which we have read from file. */
        /* Memory created by below function will be in heaap. */
        /* So release that memory before loop ends. */

        if ((read_user = get_user_info(line)) == NULL)
        {
            continue;
        }

        if (read_user->user_id == updated_user->user_id)
        {
            /* If user id matches, then replace line with `updated_user_string` */
            fputs(updated_user_string, write_fp);

            /* As `updated_user_string` does not contain "\n", add one manually. */
            fputs("\n", write_fp);
        } else {
            /* Write `line` as it is in file. */
            /* Do not need to put extra "\n", as 'line' already contains "\n". */
            fputs(line, write_fp);
        }
        free(read_user);
    }

    /* Close temp file. */
    fclose(write_fp);

    /* Close original file. */
    fclose(fp);

    /* Remove original file from disk. */
    remove(user_file_name);

    /* Rename temporary file with original file name. */
    rename("temp.txt", user_file_name);

    /* Open that file in `a+` mode so we have reached state here we left off. */
    user_fp = fopen(user_file_name, "a+");

    free(updated_user_string);
}

void release_users() {
    if (!users)
    {
        return;
    }
    for (int i = 0; i < number_of_users; i++)
    {
        free(users[i]->user_name);
        free(users[i]->user_contact);
    }
    free(users);
    users = NULL;
}

