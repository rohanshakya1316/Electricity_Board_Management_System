/* request.c */

#include "request.h"
#include "helpers.h"
#include "application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Request ** requests = NULL;

const char * request_file_name = "requests.txt";

unsigned long number_of_requests = 0; 

FILE * request_fp = NULL; 

unsigned long last_request_id = 0; 

Request * get_request(unsigned long request_id) {
    for (int i = 0; i < number_of_requests; i++)
    {
        if (requests[i]->request_id == request_id)
        {
            return requests[i];
        }
    }
    return NULL;
}

char * get_request_string(Request * request_info) {
    char request_id_string[ID_LENGTH], user_id_string[ID_LENGTH], request_status[2];

    char * request_info_string = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);

    if (request_info->requested_by == NULL)
    {
        /* If we cannot get who has made request, then just show message and quit program. */
        close_application(EXIT_FAILURE, "Some request does not have user.");
    }

    /* sprintf will be used to convert integer into string. */
    sprintf(request_id_string, "%lu", request_info->request_id);
    sprintf(user_id_string, "%lu", request_info->requested_by->user_id);
    sprintf(request_status, "%d", request_info->status);

    /* Conversion of multiple strings into single delimited string. */

    strcpy(request_info_string, strcat(request_id_string, DELIMITER));
    request_info_string = strcat(request_info_string, user_id_string);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_status);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_info->comment);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_info->address);
    
    /* We need to release memory created ny `user_info_string` */
    return request_info_string;
}

Request * get_request_info(char * request_info) {
    char * token = NULL; 

    /* Set `COLUMN_TAG` to 0 so that we can read new values. */
    /* `COLUMN_TAG` will help us to track, which column we are going to read. */
    int COLUMN_TAG = 0; 

    /* Keep copy of original `request_info` because this string will be modified. */
    /* Need to free memory pointed to by `request_info_copy`. */
    char * request_info_copy = strdup(request_info);

    Request * read_request = (Request *)malloc(sizeof(Request));

    User * user = NULL; 

    while ((token = tokenize_string(request_info_copy, DELIMITER, &request_info_copy)) != NULL)
    {
        switch (COLUMN_TAG)
        {
        case 0:
            read_request->request_id = atoi(token);
            COLUMN_TAG++;
            free(token);
            break;
        
        case 1:
            user = get_user(atoi(token));
            free(token);
            /* It is necessay, that user mentioned by request does exists. */
            if (!user)
            {
                free(read_request);
                free(request_info_copy);
                return NULL;
            }
            read_request->requested_by = user; 
            COLUMN_TAG++;
            break;
        
        case 2:
            read_request->status = atoi(token);
            COLUMN_TAG++;
            free(token);
            break;
        
        case 3:
            read_request->comment= strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        
        case 4:
            read_request->address = strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        default:
            close_application(EXIT_FAILURE, "Error while parsing user database...");
            break;
        }
    }

    free(request_info_copy);

    /* Check if we have read everything or not. */

    if (COLUMN_TAG != 5)
    {
        /* If not then just release memory created by `read_request` */
        free(read_request);
    }

    return read_request;
}

Request ** read_all_requests_from_file(FILE * fp) {
    /* All requests (pointer to `Request`) will be stored in this array */
    Request ** all_requests = NULL; 

    /* Read each line from file and prepare request detail. */
    Request * read_request = NULL; 

    /* Total number of requests. */
    unsigned long request_count = 0; 

    /* Read line from file and store in this variable. */
    char * request_info = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);

    /* Set file pointer to beginning of the file. */
    rewind(fp); 

    /* Start reading file. */

    while (fgets(request_info, STR_MAX_LENGTH, fp) != NULL)
    {
        if (strcmp(request_info, "\n") == 0)
        {
            continue;
        }

        /* Get Request structure from string, which we have read from file. */

        /* We do not need to release memory occupied by `read_request` because, */
        /* We are going to store address of this variable into `all_requests` array, */
        /* which will be realeased at end of the program */

        read_request = get_request_info(request_info);

        /* If we cannot create request successfully, then just continue to next cycle. */
        if (read_request == NULL)
        {
            continue;
        }

        /* Allocate memory for `all_requests` array. */
        all_requests = (Request **)realloc(all_requests, sizeof(Request *) * (1 + request_count));

        /* Store address of request into `all_requests` array. */
        all_requests[request_count++] = read_request; 
    }
    
    /* Release memory occupied by `request_info`. */
    free(request_info);

    /* Keep track of number of requests in global variable. */
    last_request_id = number_of_requests = request_count;

    return all_requests;
}

Request ** get_request_of_user(unsigned long user_id, int * request_count) {
    Request ** user_requests = NULL;
    int count = 0; 

    for (int i = 0; i < number_of_requests; i++)
    {
        if (requests[i]->requested_by->user_id == user_id)
        {
            user_requests = (Request **)realloc(user_requests, sizeof(Request *) * (1 + count)); 
            user_requests[count++] =requests[i];
        }
    }

    /* Modify count. */
    *request_count = count;

    return user_requests;   
}

void replace_request_in_file(FILE * fp, Request * updated_request) {
    Request * read_request = NULL; 

    /* Below function can leak memory, so clear memory before returning from this function. */
    char * updated_request_string = get_request_string(updated_request);

    char line[STR_MAX_LENGTH];

    /* Open temp file in write mode. */
    FILE * write_fp = fopen("temp.txt", "w");

    /* Set file pointer to the beginning of file. */
    rewind(fp);
    while (fgets(line, STR_MAX_LENGTH, fp) != NULL)
    {
        /* Create request from string, which we have read from file. */
        /* Memory created by below function will be in heaap. */
        /* So release that memory before loop ends. */

        if ((read_request = get_request_info(line)) == NULL)
        {
            continue;
        }

        if (read_request->request_id == updated_request->request_id)
        {
            /* If request id matches, then replace line with `updated_request_string` */
            fputs(updated_request_string, write_fp);

            /* As `updated_request_string` does not contain "\n", add one manually. */
            fputs("\n", write_fp);
        } else {
            /* Write `line` as it is in file. */
            /* Do not need to put extra "\n", as 'line' already contains "\n". */
            fputs(line, write_fp);
        }
        free(read_request);
    }

    /* Close temp file. */
    fclose(write_fp);

    /* Close original file. */
    fclose(fp);

    /* Remove original file from disk. */
    remove(request_file_name);

    /* Rename temporary file with original file name. */
    rename("temp.txt", request_file_name);

    /* Open that file in `a+` mode so we have reached state here we left off. */
    request_fp = fopen(request_file_name, "a+");

    free(updated_request_string);
}

char * get_request_status_string(request_status status) {
    switch (status)
    {
    case PENDING:
        return "Pending";
        break;
    
    case IN_PROCESS:
        return "In Process";
        break;
    
    case REJECTED:
        return "Rejected";
        break;
    default:
        break;
    }

    return "Pending";
}

void release_requests() {
    if (!requests)
    {
        return;
    }

    for (int i = 0; i < number_of_requests; i++)
    {
        free(requests[i]->comment);
        free(requests[i]->address);
        requests[i]->requested_by = NULL;
    }
    
    free(requests);
    requests = NULL; 
}
