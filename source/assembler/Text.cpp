#include "Text.h"

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>

///constructor block
static void set_index_arr(char* buffer, Index* index_arr, size_t index_arr_size)
{
    assert(buffer && index_arr);

    int ch = 0;
    size_t iter = 0;
    char* first_symbol_ptr = nullptr;
    char* last_symbol_ptr  = nullptr;

    for(size_t line_num = 0; line_num < index_arr_size; line_num++)
    {
        while(isspace(buffer[iter])) //skips ws until first symbol
            iter++;

        first_symbol_ptr = &buffer[iter];

        while(true)
        {
            ch = buffer[iter];

            if(ch == '\n' || ch == '\0')
            {
                iter++;
                break;
            }

            if(!isspace(ch))
                last_symbol_ptr = &buffer[iter]; //save last !ws symbol

            iter++;
        }

        index_arr[line_num].begin = first_symbol_ptr;
        index_arr[line_num].end = last_symbol_ptr + 1;
        *index_arr[line_num].end = '\0';

        first_symbol_ptr = nullptr;
        last_symbol_ptr  = nullptr;
    }
}

/** \brief Creates array of Index'es and fills it

    \param [out] dst_index_arr Pointer to destination pointer
    \param [out] dst_index_arr_size Pointer to number of Index'es in array
    \param [in]  buffer Buffer to fill index array

    \return TEXT_NOERR if succeed and error message otherwise
*/
static Text_err create_index_arr(Index** dst_index_arr, size_t* dst_index_arr_size, char* buffer)
{
    assert(dst_index_arr_size);
    assert(buffer);

    size_t iter     = 0;
    size_t line_num = 0;
    bool nline_flag = 1;

    while(true)
    {
        int ch = buffer[iter];

        if((ch == '\n' || ch == '\0') && nline_flag == 0)
        {
            nline_flag = 1;
            line_num++;
        }

        if(!isspace(ch))
            nline_flag = 0;

        if(ch == '\0')
            break;

        iter++;
    }

    Index* index_arr = (Index*) calloc(line_num, sizeof(Index));

    if(index_arr == nullptr)
    {
        return TEXT_BAD_ALLOC;
    }

    set_index_arr(buffer, index_arr, line_num);

    *dst_index_arr_size = line_num;
    *dst_index_arr = index_arr;

    return TEXT_NOERR;
}

/** \brief  Gets file size

    \param  [in] file_name Name of file to examine

    \return Size of file on success or 0 if failure occurs
*/
static size_t get_file_size(char* file_name)
{
    assert(file_name);

    FILE* stream = fopen(file_name, "r");
    if(stream == nullptr)
        return 0;

    if(fseek(stream, 0, SEEK_END) != 0)
    {
        fclose(stream);
        return 0;
    }

    long file_size = ftell(stream);

    fclose(stream);

    if(file_size == -1)
        return 0;

    return (size_t) file_size;
}

/** \brief Creates buffer, reads file to it

    \param [out] dst_buffer Pointer to destination pointer
    \param [out] dst_buffer_size Pointer to number of elements in buffer
    \param [in]  file_name Name of input file to fill buffer

    \return TEXT_NOERR if succeed and error message otherwise
*/
static Text_err create_buffer(char** dst_buffer, size_t* dst_buffer_size, char* file_name)
{
    assert(dst_buffer_size && file_name);

    size_t file_size = get_file_size(file_name);

    char* buffer = (char*) calloc(file_size + 1, sizeof(char));
    if(buffer == nullptr)
        return TEXT_BAD_ALLOC;

    FILE* istream = fopen(file_name, "r");
    if(istream == nullptr)
    {
        free(buffer);
        return TEXT_BAD_IFILE;
    }

    size_t file_read = fread(buffer, sizeof(char), file_size, istream);
    if(file_read != file_size)
    {
        free(buffer);
        if(fclose(istream) == EOF)
			return TEXT_UNEXPCTD_ERR;

        return TEXT_BAD_IFILE;
    }

    if(fclose(istream) == EOF)
    {
        free(buffer);
        return TEXT_UNEXPCTD_ERR;
    }

    buffer = (char*) realloc(buffer, (file_read + 1) * sizeof(char));
    if(buffer == nullptr)
    {
        free(buffer);
        return TEXT_BAD_ALLOC;
    }

    buffer[file_read] = '\0';

    *dst_buffer = buffer;
    *dst_buffer_size = file_read;

    return TEXT_NOERR;
}

Text_err text_create(Text* text, char* file_name)
{
    assert(text && file_name);

    if(text == nullptr || file_name == nullptr)
        return TEXT_NULLPTR;

    char* buffer = nullptr;
    size_t buffer_size = 0;
    Text_err msg = create_buffer(&buffer, &buffer_size, file_name);
    if(msg != TEXT_NOERR)
        return msg;

    Index* index_arr = nullptr;
    size_t index_arr_size = 0;
    msg = create_index_arr(&index_arr, &index_arr_size, buffer);
    if(msg != TEXT_NOERR)
    {
        free(buffer);
        return msg;
    }

    text->buffer = buffer;
    text->index_arr = index_arr;
    text->index_arr_size = index_arr_size;
    text->buffer_size = buffer_size;

    return TEXT_NOERR;
}

///output block
static Text_err print_line(char* begin, char* end, FILE* ostream)
{
    assert(begin && end);
    assert(ostream);

    size_t size = (size_t) (end - begin);

    if(fwrite(begin, sizeof(char), size, ostream) != size)
        return TEXT_BAD_OFILE;

    if(fwrite("\n", sizeof(char), 1, ostream) != 1)
        return TEXT_BAD_OFILE;

    return TEXT_NOERR;
}

Text_err text_print(Text* text, char* file_name)
{
    assert(text && file_name);

	if(text == nullptr || file_name == nullptr)
		return TEXT_NULLPTR;

	FILE* ostream = fopen(file_name, "w");
	if(ostream == nullptr)
		return TEXT_BAD_OFILE;

	for(size_t iter = 0; iter < text->index_arr_size; iter++)
    {
        Text_err msg = print_line(text->index_arr[iter].begin, text->index_arr[iter].end, ostream);
            if(msg != TEXT_NOERR)
                return msg;
    }

	if(fclose(ostream) == EOF)
		return TEXT_UNEXPCTD_ERR;

	return TEXT_NOERR;
}

///destructor
void text_destroy(Text* text)
{
    assert(text);

    text->buffer_size = 0;
    text->index_arr_size = 0;

    free(text->index_arr);
    free(text->buffer);
}
