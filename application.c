// application.c

#include "application.h"
#include "user.h"
#include "request.h"
#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *DELIMITER = ";";

void start_application()
{
    /* Open user file in append mode and set 'user_fp' points to file's stream.*/
    user_fp = fopen(user_file_name, "a+");
    /* Open request file in append mode and set ''request_fp* points to file's stream.*/
    request_fp = fopen(request_file_name, "a+");

    // If pointer cannot be set then warn operator about it and quit program.
    if (user_fp == NULL || request_fp == NULL)
    {
        printf("Something went wrong, while opening database file.\n");
        printf("Please make sure that file %s resides in current directory. \n", user_file_name);
        getchar();
        return;
    }

    /* Fetch all users and setup array of all users */
    users = read_all_users_from_file(user_fp);

    /* Fetch all requests and setup array of all requests */
    requests = read_all_requests_from_file(request_fp);

    // Show welcome page
    welcome();
}

void welcome()
{
    system("cls");

    gotoxy(25, 5);
    printf("Welcome To Electricity Board\n");
    gotoxy(27, 23);
    printf("Press any key to continue: ");

    // Wait until user press some key.
    getchar();

    render_menu();
}
void render_menu()
{
    unsigned char choice = 0;
    char user_count_label[STR_MAX_LENGTH];

    system("cls");

    gotoxy(25, 5);
    printf("Select any option form below:");
    gotoxy(25, 7);
    printf("1. Add new user");
    gotoxy(25, 8);
    printf("2. View user");
    gotoxy(25, 9);
    printf("3. Add new connection request");
    gotoxy(25, 10);
    printf("4. Edit existing request");
    gotoxy(25, 11);
    printf("5. View Request");
    gotoxy(25, 12);
    printf("6. Exit");

    sprintf(user_count_label, "Total Users: %lu and Total Requests: %lu", number_of_users, number_of_requests);
    gotoxy(0, 24);
    printf("%s", user_count_label);

    gotoxy(30, 15);
    printf("Enter the choice: ");

    gotoxy(50, 15);
    scanf("%c", &choice);
    flush_buffer();

    switch (choice)
    {
    case '1':
        add_user_page();
        break;
    case '2':
        view_user_page();
        break;
    case '3':
        add_request_page();
        break;
    case '4':
        edit_request_page();
        break;
    case '5':
        view_request_page();
        break;
    case '6':
        close_application(EXIT_SUCCESS, "");
        break;
    default:
        close_application(EXIT_SUCCESS, "");
        break;
    }
}


void add_user_page() {
    /* Requests Id which will be stored in file.*/
    unsigned long user_id = last_user_id + 1; 

    /* User name. */
    char user_name[STR_MAX_LENGTH];

    /* User contact no. */
    char user_contact[ID_LENGTH + 2];

    /* User Id string, which will be shown in success dialogue box. */
    char user_id_string[STR_MAX_LENGTH];

    /* This variable will store user's data. */
    User user;

    /* User informationn in string, so it can be stored into file easily. */
    char * user_info_string = NULL;

    /* All labels which will be displayed on screen. */
    const char * enter_detail_label = "Enter details of user (All fields are required) ";
    const char * user_name_label = "Enter user name: ";
    const char * user_contact_label = "Enter user Contact: ";

    system("cls");

    gotoxy(0, 1);
    printf("%s", enter_detail_label);
    
    gotoxy(0, 3);
    printf("%s", user_name_label);
    
    gotoxy(0, 4);
    printf("%s", user_contact_label);

    /* Prepare input field for user name on row = 3, column = length(user_name_label). */
    gotoxy((int)strlen(user_name_label), 3);
    scanf("%[^\n]s", &user_name);
    // fgets(user_name, STR_MAX_LENGTH, stdin);
    // user_name[strcspn(user_name, "\n")] = '\0'; // Remove trailing newline

    flush_buffer();
    
    /* Prepare input field for user contact on row = 4, column = length(user_contact_label). */
    gotoxy((int)strlen(user_contact_label), 4);
    scanf("%s", &user_contact);
    flush_buffer();

    user.user_id = user_id;
    user.user_name = user_name;
    user.user_contact = user_contact;

    /* Delimited string representation of `user`, which will be directly stored in file.*/
    /* `user_info_string` is pointing to heap memory, so release it after work is done. */
    user_info_string = get_user_string(&user);

    /* Write new user at end of the file. */
    if (user_fp == NULL) {
        perror("Error opening user file");
        exit(EXIT_FAILURE);
    }
    
    fseek(user_fp, 0, SEEK_END);
    fputs(user_info_string, user_fp);
    fputs("\n", user_fp);

    /* Free memory created by `get_user_string`*/
    free(user_info_string);

    /* Free memory occupied by array of users and requests */
    release_requests();
    release_users();

    /* Refresh all users */
    users = read_all_users_from_file(user_fp);

    /* Refresh all requests*/
    requests = read_all_requests_from_file(request_fp);

    /* Prepare success dialogue */
    sprintf(user_id_string, "User Id: %lu", user_id);
    show_dialogue_box("User added successfully", user_id_string);
    render_menu();
}

