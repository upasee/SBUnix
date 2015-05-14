#include <sys/sbunix.h>
#include <stdlib.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/sched.h>
#include <sys/paging.h>
#include <sys/file.h>
#include <sys/string.h>
#include <sys/process.h>

struct File *FileList = NULL;
struct task_struct *current_task;
volatile struct buf *scan_buf;
volatile int flag1;
volatile int scan_flag;
struct RW *readerList = NULL;
struct RW *writerList = NULL;

int parse_path(char *input, char *parent, char delim)
{
	int i=0;
	while(input[i]!='\0' && input[i]!=delim)
	{
		parent[i]=input[i];
		i++;
	}
	i++;
	if(input[i] != '\0')
		return -1;
	return 0;
}

/* Create a Linked List of all the files in tarfs */
void file_init(char *elfStart, char *elfEnd)
{	
	struct posix_header_ustar *ph_init= (struct posix_header_ustar *)elfStart;
	struct elfHdr *binary;
	char *size;
	struct File *f;
	f = (struct File *)kmalloc(sizeof(struct File));
        uint64_t sizeInt = 0;
        uint64_t prev_size = (uint64_t)elfStart;
	uint64_t file_type=0;
	char *parent = (char *)kmalloc(100);
	f->type=F_DIR;
	f->name = (char *)kmalloc(10);
	f->name="/";
	FileList = f;
	FileList->next_file=NULL;
	while((uint64_t)ph_init < (uint64_t)elfEnd)
        {
		ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
		if(strcmp(ph_init->name,"")==0)
                        break;
		prev_size += sizeof(struct posix_header_ustar) + sizeInt;
		size = (char *)ph_init->size;
                sizeInt = atoi(size,8);
		file_type = atoi((char *)ph_init->typeflag,8);
		f = (struct File *)kmalloc(sizeof(struct File));
		binary = (struct elfHdr *)(( uint64_t )ph_init + (uint64_t )sizeof(struct posix_header_ustar));
		parse_path(ph_init->name,parent,'/');
		if(file_type == 5)
		{
			f->type = F_DIR;
			f->start_addr=0;
			f->file_size=0;
		}
		else if(file_type == 0)
		{
			f->type=F_FILE;
			f->start_addr=(void *)binary;
			f->file_size=sizeInt;	
		}
		sizeInt=roundUp((void *)sizeInt, 512);
		f->name=ph_init->name;
		if(!FileList)
		{
			FileList = f;
			FileList->next_file = NULL;	
		}
		else
		{
			struct File *temp = FileList;
			while(temp->next_file)
				temp=temp->next_file;
			temp->next_file = f;
		}
        }
}

/*
int sys_close(int fd)
{
	if(fd < 0)
	{
		return -1;
	}
	if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open == 1)
	{
		current_task->open_fd[fd]->open=0;
	}
	return 0;
}*/

int sys_close(int fd)
{
        if(fd < 0)
        {
                return -1;
        }

        if (current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open == 1 && current_task->open_fd[fd]->f != 0 && current_task->open_fd[fd]->f->type == F_PIPE)
        {
                if (current_task->open_fd[fd]->perm & PTE_W)
                {
                        current_task->open_fd[fd]->f->write_count--;
                        if (current_task->open_fd[fd]->f->write_count == 0)
                        {
                                struct RW *prev = readerList;
                                struct RW *rd = NULL;
                                if (readerList->next)
                                        rd = readerList->next;
                                if (readerList->f == current_task->open_fd[fd]->f)
                                {
                                        readerList->task->status = RUNNABLE;
                                        readerList = readerList->next;
                                }
                                while (rd)
                                {
                                        if (rd->f == current_task->open_fd[fd]->f)
                                        {
                                                rd->task->status = RUNNABLE;
                                                prev->next = rd->next;
                                        }
                                        else
                                                prev = rd;
                                        rd = rd->next;
                                }
                                current_task->open_fd[fd]->f->read_count = 0;
              }
                }
                else if (current_task->open_fd[fd]->perm & PTE_NW)
                {
                        current_task->open_fd[fd]->f->read_count--;
                        if (current_task->open_fd[fd]->f->read_count == 0)
                        {
                                struct RW *prev = writerList;
                                struct RW *wr=NULL;
                                if (writerList->next)
                                      wr = writerList->next;
                                if (writerList->f == current_task->open_fd[fd]->f)
                                {
                                        writerList->task->status = RUNNABLE;
                                        writerList = writerList->next;
                                }
                                while (wr)
                                {
                                        if (wr->f == current_task->open_fd[fd]->f)
                                        {
                                                wr->task->status = RUNNABLE;
                                                prev->next = wr->next;
                                        }
                                        else
                                                prev = wr;
                                        wr = wr->next;
                                }
                                current_task->open_fd[fd]->f->read_count = 0;
                        }
                }

                current_task->open_fd[fd]->open=0;
}

        else if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open == 1)
        {
                current_task->open_fd[fd]->open=0;
        }
        return 0;
}

