#include "Binary.h"
#include "../log/log.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

Binary_err binary_init(Binary* bin, ssize_t cap)
{
    ASSERT(bin, BIN_NULLPTR);
    
    ASSERT(cap > 0, BIN_SIZE_ERR);

    bin->buffer = (bin_t*) calloc(cap, sizeof(bin_t));

    ASSERT(bin->buffer, BIN_BAD_ALLOC);

    bin->ip  = 0;
    bin->sz  = 0;
    bin->cap = (size_t) cap;

    return BIN_NOERR;
}

Binary_err binary_sread(Binary* bin, void* src, size_t count)
{
    ASSERT(bin && src, BIN_NULLPTR);
    ASSERT(bin->buffer, BIN_NULL_BUF);
    ASSERT(bin->sz + count <= bin->cap, BIN_SIZE_ERR);
    
    memcpy(&bin->buffer[bin->sz], src, count * sizeof(bin_t));

    bin->sz += count;

    return BIN_NOERR;
}

Binary_err binary_fread(Binary* bin, FILE* istream, size_t count)
{
    ASSERT(bin && istream, BIN_NULLPTR);
    ASSERT(bin->buffer, BIN_NULL_BUF);
    ASSERT(bin->sz + count <= bin->cap, BIN_SIZE_ERR);

    size_t n_read = fread(bin->buffer, sizeof(bin_t), count, istream);

    if(n_read != count)
        ASSERT(!ferror(istream), BIN_STREAM_FAIL); 

    bin->sz += count;

    return BIN_NOERR;
}

Binary_err binary_swrite(void* dst, Binary* bin, size_t count)
{
    ASSERT(dst && bin, BIN_NULLPTR);
    ASSERT(bin->buffer, BIN_NULL_BUF);
    ASSERT(count <= bin->sz - bin->ip, BIN_SIZE_ERR);
        
    memcpy(dst, &(bin->buffer[bin->ip]), count * sizeof(bin_t));

    bin->ip += count;

    return BIN_NOERR;
}

Binary_err binary_fwrite(FILE* ostream, Binary* bin, size_t count)
{
    ASSERT(ostream && bin, BIN_NULLPTR);
    ASSERT(bin->buffer, BIN_NULL_BUF);
    ASSERT(count <= bin->sz - bin->ip, BIN_SIZE_ERR);

    size_t n_written = fwrite(&bin->buffer[bin->ip], sizeof(bin_t), count, ostream);
    ASSERT(n_written == count, BIN_STREAM_FAIL);

    bin->ip += count;

    return BIN_NOERR;
}

Binary_err binary_dstr(Binary* bin)
{
    ASSERT(bin, BIN_NULLPTR);
    ASSERT(bin->buffer, BIN_NULL_BUF);

    free(bin->buffer);

    return BIN_NOERR;
}