void view_user_page() {
    /* User Id which operator wants to search */
    unsigned long user_id = 0;
    
    /* User details */
    User * user = NULL;

    /* All Request mode by above user */
    Request ** user_requests = NULL;

    /* Total count of all requests mode by above user */
    int user_requests_count = 0;

    /* Line offset will be helpful to print information at desired location. */
    int line_offset = 1;

    system("cls");

    printf("Enter user id: ");
    scanf("%lu", &user_id);
    flush_buffer();

    /* Get user details based on `user_id`*/
    user = get_user(user_id);

    /* If no such user found, then show error */
    if (user == NULL)
    {
        system("cls");
        show_dialogue_box("No such user exists.", "");
        render_menu();
        return;
    }

    system("cls");

    /* Show user details and requests made by him */

    /* Show request details on left side */
    printf("User Details\n");

    printf("\nUser Id: %lu", user->user_id);    
    printf("\nUser Name: %s", user->user_name);    
    printf("\nUser Contact: %s", user->user_contact);
    
    /* `user_requests` points to memory in heap, so release memory after work is done */
    user_requests = get_request_of_user(user_id, &user_requests_count);

    gotoxy(41, 1);

    if (user_requests == NULL)
    {
        printf("Request Details\n");
        gotoxy(41, 3);
        printf("No requests made by this user");
        gotoxy(0, 24);
        printf("Press any key to return to main menu");

        /* Wait for user to press some key. */
        getchar();
        render_menu();
        return;
    }
    /* Show request details on right side */
    printf("Request Details");

    for (int i = 0; i < user_requests_count; i++)
    {
        line_offset += 2;
        gotoxy(41, line_offset++);
        printf("Request Id: %lu", user_requests[i]->request_id);
        gotoxy(41, line_offset++);
        printf("Request Status: %s", get_request_status_string(user_requests[i]->status));
        gotoxy(41, line_offset++);
        printf("Comments: %s", user_requests[i]->comment);
        gotoxy(41, line_offset++);
        printf("Address: %s", user_requests[i]->address);
    }

    /* releasing heap memory pointed to by `user_requests` */
    free(user_requests);

    gotoxy(0, 24);
    printf("Press any key to return to main menu");

    /* Wait for user to press some key */
    getchar();
    render_menu();
    return;
}

void view_request_page() {

    /* Request Id which operator wants to search. */
    unsigned long request_id = 0; 

    /* Request Details */
    Request * request = NULL; 

    system("cls");

    printf("Enter request id: ");
    scanf("%lu", &request_id);
    flush_buffer();

    /* Get request details based on `request_id` */
    request = get_request(request_id);

    /* If no such request found, then show error */
    if (request == NULL)
    {
        system("cls");
        show_dialogue_box("No such request exists.", "Please contact adminstration. ");

        render_menu();
        return;
    }
    system("cls");
    
    /* Show Request details */
    printf("Request Details \n");

    printf("\nRequest Id: %lu", request->request_id);
    printf("\nRequested By: %s (id: %lu)", request->requested_by->user_name, request->requested_by->user_id);
    printf("\nRequest Status: %s", get_request_status_string(request->status));
    printf("\nAddress: %s", request->address);
    printf("\nComments: %s", request->comment);

    gotoxy(0, 24);

    printf("Press any key to return to main menu");

    /* Wait for user to press some key */
    getchar();

    render_menu();
}