/* Open a file */
int64_t sys_open(char *name,int flags)
{
	int i=0;
	struct File *temp = FileList;
	if(flags != O_RDONLY && flags != O_DIRECTORY)
	{
		kprintf("You do not have permission to perform this action\n");
		current_task->tf.reg_rax = -1;
		return -1;
	}
	while(temp)
	{
		if(strcmp(temp->name,name)==0)
		{
			if(flags == O_DIRECTORY)
			{
				if(temp->type != F_DIR)
				{
					current_task->tf.reg_rax = -1;
					kprintf("Please mention a valid directory name\n");
			                return -1;
				}
			}
			else
			{
				if(temp->type != F_FILE)
				{
					current_task->tf.reg_rax = -1;
                                        kprintf("Not a regular file\n");
                                        return -1;
				}
			}
			for(i=3;i<20;i++)
			{
				if(current_task->open_fd[i] == 0)
				{
					current_task->open_fd[i] = (struct FileDesc *)kmalloc(sizeof(struct FileDesc));
				}
				if(current_task->open_fd[i]->open != 0 && strcmp(current_task->open_fd[i]->f->name,temp->name) == 0)
				{
					current_task->tf.reg_rax = i;
					return i;
				}
			}

			for(i=3;i<20;i++)
                        {
				if(current_task->open_fd[i]->open == 0)
				{
					current_task->open_fd[i]->open = 1;
					current_task->open_fd[i]->fd = i;
					current_task->open_fd[i]->perm = flags;
					current_task->open_fd[i]->f = temp;
					current_task->tf.reg_rax = i;
                                        return i;
				}
			}
			if(current_task->open_fd[i]->open == 0)
			{
				kprintf("Sorry! You can't open more than 20 files at once!\n");
			}
			break;
		}
		if(temp->next_file)
		{
			temp = temp->next_file;
		}
		else
			break;
	}
	current_task->tf.reg_rax = -1;
	return -1;
}

size_t sys_read(int fd, void *buf, size_t count)
{
	int temp_count=0;
	if(fd < 0)
	{
		memset1(buf, 0, count);
		current_task->tf.reg_rax = -1;
		return -1;
	}
	if((fd == 0) && (current_task->open_fd[fd]->fd == 0))
	{
		if (current_task->background != 1)
                {
		scan_flag=1;
		__asm __volatile( "sti"
					:
					: );
		current_task->status = BLOCKED;
		memset1(&(scan_buf->pp_buf), 0, 0x1000);
		scan_buf->pos = 0;
		while (flag1 != 1);
		if (flag1 == 1)
		{
			memcpy(buf, scan_buf->pp_buf, count);
			__asm __volatile( "cli"
					:
					: );
			flag1=0;
			scan_flag=0;
			current_task->status=RUNNABLE;
			return count;
		}
		}
		 else
               {
                       current_task->status = EXIT;
                       sched();
              }
	}


	else if ((current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0 &&  current_task->open_fd[fd]->f->type == F_PIPE) || (current_task->open_fd[fd] != 0 && fd == 0 && current_task->open_fd[fd]->fd != 0))
	{
		struct RW *newRW = (struct RW*)kmalloc(sizeof(struct RW));
		current_task->status = WAITING;
		newRW->task = current_task;
		newRW->fdnum = fd;
		newRW->buf = buf;
		newRW->f = current_task->open_fd[fd]->f;
		newRW->size = count;
		newRW->next = NULL;
		if(!readerList)
			readerList = newRW;
		else
		{
			struct RW *rd = readerList;
			while(rd)
			{
				rd = rd->next;
			}
			rd->next = newRW;
		}
		newRW->f->read_count++;
		if ((newRW->f->pos == 0) && (newRW->f->write_count == 0))
		{
			newRW->f->read_count = 0;
			struct RW *read = readerList;
			while(read->next)
			{
				if (read->next->f == newRW->f)
				{
					read->next = readerList->next->next;
				}
			}
			return 0;
		}
		iterateAndRead();
		iterateAndWrite();
	}


	else if(fd >= 3 && fd <= 20)
	{

		if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0 && current_task->open_fd[fd]->f->type == F_FILE)
		{
			memset1(buf, 0, count);
			temp_count = current_task->open_fd[fd]->f->file_size - current_task->open_fd[fd]->offset-1;
			if(current_task->open_fd[fd]->offset  >= current_task->open_fd[fd]->f->file_size)
			{
				current_task->tf.reg_rax = 0;
				kprintf("End of file\n");
				return 0;
			}
			if(temp_count < count)
			{
				memcpy((void *)buf,(void *)(current_task->open_fd[fd]->f->start_addr + current_task->open_fd[fd]->offset),temp_count);
				current_task->tf.reg_rax = temp_count;
				current_task->open_fd[fd]->offset+=count;
				return temp_count;
			}
			else
			{
				memcpy((void *)buf,(void *)(current_task->open_fd[fd]->f->start_addr+current_task->open_fd[fd]->offset),count);
				current_task->tf.reg_rax = count;
				current_task->open_fd[fd]->offset+=count;
				return count;
			}
		}
		else
		{
			kprintf("You need to open the file in order to read\n");
			return -1;
		}
	}
	return -1;
}

