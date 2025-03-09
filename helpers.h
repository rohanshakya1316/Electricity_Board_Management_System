// helpers.h

#ifndef HELPERS_H
#define HELPERS_H

/*
    Flush buffer after user enter some input. 
    If we won't flush buffer then 'enter' presssed by user will be stored in buffer,
    and it will be used as an input to next I/O function, which may cause issue.
*/

extern void flush_buffer(void);

/* 
    Function which returns single string delimited by 'delimiter' from 'source'. 
    Also we want to modify 'source' after reading token so we have used 'context' (char **),
    which is pointer to 'source', which will modify input 'source'
    input: Delimited string,
    input: Delimiter by which string is separated. 
    input: context i.e. address of 'string' (char *), so we have used char ** to catch
            address of string.
    output: Single token separated by 'delimiter'.
*/

extern char * tokenize_string(char * source, char const * delimiter, char ** context);

#endif /* helper_h*/