void add_request_page() {
    /* Requests Id which will be stored in file. */
    unsigned long request_id = last_request_id + 1; 

    /* User id of user who made request */
    unsigned long user_id = 0; 

    /* Comment, if operator wants to enter one. */
    char comment[STR_MAX_LENGTH];

    /* Address of resource, where electricity lines to be installed. */
    char address[STR_MAX_LENGTH];

    /* Request Id string, which will be shown in success dialogu box. */
    char request_id_string[STR_MAX_LENGTH];

    /* User details. */
    User * user = NULL; 

    /* Request Details. */
    Request request; 

    /* Request information in string, so it can be stored into file easily.*/
    char * request_info_string = NULL; 

    /* All labels which will be displayed on screen. */
    const char * enter_detail_label = "Enter details of Request made by user: ";
    const char * user_id_label = "Enter user id: ";
    const char * request_comment_label = "Enter any comment (optional): ";
    const char * request_address_label = "Enter address: ";

    system("cls");

    gotoxy(0, 1);
    printf("%s", enter_detail_label);

    gotoxy(0, 3);
    printf("%s", user_id_label);

    gotoxy(0, 4);
    printf("%s", request_comment_label);

    gotoxy(0, 5);
    printf("%s", request_address_label);

    /* Prepare input field for username on row = 3, column = length(user_name_label). */

    gotoxy((int)strlen(user_id_label), 3);
    scanf("%lu", &user_id);
    flush_buffer();

    /* Prepare input field for user contact on row = 4, column = length(user_contact_label). */
    gotoxy((int)strlen(request_comment_label), 4);
    /* Using fgets because string can be empty. */
    fgets(comment, STR_MAX_LENGTH, stdin);

    /* If user press just enter, (i.e. empty data). */
    if (strcmp(comment, "\n") == 0)
    {
        strcpy(comment, "");
    } else {
        /* If operator has entered any string, it will contain '\n' as last character. */
        /* So replace that character will NULL */
        comment[strlen(comment) - 1] = '\0';
    }

    /* Prepare input field for user contact on row = 5, column = length(user_contact_label). */
    gotoxy((int)strlen(request_address_label), 5);
    scanf("%[^\n]s", address);
    flush_buffer();

    user = get_user(user_id);

    /* If cannot find user corresponding to `user_id` then simply redirect operator to main menu. */
    if (user == NULL)
    {
        system("cls");
        show_dialogue_box("No such user exists.", "Please contact adminstration.");
        render_menu();
    }

    request.request_id = request_id;
    request.requested_by = user; 
    request.status = PENDING; 
    request.comment = comment; 
    request.address = address; 

    /* Delimited string representation of `request`, which will be stored in file directly. */
    /* `request_info_string` is pointing to heap memory, so release it after work is done. */

    request_info_string = get_request_string(&request);

    /* Write new request to file. */

    fseek(request_fp, 0, SEEK_END);
    fputs(request_info_string, request_fp);
    fputs("\n", request_fp);

    /* Free memory created by `get_request_string` */
    free(request_info_string);

    /* Free memory occupied by array of users and requests */
    release_requests();
    release_users();

    /* Refresh all users */
    users = read_all_users_from_file(user_fp);

    /* Refresh all requests */
    requests = read_all_requests_from_file(request_fp);

    /* Prepare success dialogue */
    sprintf(request_id_string, "Request Id: %lu", request_id);
    show_dialogue_box("Request added successfully.", request_id_string);
    render_menu();
}