/*
size_t sys_read(int fd, void *buf, size_t count)
{
	int temp_count=0;
	if(fd < 0)
	{
		memset1(buf, 0, count);		
		current_task->tf.reg_rax = -1;
		return -1;
	}
	if(fd >= 3 && fd <= 20)
	{
		if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0 && current_task->open_fd[fd]->f->type == F_FILE)
		{
			kprintf("offset in read is %x\n",current_task->open_fd[fd]->offset);
			memset1(buf, 0, count);
			temp_count = current_task->open_fd[fd]->f->file_size - current_task->open_fd[fd]->offset-1;
			if(current_task->open_fd[fd]->offset  >= current_task->open_fd[fd]->f->file_size)
			{
				current_task->tf.reg_rax = 0;
				kprintf("End of file\n");
				return 0;
			}
			if(temp_count < count)
			{
				memcpy((void *)buf,(void *)(current_task->open_fd[fd]->f->start_addr + current_task->open_fd[fd]->offset),temp_count);
				current_task->tf.reg_rax = temp_count;
				current_task->open_fd[fd]->offset+=count;
				kprintf("count is %x\n",count);
				kprintf("offset while returning from read is %x\n",current_task->open_fd[fd]->offset);
				return temp_count;
			}
			else
			{
				memcpy((void *)buf,(void *)(current_task->open_fd[fd]->f->start_addr+current_task->open_fd[fd]->offset),count);
				current_task->tf.reg_rax = count;
				current_task->open_fd[fd]->offset+=count;
				kprintf("count is %x\n",count);
				kprintf("offset while returning from read is %x\n",current_task->open_fd[fd]->offset);
				return count;
			}
			kprintf("offset while returning from read is %x\n",current_task->open_fd[fd]->offset);
		}
		else
		{
			kprintf("You need to open the file in order to read\n");
			return -1;
		}
	}

	else if(fd == 0)
	{
		scan_flag=1;
		__asm __volatile( "sti"
					:
					: );
		current_task->status = BLOCKED;
//		kprintf("scan_buf is %x\n",&(scan_buf->pp_buf));
		memset1(&(scan_buf->pp_buf), 0, 0x1000);
		scan_buf->pos = 0;
		while (flag1 != 1);
		if (flag1 == 1)
		{
			memcpy(buf, scan_buf->pp_buf, count);
			__asm __volatile( "cli"
					:
					: );
			scan_flag=0;
			flag1=0;
			return count;
		}
	}
        return -1;
}
*/
int64_t sys_lseek(uint64_t fd, uint64_t offset, uint64_t whence)
{
	if(fd >= 3 && fd <= 20)
        {
                if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0)
		{
			if(whence == SEEK_SET)
			{
				current_task->open_fd[fd]->offset = offset;
			}
			else if(whence == SEEK_CUR)
			{
				current_task->open_fd[fd]->offset += offset;
			}
			else if(whence == SEEK_END)
			{
				current_task->open_fd[fd]->offset += current_task->open_fd[fd]->f->file_size;
			}
			else
			{
				current_task->tf.reg_rax = -1;
				return -1;
			}
			if(current_task->open_fd[fd]->offset >= (current_task->open_fd[fd]->f->file_size -1 ))
			{
				current_task->tf.reg_rax = -1;
                                return -1;
			}
			current_task->tf.reg_rax = offset;
			return offset;
		}
	}
	current_task->tf.reg_rax = -1;
	return -1;
}

size_t sys_write(int fd, void *buf, size_t count)
{
        if (fd < 0)
        {
                current_task->tf.reg_rax = -1;
                return -1;
        }
        if ((fd == 1) && (current_task->open_fd[fd]->fd == 1))
                write_string(7, (const char*)buf, count);

        else if ((current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0 &&  current_task->open_fd[fd]->f->type == F_PIPE) ||(current_task->open_fd[fd] != 0 && fd == 1 && current_task->open_fd[fd]->fd != 1))
        {
                struct RW *newRW = (struct RW *)kmalloc(sizeof(struct RW));
                newRW->task = current_task;
                newRW->task->status = WAITING;
                newRW->f = current_task->open_fd[fd]->f;
                newRW->fdnum = current_task->open_fd[fd]->fd;
                newRW->size = count;
                newRW->buf = buf;
                if(!writerList)
                        writerList = newRW;
                else
                {
                        struct RW *wr = writerList;
                        while(wr)
                        {
                                wr = wr->next;
                        }
                        wr->next = newRW;
                }
                newRW->f->write_count++;
                iterateAndWrite();
                iterateAndRead();
                iterateAndWrite();
        }


        return count;
}

