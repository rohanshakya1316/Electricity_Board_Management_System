// application.h

// we are using #ifndef macro to prevent multiple inclusion of header files

#ifndef APPLICATION_H
#define APPLICATION_H

// Maximum length of user id and request id

#define ID_LENGTH 10

// Maximum length of string used in the project
// User name, comment, address, etc.

#define STR_MAX_LENGTH 256

// All application and user interface related functions. 
// String which will be used to separate data of each line in given file
extern const char * DELIMITER;

// Start application
extern void start_application(void);

// Show welcome screen of application.
extern void welcome(void);

// SHow menu for operator with various options.
extern void render_menu(void);

// Add new user who want to request for electricity. 
extern void add_user_page(void);

// Operator can view specific use and requests done by that user. 
extern void view_user_page(void);

// Operator can add new request which is requested by specific user. 
extern void add_request_page(void);

// In future, if operator wants to edit request (i.e., change status or enter comments).
extern void edit_request_page(void);

// If operator wants to view specific request.
extern void view_request_page(void);

/*
    Close application after operator completes his work
    input: status of application
    input: message to display on the screen in case of erroneous exit.
*/
extern void close_application(int exit_status, const char * message);

/*
    Function which set cursor at X, Y on display
    input: X Coordinate of screen 
    input: Y Coordinate of screen
*/
extern void gotoxy(int x, int y);

/*
    Function which display message and additional info on screen 
    input: message to be displayed. 
    input: additional information along with message.
*/
extern void show_dialogue_box(const char * message, char * info);

#endif // application.h