void edit_request_page() {
    /* Request id for request which user want to edit. */
    unsigned long request_id = 0; 

    /* New status for request. Defaults to `PENDING`. */
    request_status new_status = PENDING;

    /* New comments which user will enter. */
    char new_comment[STR_MAX_LENGTH];

    /* Request Details. */
    Request * request = NULL; 

    /* Labels which will be displayed on screen. */

    char * enter_new_status_label = "Enter request status: ";
    char * enter_new_status_suggestion_label = "( 1 -> In progress, 2 -> Rejected)";
    char * enter_new_comment_label = "Enter any comments: ";

    system("cls");
    printf("Enter Request Id: ");
    scanf("%lu", &request_id);
    flush_buffer();

    /* Fetch request from `requests` array. */
    request = get_request(request_id);

    /* If cannot find request corresponding to `request_id` then simply redirect operator to main menu*/
    if (request == NULL)
    {
        system("cls");
        show_dialogue_box("No such request exists.", "");
        render_menu();
        return;
    }

    /* Display request parameters on screen */
    printf("\nRequest Details\n");
    printf("\nRequest Id: %lu", request->request_id);
    printf("\nRequest By: %s (id : %lu)", request->requested_by->user_name, request->requested_by->user_id);
    printf("\nRequest Status: %s", get_request_status_string(request->status));
    printf("\nAddress: %s", request->address);
    printf("\nComments: %s", request->comment);

    printf("\n\n************************************************************************************");
    
    printf("\nEnter new Details\n\n");

    printf("%s", enter_new_status_label);
    printf("\n%s", enter_new_status_suggestion_label);

    gotoxy((int)strlen(enter_new_status_label), 14);

    scanf("%d", &new_status);
    flush_buffer();

    printf("\n%s", enter_new_comment_label);
    fgets(new_comment, STR_MAX_LENGTH, stdin);

    /* If user press just enter. (i.e. empty string) */
    if (strcmp(new_comment, "\n") == 0)
    {
        strcpy(new_comment, request->comment);
    } else {
        new_comment[strlen(new_comment) - 1] = '\0';
    }

    /* Store updated data in `request` */
    request->status = new_status; 

    /* Donot copy `new_comment` into `request->comment`, */
    /* that requires expansion in size of `request->comment`, */
    /* if `new_comment` is bigger than `request->comment` in length, */
    /* Just free memory created by `request->comment` (otherwise memory will leak.) and create new one*/

    free(request->comment);
    request->comment = strdup(new_comment);

    /* Replace Data in file */
    replace_request_in_file(request_fp, request);

    /* Free memory occupied by arary of users and requests. */
    release_requests();
    release_users();

    /* Refresh all users */
    users = read_all_users_from_file(user_fp);

    /* Refresh all requests */
    requests = read_all_requests_from_file(request_fp);

    show_dialogue_box("Request updated successfully.", "");
    render_menu();
}

void close_application(int status, const char * message) {
    fclose(user_fp);
    fclose(request_fp);
    release_users();
    release_requests();
    system("cls");
    printf("%s", message);
    exit(status);
}

void show_dialogue_box(const char * message, char * id) {
    int message_length = (int)strlen(message);

    const char * press_any_key = "Press any key to continue";
    int info_length = (int)strlen(press_any_key);

    int left_offset = 15; 
    int length = 50;
    int line_offset = 9; 

    system("cls");

    /* Line 1 */
    gotoxy(left_offset, line_offset++);
    printf("**************************************************");

    /* Line 2 */
    gotoxy(left_offset, line_offset);
    printf("*");

    gotoxy(left_offset + length - 1, line_offset++);
    printf("*");

    /* Line 3 */
    gotoxy(left_offset, line_offset);
    printf("*");
    gotoxy((80 - message_length)/2, line_offset);
    printf("%s", message);
    gotoxy(left_offset + length - 1, line_offset++);
    printf("*");

    if (strcmp(id, "") != 0)
    {
        /* Line 4 */
        gotoxy(left_offset, line_offset);
        printf("*");
        gotoxy((int)((80 - strlen(id))/2), line_offset);
        printf("%s", id);
        gotoxy(left_offset + length - 1, line_offset++);
        printf("*");
    }

    /* Line 5 */
    gotoxy(left_offset, line_offset);
    printf("*");
    gotoxy((int)((80 - info_length)/2), line_offset);
    printf("%s", press_any_key);
    gotoxy(left_offset + length - 1, line_offset++);
    printf("*");
    
    /* Line 6 */
    gotoxy(left_offset, line_offset);
    printf("*");
    gotoxy(left_offset + length - 1, line_offset++);
    printf("*");

    /* Line 7 */
    gotoxy(left_offset, line_offset++);
    printf("**************************************************");

    getchar();
}

void gotoxy(int x, int y) {
    printf("%c[%d;%dH", 0x1B, y, x); // 0x1B is the escape character
}