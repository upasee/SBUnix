#include <sys/sbunix.h>
#include <stdlib.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/paging.h>
#include <sys/file.h>
#include <sys/string.h>
#include <sys/dir.h>
#include <sys/process.h>

void append_slash_after(char *s);
void append_slash_before(char *s);
void remove_slash_before(char *s);
void str_append(char *result, char *src2, char *src1);
int get_prev_directory(char *dir, char *prev);

int parse_root(char *input, char *parent, char delim)
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

int parse_for_directory(char str[], char *input, char delim)
{
        int i=0,j=0;
        for(i=0;str[i]!='\0' && str[i] != delim;i++);
        i++;
        while(str[i]!=delim && str[i]!='\0')
        {
                input[j] = str[i];
                i++;
                j++;
        }
        i=i+1;
        input[j]='\0';
        if(str[i]!='\0')
                return -1;
        return 0;
}

int64_t sys_getdents(uint64_t fd, char *buf, int count)
{
	if(fd < 3 || fd >=20)
		return 0;
	struct dirent *d = (struct dirent *)kmalloc(5*sizeof(struct dirent));
	struct posix_header_ustar *ph_init= (struct posix_header_ustar *)&_binary_tarfs_start;
        char *size;
        uint64_t sizeInt = 0;
	int present_flag=0;
        uint64_t prev_size=(uint64_t)&_binary_tarfs_start;
	int name_len=0;
	int temp_count=0;
	int temp_pos=0;
	if(current_task->open_fd[fd] != 0 && current_task->open_fd[fd]->open != 0)
	{

		if(strcmp(current_task->open_fd[fd]->f->name,"/")== 0)
		{
			while((temp_count < count) && ((uint64_t)ph_init < (uint64_t)&_binary_tarfs_end))
                        {
				ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
                                prev_size += sizeof(struct posix_header_ustar) + sizeInt;
                                size = (char *)ph_init->size;
                                sizeInt = atoi(size,8);
                                sizeInt=roundUp((void *)sizeInt, 512);
                                if(strcmp(ph_init->name,"")==0)
                                {
                                        current_task->tf.reg_rax = temp_count;
                                        return temp_count;
                                }
				char *input = (char *)kmalloc(10*sizeof(char));
				int ret=parse_root(ph_init->name,input,'/');
				if(ret < 0)
					continue;
				if(ret >= 0)
				{
					if(temp_pos < current_task->open_fd[fd]->pos)
                                	{
                                        	temp_pos++;
	                                        continue;
        	                        }
	                                d->d_off+=1;
        	                        d->d_reclen=sizeof(struct dirent);
                                	strcpy((void *)d->d_name,(void *)input);
                                        loadcr3((void *)current_task->cr3);
                                        strcpy((void *)d->d_name,(void *)input);
                                        memcpy(&buf[temp_count], (char *)d, d->d_reclen);
					temp_count+=d->d_reclen;
	                                if((count - temp_count) < d->d_reclen)
        	                                break;
                                }
			}	
		}
		else
		{
			// Search for directory in tarfs
			while((uint64_t)ph_init < (uint64_t)&_binary_tarfs_end)
			{
				ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
				prev_size += sizeof(struct posix_header_ustar) + sizeInt;
				size = (char *)ph_init->size;
				sizeInt = atoi(size,8);
				sizeInt=roundUp((void *)sizeInt, 512);
				if(strcmp(ph_init->name,"")==0)
				{
					current_task->tf.reg_rax = 0;
					return 0;
				}
				if(strcmp(ph_init->name,current_task->open_fd[fd]->f->name)==0)
				{
					name_len = strlen(current_task->open_fd[fd]->f->name);
					present_flag=1;
					break;
				}
			}
			if(present_flag == 0)
			{
				kprintf("Directory not found\n");
				current_task->tf.reg_rax = 0;
				return 0;
			}	
			while((temp_count < count) && ((uint64_t)ph_init < (uint64_t)&_binary_tarfs_end))
			{
				ph_init = (struct posix_header_ustar *)(prev_size + sizeInt);
				if(strncmp(ph_init->name,current_task->open_fd[fd]->f->name,name_len) == 0)
				{
					prev_size += sizeof(struct posix_header_ustar) + sizeInt;
					size = (char *)ph_init->size;
					sizeInt = atoi(size,8);
					sizeInt = roundUp((void *)sizeInt, 512);
				}
				else
				{
					current_task->open_fd[fd]->pos+=temp_count/sizeof(struct dirent);
					current_task->tf.reg_rax=temp_count;
					return temp_count;
				}
				char *input = (char *)kmalloc(10*sizeof(char));
				d->d_off+=1;
				d->d_reclen=sizeof(struct dirent);
				int ret=0;
				ret=parse_for_directory(ph_init->name,input,'/');
				if(ret >= 0)
				{
					strcpy((void *)d->d_name,(void *)input);
					if(temp_pos < current_task->open_fd[fd]->pos)
	                                {
        	                                temp_pos++;
                	                        continue;
                        	        }
					loadcr3((void *)current_task->cr3);
					strcpy((void *)d->d_name,(void *)input);
					memcpy(&buf[temp_count], (char *)d, d->d_reclen);
				}
				temp_count+=d->d_reclen;
				if((count - temp_count) < d->d_reclen)
					break;
			}
		}
		current_task->open_fd[fd]->pos+=temp_count/d->d_reclen;
	}	
	else
	{
		kprintf("Please open the directory first\n");
	}
	current_task->tf.reg_rax=temp_count;
	return temp_count;
}

