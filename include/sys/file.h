#ifndef _FILE_H
#define _FILE_H

#include <sys/defs.h>
#include <sys/process.h>

enum
{
	F_DIR=0,
	F_FILE,
	F_PIPE
};

struct File
{
	char *name;
	void *start_addr;
	uint64_t file_size;
	uint64_t type;
	struct File *next_file;
	uint64_t read_count;
        uint64_t write_count;
        uint64_t pos;
};

struct FileDesc
{
	struct File *f;
	uint64_t fd;
	uint64_t offset;
	uint64_t perm;
	uint64_t pos;
	uint64_t open;
};

struct RW
{
        struct task_struct *task;
        uint64_t fdnum;
        struct RW *next;
        struct File *f;
        uint64_t size;
        void *buf;
};


struct buf
{
        char pp_buf[4096];
        uint64_t pos;
};

volatile extern struct buf *scan_buf;

extern volatile int flag1;
extern volatile int scan_flag;
extern struct File *FileList;

void file_init(char *elfStart, char *elfEnd);
int64_t sys_open(char *name,int flags);
int sys_close(int fd);
size_t sys_read(int fd, void *buf, size_t count);
int64_t sys_lseek(uint64_t fd, uint64_t offset, uint64_t whence);
int64_t sys_getdents(uint64_t fd, char *buff,int count);
int parse_path(char *input, char *parent, char delim);
size_t sys_write(int fd, void *buf, size_t count);
void iterateAndRead();
void iterateAndWrite();

#endif
