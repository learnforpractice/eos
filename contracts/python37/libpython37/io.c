typedef __int128 int128_t ;
typedef unsigned __int128 uint128_t;
typedef char bool;

#include <eosiolib/db.h>
#include <eosiolib/action.h>
#include <eosiolib/chain.h>
#include <eosiolib/permission.h>
#include <eosiolib/privileged.h>
#include <eosiolib/system.h>
#include <eosiolib/token.h>
#include <eosiolib/transaction.h>
#include <eosiolib/token.h>
#include <eosiolib/crypto.h>
#include <eosiolib/print.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void prints( const char* cstr );
void printi( int64_t n );

struct MemoryFile {
    int id;
    int pos;
    int length;
};

#define IO_BUFFER_SIZE (64*1024)

static char *io_buffer = NULL;
//int open(const char *pathname, int flags, mode_t mode) {
int _open(const char *pathname, int flags, int mode) {
//    prints("+++++++_open:");prints(pathname);prints("\n");
    if (strcmp("contract", pathname) == 0) {
        return -1;
    }
    return -1;
    if (io_buffer == NULL) {
        io_buffer = (char *)calloc(IO_BUFFER_SIZE, 1);
    }
    struct MemoryFile *fp = (struct MemoryFile *)malloc(sizeof(struct MemoryFile));
    if (fp == NULL) {
        return -1;
    }

    uint64_t id = s2n(pathname, strlen(pathname));
    uint64_t code = current_receiver();
    uint64_t scope = s2n("filesystem", strlen("filesystem"));
    uint64_t table = scope;
    uint64_t payer = current_receiver();
    int itr = db_find_i64(code, scope, table, id);
    if (itr < 0) {
        itr = db_store_i64(scope, table, payer, id, "", 0);
        fp->length = 0;
    } else {
        fp->length = db_get_i64(itr, NULL, 0);
    }
    fp->id = itr;
    fp->pos = 0;
    prints(__FUNCTION__);prints("flags: ");printi(flags);prints("mode: ");printi(mode);prints("\n");
    return (int)fp;
}

off_t _lseek(int fd, off_t offset, int whence) {
    if (fd < 0) {
        return -1;
    }

    prints("+++++++_lseek:");printi(offset);prints("\n");

    struct MemoryFile *fp = (struct MemoryFile *)fd;
    if (fp->length < offset) {
        return -1;
    }
    fp->pos = offset;
    return offset;
}

ssize_t _write(int fd, const void *buf, size_t count) {
//    prints("++++_write:");printi(fd);prints(buf);prints("\n");
    if (fd == 1) {
        prints(buf);
        return count;
    }
    struct MemoryFile *fp = (struct MemoryFile *)fd;
    if (fp == NULL) {
        return 0;
    }
    memset(io_buffer, 0, IO_BUFFER_SIZE);

    int size = db_get_i64(fp->id, 0, 0);
    prints("++++_write fp->pos:");printi(fp->pos);prints("\n");
    if (size > 0) {
        db_get_i64(fp->id, io_buffer, size);
    }
    memcpy(io_buffer+fp->pos, buf, count);
    int write_size = fp->pos + count;
    if (write_size < size) {
        write_size = size;
    }
//    prints("++++_write:");prints(buf);printi(write_size);prints("\n");
    db_update_i64(fp->id, current_receiver(), io_buffer, write_size);
    fp->pos += count;
    return count;
}

#include <sys/stat.h>

int _fstat(int fd, struct stat *statbuf) {
    struct MemoryFile *fp = (struct MemoryFile *)fd;
    if (fp == NULL) {
        return 0;
    }
    statbuf->st_size = fp->length;
    return 0;
}

ssize_t _read(int fd, void *buf, size_t nbyte) {
    struct MemoryFile *fp = (struct MemoryFile *)fd;
//    memset(io_buffer, 0, IO_BUFFER_SIZE);
    if (fp == NULL) {
        return 0;
    }
    if (fp->pos >= fp->length) {
        return 0;
    }
    int size = db_get_i64(fp->id, 0, 0);
    if (size <= 0) {
        return 0;
    }
//    prints("++++_read size:");printi(size);prints("\n");
//    prints("++++_read nbyte:");printi(nbyte);prints("\n");
    if (nbyte > size) {
        nbyte = size;
    }

    db_get_i64(fp->id, io_buffer, fp->length);
    memcpy(buf, io_buffer+fp->pos, nbyte);
    fp->pos += nbyte;
    return nbyte;
}

int _close(int fd) {
    prints(__FUNCTION__);prints("\n");
    struct MemoryFile *fp = (struct MemoryFile *)fd;
    if (fp == NULL) {
        return -1;
    }
    free(fp);
    return 0;
}