int sys_getcwd(char *buff,int size)
{
	loadcr3((void *)current_task->cr3);
	memset1(buff,0,size);
	memcpy((void *)buff,(void *)current_dir,strlen(current_dir));
	current_task->tf.reg_rax=0;
	return 0;
}

int special_case(char *s, char *inp)
{
	if(strcmp(s,"..")==0)
		return -1;
	int i=0,j=0;
	while(s[i]!='\0' && s[i] != inp[j])
		i++;
	while(s[i]!='\0' && inp[j]!='\0')
	{
		if(s[i]!=inp[j])
			return -1;
		else
		{
			i++;
			j++;
		}
	}
	if(inp[j] != '\0')
		return -1;
	return 0;
}

int sys_chdir(char *path)
{
	char *path_temp = (char *)kmalloc(20);
	strcpy(path_temp,path);
	char *dir_copy = (char *)kmalloc(20);
	int spl=special_case(path_temp,"..");
	if(spl == 0)
	{
		strcpy(dir_copy,current_dir);
	}
	if(strcmp(path_temp,"/")==0)
	{
		strcpy(current_dir,"/");
		return 0;
	}
	else if(strcmp(path_temp,"..") == 0)
	{
		char *prev = (char *)kmalloc(sizeof(char));
		int k=get_prev_directory(current_dir,prev);
		if(k>=0)
			strcpy(current_dir,prev);
		return 0;
	}
	else if(spl >= 0)
	{
		char *input[20];
		int n = tokenize(path_temp,input,'/');
		int i=0;
		for(i=0;i<n;i++)
		{
			if(sys_chdir(input[i]) == -1)
			{
				strcpy(current_dir,dir_copy);
				return -1;	
			}
		}
		return 0;
	}
	else if(strcmp(path_temp,".") == 0)
	{
		return 0;	
	}
	else if(path_temp[0] == '/')
	{
		struct File *temp = FileList;
		remove_slash_before(path_temp);
		append_slash_after(path_temp);
		while(temp)
		{
			if(strcmp(temp->name,path_temp) == 0)
			{
				if(temp->type == F_DIR)
				{
					strcpy(current_dir,temp->name);
					append_slash_before(current_dir);
					append_slash_after(current_dir);
					return 0;
				}
				else
					return -1;
			}
			if(temp->next_file)
				temp=temp->next_file;
			else
				return -1;
		}
		
	}
	else
	{
		char *result = (char *)kmalloc(100);
		str_append(result,path_temp,current_dir);	
		remove_slash_before(result);
                append_slash_after(result);
		struct File *temp = FileList;
                while(temp)
                {
                        if(strcmp(temp->name,result) == 0)
                        {
                                if(temp->type == F_DIR)
                                {
                                        strcpy(current_dir,temp->name);
					append_slash_before(current_dir);
                                        append_slash_after(current_dir);
					return 0;	
                                }
                                else
                                        return -1;
                        }
                        if(temp->next_file)
                                temp=temp->next_file;
                        else
			{
				kprintf("No such file or directory\n");
                                return -1;
			}
                }
	}
	return 0;
}

void str_append(char *result, char *src2, char *src1)
{
	int i=0;
	while(src1[i]!='\0')
	{
		result[i]=src1[i];
		i++;
	}
	int j=0;
	while(src2[j]!='\0')
		result[i++]=src2[j++];
	result[i]='\0';
}

int get_prev_directory(char *dir, char *prev)
{
	int i=0;
	int count=0;
	int temp_count=0;
	while(dir[i]!='\0')
	{
		if(dir[i] == '/')
			count++;
		i++;
	}
	if(count == 0 || count == 1)
		return -1;
	i=0;
	while(temp_count <= count-2 && dir[i] != '\0')
	{
		if(dir[i] == '/')
			temp_count++;
		prev[i]=dir[i];
	}
	append_slash_after(prev);
	return 0;
} 

void append_slash_after(char *s)
{
	int i=0;
	while(s[i]!='\0')
		i++;
	if(s[i-1] != '/')
		s[i++]='/';
	s[i]='\0';
}

void append_slash_before(char *s)
{
	int i=0;
	int j=0;
	char d[20];
	if(s[0]!='/')
	{
		d[j++]='/';
		while(s[i]!='\0')
		{
			d[j]=s[i];
			i++;
			j++;
		}
		d[j]='\0';
		strcpy(s,d);
	}

}

void remove_slash_before(char *s)
{
        int i=0;
        if(s[0]=='/')
        {
		if(s[1] =='\0')
		{
			return;
		}
                i++;
                while(s[i]!='\0')
                {
                        s[i-1]=s[i];
                        i++;
                }
                s[i-1]='\0';
        }
}

