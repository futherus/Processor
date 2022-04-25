/** \file
    \brief Struct Text and helper functions
*/

#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include <stddef.h>

enum Text_err
{
    TEXT_NOERR           = 0, /// no error
    TEXT_BAD_IFILE       = 1, /// error with input file
    TEXT_BAD_OFILE       = 2, /// error with output file
    TEXT_BAD_ALLOC       = 3, /// error with allocating
    TEXT_NULLPTR         = 4, /// ERROR: nullptr was passed
    TEXT_UNEXPCTD_ERR    = 5, /// ERROR
};

/// \brief Stores pointers to first and last+1 elements
struct Index
{
    char* begin = nullptr;
    char* end   = nullptr;
};

/// \brief Stores text itself and array of Index'es to lines
struct Text
{
    size_t buffer_size = 0;
    char*  buffer      = nullptr;

    size_t index_arr_size = 0;
    Index* index_arr      = nullptr;
};

/** \brief Creates text, fills it from file

    \param [out] text      Pointer to text to be initialized
    \param [in]  file_name Name of file to fill text from

    \return Text_err::TEXT_NOERR if succeed, message error otherwise
*/
Text_err text_create(Text*, char* infile_name);


/** \brief Prints text in plain format

    \param [in] text      Pointer to text to be printed
    \param [in] file_name Name of output file for text
*/
Text_err text_print(Text* text, char* file_name);

/// \brief Text destructor
void text_destroy(Text*);

#endif // TEXT_H