void iterateAndRead()
{
        struct RW *prev = readerList;
        struct RW *rd1 = NULL;
        if (readerList)
                rd1 = readerList->next;
        if (readerList)
        {
        if (readerList->f->pos > readerList->size)
        {
                loadcr3((void *)readerList->task->cr3);
                memcpy((void *)readerList->buf, (void *)(readerList->f->start_addr), readerList->size);
                memcpy((void *)(readerList->f->start_addr), (void *)(readerList->f->start_addr + readerList->size), (readerList->f->pos - (readerList->size)));
                readerList->f->pos = readerList->f->pos - readerList->size;
                memset1((void *)(readerList->f->start_addr + readerList->f->pos), 0, readerList->size);
                readerList->f->read_count--;
                readerList = readerList->next;
                readerList->task->status = RUNNABLE;

        }
        else if (readerList->f->pos <= readerList->size)
        {
                loadcr3((void *)readerList->task->cr3);
                memcpy((void *)readerList->buf, (void *)readerList->f->start_addr, readerList->f->pos);
                memset1((void *)(readerList->f->start_addr), 0, readerList->f->pos);
                readerList->size = readerList->size - readerList->f->pos;
                readerList->f->pos = 0;
        }
        while(rd1)
        {
                if (rd1->f->pos > rd1->size)
                {
                        loadcr3((void *)rd1->task->cr3);
                        memcpy((void *)rd1->buf, (void *)(rd1->f->start_addr), rd1->size); 
                        memcpy((void *)(rd1->f->start_addr), (void *)(rd1->f->start_addr + rd1->size), (rd1->f->pos - (rd1->size)));
                        rd1->f->pos = rd1->f->pos - rd1->size;
                        memset1((void *)(rd1->f->start_addr + rd1->f->pos), 0, (rd1->size));
                        prev->next = rd1->next;
                        rd1->f->read_count--;
                        rd1->task->status = RUNNABLE;
                }
                else if (rd1->f->pos <= rd1->size)
                {
                       	loadcr3((void *)rd1->task->cr3);
			memcpy((void *)rd1->buf, (void *)rd1->f->start_addr, rd1->f->pos);
                        memset1((void *)(rd1->f->start_addr), 0, rd1->f->pos);
                        rd1->size = rd1->size - rd1->f->pos;
                        rd1->f->pos = 0;
                }
                else
                        prev = rd1;
                rd1 = rd1->next;
        }
}
}


void iterateAndWrite()
{
        struct RW *prev = writerList;
        struct RW *wr1 = NULL;
        if (writerList)
                wr1 = writerList->next;
        if (writerList)
        {
if ((4096 - writerList->f->pos) >= writerList->size)
        {
                loadcr3((void *)writerList->task->cr3);
                memcpy((void *)(writerList->f->start_addr + writerList->f->pos), (void *)(writerList->buf), writerList->size);
                writerList->f->pos = writerList->f->pos + writerList->size;
                writerList->f->write_count--;
                writerList->task->status = RUNNABLE;
                writerList = writerList->next;

        }
        else if ((4096 - writerList->f->pos) < writerList->size)
        {
                loadcr3((void *)writerList->task->cr3);
                memcpy((void *)(writerList->f->start_addr + writerList->f->pos), (void *)(writerList->buf), (4096 - writerList->f->pos));
                writerList->size = writerList->size - writerList->f->pos;
                writerList->f->pos = 4095;
        }
        while(wr1)
        {
                if ((4096 - wr1->f->pos) >= wr1->size)
                {
                        loadcr3((void *)wr1->task->cr3);
                        memcpy((void *)(wr1->f->start_addr + wr1->f->pos), (void *)(wr1->buf), wr1->size);
                        wr1->f->pos = wr1->f->pos + wr1->size;
                        wr1->f->write_count--;
                        wr1->task->status = RUNNABLE;
                        prev->next = wr1->next;
                }
                else if ((4096 - wr1->f->pos) < wr1->size)
                {
                        loadcr3((void *)wr1->task->cr3);
                        memcpy((void *)(wr1->f->start_addr + wr1->f->pos), (void *)(wr1->buf), (4096 - wr1->f->pos));
                        wr1->size = wr1->size - wr1->f->pos;
                        wr1->f->pos = 4095;
                }
                else
                        prev = wr1;
                wr1 = wr1->next;
        }
}
}




