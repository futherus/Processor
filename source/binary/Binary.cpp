#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "Binary.h"
#include "../dumpsystem/dumpsystem.h"

Binary_err binary_init(Binary* bin, ssize_t cap)
{
    CHECK$(!bin,         BIN_NULLPTR,   return BIN_NULLPTR;   )
    
    CHECK$(cap <= 0,     BIN_SIZE_ERR,  return BIN_SIZE_ERR;  )

    bin->buffer = (bin_t*) calloc(cap, sizeof(bin_t));

    CHECK$(!bin->buffer, BIN_BAD_ALLOC, return BIN_BAD_ALLOC; )

    bin->ip  = 0;
    bin->sz  = 0;
    bin->cap = (size_t) cap;

    return BIN_NOERR;
}

Binary_err binary_sread(Binary* bin, void* src, size_t count)
{
    CHECK$(!(bin && src),              BIN_NULLPTR,  return BIN_NULLPTR;  )
    CHECK$(!bin->buffer,               BIN_NULL_BUF, return BIN_NULL_BUF; )
    CHECK$(bin->sz + count > bin->cap, BIN_SIZE_ERR, return BIN_SIZE_ERR; )
    
    memcpy(&bin->buffer[bin->sz], src, count * sizeof(bin_t));

    bin->sz += count;

    return BIN_NOERR;
}

Binary_err binary_fread(Binary* bin, FILE* istream, size_t count)
{
    CHECK$(!(bin && istream),          BIN_NULLPTR,    return BIN_NULLPTR;  )
    CHECK$(!bin->buffer,               BIN_NULL_BUF,   return BIN_NULL_BUF; )
    CHECK$(bin->sz + count > bin->cap, BIN_SIZE_ERR,   return BIN_SIZE_ERR; )

    size_t n_read = fread(bin->buffer, sizeof(bin_t), count, istream);

    if(n_read != count)
        CHECK$(!ferror(istream),        BIN_STREAM_FAIL, return BIN_STREAM_FAIL;)

    bin->sz += count;

    return BIN_NOERR;
}

Binary_err binary_swrite(void* dst, Binary* bin, size_t count)
{
    CHECK$(!(dst && bin),             BIN_NULLPTR,  return BIN_NULLPTR;  )
    CHECK$(!bin->buffer,              BIN_NULL_BUF, return BIN_NULL_BUF; )
    CHECK$(count > bin->sz - bin->ip, BIN_SIZE_ERR, return BIN_SIZE_ERR; )
        
    memcpy(dst, &(bin->buffer[bin->ip]), count * sizeof(bin_t));

    bin->ip += count;

    return BIN_NOERR;
}

Binary_err binary_fwrite(FILE* ostream, Binary* bin, size_t count)
{
    CHECK$(!(ostream && bin),         BIN_NULLPTR,     return BIN_NULLPTR;  )
    CHECK$(!bin->buffer,              BIN_NULL_BUF,    return BIN_NULL_BUF; )
    CHECK$(count > bin->sz - bin->ip, BIN_SIZE_ERR,    return BIN_SIZE_ERR; )

    size_t n_written = fwrite(&bin->buffer[bin->ip], sizeof(bin_t), count, ostream);
    CHECK$(n_written != count,        BIN_STREAM_FAIL, return BIN_STREAM_FAIL; )

    bin->ip += count;

    return BIN_NOERR;
}

Binary_err binary_dstr(Binary* bin)
{
    CHECK$(!bin, BIN_NULLPTR, return BIN_NULLPTR; )
        
    free(bin->buffer);

    return BIN_NOERR;
}
