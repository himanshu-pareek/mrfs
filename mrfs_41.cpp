#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
using namespace std;
/*
	assuming the upper bound on number of blocks to be 2^20
	assuming the upper bound on number of inodes to be 2^10
*/

#define blocks_ub (1<<20)
#define inodes_ub (1<<10)

typedef struct
{
	int total_size;
	int max_inodes;
	int current_inode;
	int max_blocks;
	int current_block;
	bitset<blocks_ub> free_blocks;
	bitset<inodes_ub> free_inodes;

}super_type;

const int super_size = sizeof(super_type);
const int super_blocks = (super_size % 256==0)?(super_size/256):(super_size/256 +1);
const int super_size_occupied = super_blocks*256;

typedef struct
{
	bool file_type;
	int file_size;
	time_t last_mod;
	time_t last_read;
	unsigned char acess_perm[3];
	int ptr[10];

}inode_type;

typedef struct
{
	inode_type inode[inodes_ub];

}inode_list_type;

const int inode_list_size = sizeof(inode_list_type);
const int inode_list_blocks = (inode_list_size % 256==0)?(inode_list_size/256):(inode_list_size/256 +1);
const int inode_list_size_occupied = inode_list_blocks*256;

typedef unsigned char block_type[256];


map<int,block_type*> block;

typedef struct
{
	char file_name[30];
	short int inode;

}direct_entry_type;

typedef struct
{
	direct_entry_type entry[8];
}directory_type;

typedef struct
{
	int ptr[64];
}indirect_block;

typedef struct
{
	int inode_no;
	int offset;
	char mode;
	bool open=false;
}file_table_entry;


 char* mem_start;
 int current_directory;
 super_type* super;
 inode_list_type* ilist;
 map<int,file_table_entry > file_table;
 int number_of_file_dis =0;
 sem_t sem;

int chmod_myfs (char *name, int mode) {
	int s=sem_wait(&sem);
	int flag=1;
	int inode_no=-1;
	for (int i=0;i<8,flag;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8,flag;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			if (strcmp(dir->entry[j].file_name,name)==0) {
	    				inode_no = dir->entry[j].inode;
	    				//dir->entry[j].inode=-1;
		    			//super->free_inodes.reset(inode_no);
		    			//super->current_inode--;
	    				flag=0;
	    				break;
	    			}
	    		}
	    	}
	    }
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64,flag;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8,flag;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,name)==0) {
		    				inode_no = dir->entry[j].inode;
		    				//dir->entry[j].inode=-1;
		    				//super->free_inodes.reset(inode_no);
		    				//super->current_inode--;
		    				flag=0;
		    				break;
		    			}
		    		}
		    	}
		    }
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64,flag;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64,flag;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8,flag;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,name)==0) {
				    				inode_no = dir->entry[j].inode;
				    				//dir->entry[j].inode=-1;
		    						//super->free_inodes.reset(inode_no);
		    						//super->current_inode--;
				    				flag=0;
				    				break;
				    			}
    			    		}
    			    	}
    			    }
		    	}
		    }
    	}
    }
    if (inode_no==-1) {
    	cout << "\nfile not found\n";
    	s=sem_post(&sem);
    	return -1;
    }
    else cout << "\nfile found\n";
    cout << current_directory<<endl;
    bitset<9>b(mode);
    bitset<3>t;
    t[0]=b[0];
    t[1]=b[1];
    t[2]=b[2];
    ilist->inode[inode_no].acess_perm[2]=(unsigned char)(t.to_ulong());
    t[0]=b[3];
    t[1]=b[4];
    t[2]=b[5];
    ilist->inode[inode_no].acess_perm[1]=(unsigned char)(t.to_ulong());
	t[0]=b[6];
    t[1]=b[7];
    t[2]=b[8];
    ilist->inode[inode_no].acess_perm[0]=(unsigned char)(t.to_ulong());
    s=sem_post(&sem);
	return 0;

}
int status_myfs () {
	int s=sem_wait(&sem);
	//cout << endl<<(super_blocks + inode_list_blocks)*256<<endl;
	cout << "\ntotal size of file system:  "<< super->total_size <<endl;
	cout << "\ntotal size occupied:  "<<super->current_block*256 <<endl;
	cout << "\ntotal size available:  "<<super->total_size-super->current_block*256 <<endl;
	cout << "\ntotal number of files:  "<<super->current_inode <<endl;
	s=sem_post(&sem);
	return -1;
}

int  dump_myfs(char* filename) {
	int s=sem_wait(&sem);
	FILE *fp = fopen((filename) , "w");
	int bytes = fwrite (mem_start, 1, super->total_size, fp);
	s=sem_post(&sem);
	return 1;
}

int  restore_myfs(char* filename) {
	int s=sem_init(&sem,0,1);
	s=sem_wait(&sem);
	FILE *filefd = fopen(filename, "rb");
    int file_size;
    fseek(filefd, 0, SEEK_END);
    file_size = ftell(filefd);
    fclose(filefd);
    mem_start = new char[file_size];
    FILE *fp = fopen(filename, "rb");
    int x= fread (mem_start, 1, file_size, fp);
	super = (super_type*)mem_start;
	ilist = (inode_list_type*)(mem_start + super_size_occupied);
	for (int i=0;i<super->max_blocks;i++) {
		block[i]=(block_type*)mem_start+i;
		//cout << block[i]<<endl;
	}
	current_directory=0;
	s=sem_post(&sem);
	return 1 ;
}

void init_dir(int block_no) {
	directory_type* dir = (directory_type*)block[block_no];
	for (int j=0;j<8;j++) {
		((dir->entry)+j)->inode=-1;
		//cout << dir->entry[j].inode<<endl;
	}
}

void init_indirect_block(int block_no) {
	indirect_block* idb = (indirect_block*)block[block_no];
    	for (int i=0;i<64;i++) {
    		idb->ptr[i]=-1;
    	}
}

void print(block_type* block,int size) {
	unsigned char* str=*block;
	for (int i=0;i<size;i++) {
		cout << *(str+i);
	}
}

void print( char* str,int size) {
	for (int i=0;i<size;i++) {
		cout << *(str+i);
	}
}

void print(bool b) {
	if (b)cout << ".d.";
	else cout << "...";
}

void print(unsigned char c) {
	unsigned int i = (int)c-'0';
	bitset<3> b(i);
	cout << ".";
	if (b[2]==1)cout << 'r';
	else cout << '-';
	if (b[1]==1)cout << 'w';
	else cout << '-';
	if (b[0]==1)cout << 'x';
	else cout << '-';
	//cout << "\t";

}

bitset<3> acess(unsigned char a) {
	unsigned int i = (int)a-'0';
	return bitset<3>(i);
}

int open_myfs (char *filename, char mode) {
	int s=sem_wait(&sem);
	int flag=1;
	int inode_no=-1;
	for (int i=0;i<8;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			if (strcmp(dir->entry[j].file_name,filename)==0) {
	    				inode_no = dir->entry[j].inode;
	    				//dir->entry[j].inode=-1;
		    			//super->free_inodes.reset(inode_no);
		    			//super->current_inode--;
	    				flag=0;
	    				break;
	    			}
	    		}
	    		if (flag==0)break;
	    	}
	    }
	    if (flag==0)break;
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,filename)==0) {
		    				inode_no = dir->entry[j].inode;
		    				//dir->entry[j].inode=-1;
		    				//super->free_inodes.reset(inode_no);
		    				//super->current_inode--;
		    				flag=0;
		    				break;
		    			}
		    		}
		    		if (flag==0)break;
		    	}
		    }
		    if (flag==0)break;
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,filename)==0) {
				    				inode_no = dir->entry[j].inode;
				    				//dir->entry[j].inode=-1;
		    						//super->free_inodes.reset(inode_no);
		    						//super->current_inode--;
				    				flag=0;
				    				break;
				    			}
    			    		}
    			    		if (flag==0)break;
    			    	}
    			    }
    			    if (flag==0)break;
		    	}
		    }
		    if (flag==0)break;
    	}
    }
    if (inode_no==-1) {
    	if (mode=='r') {
    		cout << "\nfile not found\n";
    		s=sem_post(&sem);
    	    return -1;
    	}
    	if (mode=='w') {
    		int inode_num ;
		    for (int i=0;i<inodes_ub;i++) {
		    	if (super->free_inodes[i]==0) {
		    		inode_num = i;
		    		super->free_inodes.set(i);
		    		super->current_inode++;
		    		break;
		    	}
		    }
		    int flag=0;
		    for (int i=0;i<8;i++) {
		    	if (ilist->inode[current_directory].ptr[i]==-1) {
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		ilist->inode[current_directory].ptr[i] = j;
				    		init_dir(j);
				    		super->free_blocks.set(j);
				    		super->current_block++;
				    		break;
				    	}
			    	}
				}
		    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode==-1) {
		    			dir->entry[j].inode = inode_num;
		    			strcpy(dir->entry[j].file_name , filename);
		    			//cout << dir->entry[j].file_name<<endl;
		    			//cout << dir->entry[j].inode<<endl;
		    			//cout << ilist->inode[current_directory].ptr[i]<<endl;
		    			flag=1;
		    			break;
		    		}
		    	}
		    	if (flag)break;
		    }
		    //cout << "lol\n";
		    if (flag==0) {
		    	if (ilist->inode[current_directory].ptr[8]==-1) {
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		ilist->inode[current_directory].ptr[8] = j;
				    		init_indirect_block(j);
				    		super->free_blocks.set(j);
				    		super->current_block++;
				    		break;
				    	}
			    	}
				}
				indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[8]];
				for (int i=0;i<64;i++) {
					if (idb->ptr[i]==-1) {
						for (int j=0;j<super->max_blocks;j++) {
					    	if (super->free_blocks[j]==0) {
					    		idb->ptr[i] = j;
					    		init_dir(j);
					    		super->free_blocks.set(j);
					    		super->current_block++;
					    		break;
					    	}
				    	}
					}
			    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
			    	for (int j=0;j<8;j++) {
			    		if (dir->entry[j].inode==-1) {
			    			dir->entry[j].inode = inode_num;
			    			strcpy(dir->entry[j].file_name , filename);
			    			flag=1;
			    			break;
			    		}
			    	}
			    	if (flag)break;
				}
		    }
		    if (flag==0) {
		    	if (ilist->inode[current_directory].ptr[9]==-1) {
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		ilist->inode[current_directory].ptr[9] = j;
				    		init_indirect_block(j);
				    		super->free_blocks.set(j);
				    		super->current_block++;
				    		break;
				    	}
			    	}
				}
				indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[9]];
				for (int k=0;k<64;k++) {
					if (idb->ptr[k]==-1) {
						for (int j=0;j<super->max_blocks;j++) {
					    	if (super->free_blocks[j]==0) {
					    		idb->ptr[k] = j;
					    		init_indirect_block(j);
					    		super->free_blocks.set(j);
					    		super->current_block++;
					    		break;
					    	}
				    	}
					}
					indirect_block* iidb =(indirect_block*)block[idb->ptr[k]];
					for (int i=0;i<64;i++) {
						if (iidb->ptr[i]==-1) {
							for (int j=0;j<super->max_blocks;j++) {
						    	if (super->free_blocks[j]==0) {
						    		iidb->ptr[i] = j;
						    		init_dir(j);
						    		super->free_blocks.set(j);
						    		super->current_block++;
						    		break;
						    	}
					    	}
						}
				    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
				    	for (int j=0;j<8;j++) {
				    		if (dir->entry[j].inode==-1) {
				    			dir->entry[j].inode = inode_num;
				    			strcpy(dir->entry[j].file_name , filename);
				    			flag=1;
				    			break;
				    		}
				    	}
				    	if (flag)break;
					}
					if (flag)break;
				}
		    }
		    ilist->inode[inode_num].file_type = 0;
			ilist->inode[inode_num].file_size = 0;
			ilist->inode[inode_num].last_mod = time(NULL);
			ilist->inode[inode_num].last_read = time(NULL);
			ilist->inode[inode_num].acess_perm[0]=6;
			ilist->inode[inode_num].acess_perm[1]=6;
			ilist->inode[inode_num].acess_perm[2]=4;
			for (int i=0;i<10;i++)ilist->inode[inode_num].ptr[i]=-1;
			inode_no=inode_num;
		}
    }
    bitset<3> b =acess(ilist->inode[inode_no].acess_perm[0]);
    //cout << b<<endl;
    if (b[2]==0&&mode=='r') {
    	cout << "\nread acess not there\n";
    	s=sem_post(&sem);
    	return -1;
    }
    if (b[1]==0&&mode=='w') {
    	cout << "\nwrite acess not there\n";
    	s=sem_post(&sem);
    	return -1;
    }
    //else cout << "\nfile found\n";
    file_table_entry file;
    file.inode_no = inode_no;
    file.offset=0;
    file.mode=mode;
    file.open=true;
    number_of_file_dis++;
    file_table[number_of_file_dis]=file;
    s=sem_post(&sem);
    return number_of_file_dis;
}

int next_block(int inode_no , int offset) {
	int nwrite = 0;
    for (int i=0;i<8 ;i++) {
    	//cout << i<<endl;
    	//cout << ilist->inode[inode_no].ptr[i]<<endl;
		if (ilist->inode[inode_no].ptr[i]!=-1) {
			nwrite+=256;
			if (nwrite>offset)return ilist->inode[inode_no].ptr[i];
			//print(block[ilist->inode[inode_no].ptr[i]],min(256,total_size-nwrite));
			//nwrite+=min(256,total_size-nwrite);
    	}
    	else if (ilist->inode[inode_no].ptr[i]==-1&&nwrite<=offset) {

    		int block_no;
    		for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	ilist->inode[inode_no].ptr[i]=block_no;
	    	nwrite+=256;
			if (nwrite>offset)return ilist->inode[inode_no].ptr[i];
    	}
    	//if ()
	}
	if (ilist->inode[inode_no].ptr[8]==-1) {
			int block_no;
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	//cout << block_no<<endl;
	    	init_indirect_block(block_no);
	    	ilist->inode[inode_no].ptr[8]=block_no;}
	    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[8]];
	    	for (int i=0;i<64 ;i++) {
	    		if (idb->ptr[i]==-1) {int block_no_i;
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		 block_no_i = j;
			    		 super->free_blocks.set(j);
			    		 super->current_block++;
		    		break;
			    	}
		    	}
		    	idb->ptr[i]=block_no_i;}
		    	nwrite+=256;
				if (nwrite>offset)return idb->ptr[i];

			}
	if (ilist->inode[inode_no].ptr[9]==-1&&nwrite<=offset) {
			int block_no;
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	//cout << block_no<<endl;
	    	init_indirect_block(block_no);
	    	ilist->inode[inode_no].ptr[9]=block_no;}
	    	 idb =(indirect_block*)block[ilist->inode[inode_no].ptr[9]];
	    	for (int i=0;i<64 ;i++) {
	    		if (idb->ptr[i]==-1) {int block_no_i;
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		 block_no_i = j;
			    		 super->free_blocks.set(j);
			    		 super->current_block++;
		    		break;
			    	}
		    	}
		    	//cout << block_no_i<<endl;
		    	init_indirect_block(block_no_i);
		    	idb->ptr[i]=block_no_i;}
		    	indirect_block* iidb = (indirect_block*)block[idb->ptr[i]];

		    	for (int i=0;i<64 ;i++) {

		    		if (iidb->ptr[i]==-1) {int block_no_ii;
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		 block_no_ii = j;
				    		 super->free_blocks.set(j);
				    		 super->current_block++;
		    		break;
				    	}
			    	}
			    	iidb->ptr[i]=block_no_ii;}
			    	nwrite+=256;
					if (nwrite>offset)return iidb->ptr[i];

				}

			}

}

int read_myfs (int fd, int nbytes, char *buff) {
	int s=sem_wait(&sem);
	//std :: map<int,file_table_entry>::iterator = it;
	//it = file_table.find(fd);
	if (file_table[fd].open==false||file_table[fd].mode!='r') {cout << "fd invalid\n";return -1;}
	file_table_entry file = file_table[fd];
	ilist->inode[file.inode_no].last_read = time(NULL);
	int size = ilist->inode[file.inode_no].file_size - file.offset;
	int bytes_read = min(size,nbytes);
	if (bytes_read==0) {s=sem_post(&sem);return 0;}
	int writen=0;
	int block_no = next_block(file.inode_no,file.offset);
	//print(block[block_no],256);
	memcpy(buff,*(block[block_no])+((file.offset)%256),min((bytes_read-writen),(256-((file.offset)%256))));
	writen+=min((bytes_read-writen),(256-((file.offset)%256)));

	//print()
	//cout << writen<<endl;
	while(bytes_read>writen) {
		block_no = next_block(file.inode_no,file.offset + writen);
		//print(block[block_no],min((bytes_read-writen),256));
		memcpy(buff+writen,*(block[block_no]),min((bytes_read-writen),256));
		writen+=min((bytes_read-writen),256);
		//cout << writen<<endl;
	}
	file_table[fd].offset+=bytes_read;
	s=sem_post(&sem);
	return bytes_read;
}

int write_myfs (int fd, int nbytes, char *buff) {
	int s=sem_wait(&sem);
	//std :: map<int,file_table_entry>::iterator = it;
	//it = file_table.find(fd);
	if (file_table[fd].open==false||file_table[fd].mode!='w') {cout << "fd invalid\n";s=sem_post(&sem);return -1;}
	ilist->inode[file_table[fd].inode_no].file_size=max(ilist->inode[file_table[fd].inode_no].file_size,file_table[fd].offset+nbytes);
	//cout << ilist->inode[file_table[fd].inode_no].file_size<<endl;
	file_table_entry file = file_table[fd];
	ilist->inode[file.inode_no].last_mod = time(NULL);
	int bytes_write = nbytes;
	if (bytes_write==0) {s=sem_post(&sem);return 0;}
	int writen=0;
	int block_no = next_block(file.inode_no,file.offset);
	//print(block[block_no],256);
	memcpy(*(block[block_no])+((file.offset)%256),buff,min((bytes_write-writen),(256-((file.offset)%256))));
	writen=min((bytes_write-writen),(256-((file.offset)%256)));
	//cout << writen<<endl;
	//print()
	//cout << writen<<endl;
	while(bytes_write>writen) {
		block_no = next_block(file.inode_no,file.offset + writen);
		//cout << block_no<<endl;
		//print(block[block_no],min((bytes_read-writen),256));
		memcpy(*(block[block_no]),buff+writen,min((bytes_write-writen),256));
		writen+=min((bytes_write-writen),256);
		//cout << writen<<endl;
	}
	file_table[fd].offset+=bytes_write;
	//cout << "lol";
	s=sem_post(&sem);
	return bytes_write;
}

int eof_myfs(int fd) {
	int s=sem_wait(&sem);
	if (file_table[fd].open==false) {cout << "fd invalid\n";s=sem_post(&sem);return -1;}
	if (ilist->inode[file_table[fd].inode_no].file_size==file_table[fd].offset) {s=sem_post(&sem);return 1;}
	else{s=sem_post(&sem); return 0;}
}

int close_myfs (int fd ) {
	int s=sem_wait(&sem);
	if (file_table[fd].open==false) {cout << "fd invalid\n";s=sem_post(&sem);return -1;}
	file_table[fd].open=false;
	s=sem_post(&sem);
	return 1;
}

int rmdir_myfs (char *dirname) {
	int s=sem_wait(&sem);
	int inode_no=-1;
	int flag=0;
	short int* temp;
	for (int i=0;i<8;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
	    				inode_no=dir->entry[j].inode;
	    				temp = &(dir->entry[j].inode);
	    				flag=1;
	    				break;
	    			}
	    			//cout << dir->entry[j].inode<<endl;
	    		}
	    		if (flag)break;
	    	}
	    }
	    if (flag)break;
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag==0) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
		    				inode_no=dir->entry[j].inode;
			    			temp = &(dir->entry[j].inode);

		    				flag=1;
		    				break;
		    			}
		    		}
		    		if (flag)break;
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag==0) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
				    				inode_no=dir->entry[j].inode;
				    				temp = &(dir->entry[j].inode);
				    				flag=1;
				    				break;
				    			}
    			    		}
    			    		if (flag)break;
    			    	}
    			    }
    			    if (flag)break;
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (inode_no==-1) {cout << "dir not found\n";s=sem_post(&sem);return 1;}
    else {
    	cout << "dir found "<<dirname<<endl;
    }
    flag=0;

    for (int i=0;i<8;i++) {
		if (ilist->inode[inode_no].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[inode_no].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			flag=1;
	    			break;
	    			//cout << dir->entry[j].inode<<endl;
	    		}
	    	}
	    }
	    if (flag)break;
    }
    if (ilist->inode[inode_no].ptr[8]!=-1) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[inode_no].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			flag=1;
	    				break;
		    		}
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (ilist->inode[inode_no].ptr[9]!=-1) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[inode_no].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			flag=1;
	    						break;
    			    		}
    			    	}
    			    }
    			    if (flag)break;
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (flag) {
    	cout << "directory not empty\ncannot be deleted\n";
    	s=sem_post(&sem);
    	return -1;
    }
    cout << "directory empty therefore deleted\n";
    *temp = -1;
    super->free_inodes.reset(inode_no);
	super->current_inode--;
	s=sem_post(&sem);
	return 1;
}

int chdir_myfs (char *dirname) {
	int s=sem_wait(&sem);
	int inode_no=-1;
	int flag=0;
	for (int i=0;i<8;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
	    				inode_no=dir->entry[j].inode;
	    				flag=1;
	    				break;
	    			}
	    			//cout << dir->entry[j].inode<<endl;
	    		}
	    		if (flag)break;
	    	}
	    }
	    if (flag)break;
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag==0) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
		    				inode_no=dir->entry[j].inode;
		    				flag=1;
		    				break;
		    			}
		    		}
		    		if (flag)break;
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag==0) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,dirname)==0 && ilist->inode[dir->entry[j].inode].file_type==1) {
				    				inode_no=dir->entry[j].inode;
				    				flag=1;
				    				break;
				    			}
    			    		}
    			    		if (flag)break;
    			    	}
    			    }
    			    if (flag)break;
		    	}
		    }
		    if (flag)break;
    	}
    }
    if (inode_no==-1) {cout << "dir not found\n";s=sem_post(&sem);return 1;}
    else {
    	cout << "dir changed to "<<dirname<<endl;

    	//cout << current_directory<<endl;
    	current_directory = inode_no;
    	ilist->inode[inode_no].last_read = time(NULL);
    	//cout << current_directory<<endl;
    }
    s=sem_post(&sem);
    return 0;
}

int mkdir_myfs (char *dirname) {
	int s=sem_wait(&sem);
	int inode_no;
	for (int i=0;i<inodes_ub;i++) {
    	if (super->free_inodes[i]==0) {
    		inode_no = i;
    		super->free_inodes.set(i);
    		super->current_inode++;
    		break;
    	}
    }
    int flag=0;
    for (int i=0;i<8;i++) {
    	if (ilist->inode[current_directory].ptr[i]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[i] = j;
		    		init_dir(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
    	for (int j=0;j<8;j++) {
    		if (dir->entry[j].inode==-1) {
    			dir->entry[j].inode = inode_no;
    			strcpy(dir->entry[j].file_name , dirname);
    			//cout << dir->entry[j].file_name<<endl;
    			//cout << dir->entry[j].inode<<endl;
    			//cout << ilist->inode[current_directory].ptr[i]<<endl;
    			flag=1;
    			break;
    		}
    	}
    	if (flag)break;
    }
    //cout << "lol\n";
    if (flag==0) {
    	if (ilist->inode[current_directory].ptr[8]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[8] = j;
		    		init_indirect_block(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
		indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[8]];
		for (int i=0;i<64;i++) {
			if (idb->ptr[i]==-1) {
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		idb->ptr[i] = j;
			    		init_dir(j);
			    		super->free_blocks.set(j);
			    		super->current_block++;
			    		break;
			    	}
		    	}
			}
	    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode==-1) {
	    			dir->entry[j].inode = inode_no;
	    			strcpy(dir->entry[j].file_name , dirname);
	    			flag=1;
	    			break;
	    		}
	    	}
	    	if (flag)break;
		}
    }
    if (flag==0) {
    	if (ilist->inode[current_directory].ptr[9]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[9] = j;
		    		init_indirect_block(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
		indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[9]];
		for (int k=0;k<64;k++) {
			if (idb->ptr[k]==-1) {
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		idb->ptr[k] = j;
			    		init_indirect_block(j);
			    		super->free_blocks.set(j);
			    		super->current_block++;
			    		break;
			    	}
		    	}
			}
			indirect_block* iidb =(indirect_block*)block[idb->ptr[k]];
			for (int i=0;i<64;i++) {
				if (iidb->ptr[i]==-1) {
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		iidb->ptr[i] = j;
				    		init_dir(j);
				    		super->free_blocks.set(j);
				    		super->current_block++;
				    		break;
				    	}
			    	}
				}
		    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode==-1) {
		    			dir->entry[j].inode = inode_no;
		    			strcpy(dir->entry[j].file_name , dirname);
		    			flag=1;
		    			break;
		    		}
		    	}
		    	if (flag)break;
			}
			if (flag)break;
		}
    }
    ilist->inode[inode_no].file_type = 1;
	ilist->inode[inode_no].file_size = 0;
	ilist->inode[inode_no].last_mod = time(NULL);
	ilist->inode[inode_no].last_read = time(NULL);
	ilist->inode[inode_no].acess_perm[0]=7;
	ilist->inode[inode_no].acess_perm[1]=7;
	ilist->inode[inode_no].acess_perm[2]=5;
	for (int j=0;j<super->max_blocks;j++) {
    	if (super->free_blocks[j]==0) {
    		ilist->inode[inode_no].ptr[0] = j;
    		init_dir(j);
    		super->free_blocks.set(j);
    		super->current_block++;
    		break;
    	}
	}
	directory_type* dir = (directory_type*)block[ilist->inode[inode_no].ptr[0]];
	dir->entry[0].inode = current_directory;
	strcpy(dir->entry[0].file_name , "dotdot");


	for (int i=1;i<10;i++)ilist->inode[inode_no].ptr[i]=-1;
		s=sem_post(&sem);
	return 0;
}


int rm_myfs(char *file_del) {
	int s=sem_wait(&sem);
	int flag=1;
	int inode_no=-1;
	for (int i=0;i<8;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			if (strcmp(dir->entry[j].file_name,file_del)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
	    				inode_no = dir->entry[j].inode;
	    				dir->entry[j].inode=-1;
		    			super->free_inodes.reset(inode_no);
		    			super->current_inode--;
	    				flag=0;
	    				break;
	    			}
	    		}
	    		if (flag==0)break;
	    	}
	    }
	    if (flag==0)break;
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,file_del)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
		    				inode_no = dir->entry[j].inode;
		    				dir->entry[j].inode=-1;
		    				super->free_inodes.reset(inode_no);
		    				super->current_inode--;
		    				flag=0;
		    				break;
		    			}
		    		}
		    		if (flag==0)break;
		    	}
		    }
		    if (flag==0)break;
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,file_del)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
				    				inode_no = dir->entry[j].inode;
				    				dir->entry[j].inode=-1;
		    						super->free_inodes.reset(inode_no);
		    						super->current_inode--;
				    				flag=0;
				    				break;
				    			}
    			    		}
    			    		if (flag==0)break;
    			    	}
    			    }
    			    if (flag==0)break;
		    	}
		    }
		    if (flag==0)break;
    	}
    }
    if (inode_no==-1) {
    	cout << "\nfile not found\n";
    	s=sem_post(&sem);
    	return -1;
    }
    else cout << "\nfile found\n";

	for (int i=0;i<8 ;i++) {
		if (ilist->inode[inode_no].ptr[i]!=-1) {
    		super->free_blocks.reset(ilist->inode[inode_no].ptr[i]);
    		super->current_block--;
    		ilist->inode[inode_no].ptr[i]=-1;
    	}

	}
	//cout << "wegfw\n";
	if (ilist->inode[inode_no].ptr[8]!=-1) {
    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[8]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		super->free_blocks.reset(idb->ptr[i]);
	    		super->current_block--;
	    		idb->ptr[i]=-1;
	    	}

		}
		ilist->inode[inode_no].ptr[8]=-1;
	}
	//cout << "qefeq\n";
	if (ilist->inode[inode_no].ptr[9]!=-1) {

    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[9]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		indirect_block* iidb = (indirect_block*)block[idb->ptr[i]];
	    			    	//init_indirect_block(block_no_i);

		    	for (int i=0;i<64 ;i++) {
		    		if (iidb->ptr[i]!=-1) {
				    	super->free_blocks.reset(iidb->ptr[i]);
				    	super->current_block--;
				    	iidb->ptr[i]=-1;
				    }

				}
				idb->ptr[i]=-1;

			}

		}
		ilist->inode[inode_no].ptr[9]=-1;
	}
	cout << "deleted\n";
	s=sem_post(&sem);
	return 0;
}


int ls_myfs() {
	int s=sem_wait(&sem);
	cout << "Permissions\t\tname\tsize\tDate last modified\n";
	for (int i=0;i<8;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			print(ilist->inode[dir->entry[j].inode].file_type);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			cout << "\t";
	    			cout << "\t"<<dir->entry[j].file_name<<"\t"<<ilist->inode[dir->entry[j].inode].file_size<<"\t"<<asctime(localtime(&(ilist->inode[dir->entry[j].inode].last_mod)))<<endl;
	    		}
	    	}
	    }
    }
    if (ilist->inode[current_directory].ptr[8]!=-1) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			print(ilist->inode[dir->entry[j].inode].file_type);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			cout << "\t";
	    			cout << "\t"<<dir->entry[j].file_name<<"\t"<<ilist->inode[dir->entry[j].inode].file_size<<"\t"<<asctime(localtime(&(ilist->inode[dir->entry[j].inode].last_mod)))<<endl;
		    		}
		    	}
		    }
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			print(ilist->inode[dir->entry[j].inode].file_type);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			print(ilist->inode[dir->entry[j].inode].acess_perm[0]);
    			    			cout << "\t";
	    			cout << "\t"<<dir->entry[j].file_name<<"\t"<<ilist->inode[dir->entry[j].inode].file_size<<"\t"<<asctime(localtime(&(ilist->inode[dir->entry[j].inode].last_mod)))<<endl;
    			    		}
    			    	}
    			    }
		    	}
		    }
    	}
    }
    s=sem_post(&sem);
    return 1;
}

int showfile_myfs(char* filename) {
	int s=sem_wait(&sem);
	int flag=1;
	int inode_no=-1;
	for (int i=0;i<8,flag;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8,flag;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			if (strcmp(dir->entry[j].file_name,filename)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
	    				inode_no = dir->entry[j].inode;
	    				//dir->entry[j].inode=-1;
		    			//super->free_inodes.reset(inode_no);
		    			//super->current_inode--;
	    				flag=0;
	    				break;
	    			}
	    		}
	    	}
	    }
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64,flag;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8,flag;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,filename)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
		    				inode_no = dir->entry[j].inode;
		    				//dir->entry[j].inode=-1;
		    				//super->free_inodes.reset(inode_no);
		    				//super->current_inode--;
		    				flag=0;
		    				break;
		    			}
		    		}
		    	}
		    }
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64,flag;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64,flag;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8,flag;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,filename)==0&& ilist->inode[dir->entry[j].inode].file_type==0) {
				    				inode_no = dir->entry[j].inode;
				    				//dir->entry[j].inode=-1;
		    						//super->free_inodes.reset(inode_no);
		    						//super->current_inode--;
				    				flag=0;
				    				break;
				    			}
    			    		}
    			    	}
    			    }
		    	}
		    }
    	}
    }
    if (inode_no==-1) {
    	cout << "\nfile not found\n";
    	s=sem_post(&sem);
    	return -1;
    }
    else cout << "\nfile found\n";
    bitset<3> b =acess(ilist->inode[inode_no].acess_perm[0]);
   // cout << b<<endl;
    if (b[2]==0) {
    	cout << "read acess not there\n";
    	s=sem_post(&sem);
    	return -1;
    }
    ilist->inode[inode_no].last_read = time(NULL);
	int total_size = ilist->inode[inode_no].file_size;
    int nwrite = 0;
    for (int i=0;i<8 ;i++) {
		if (ilist->inode[inode_no].ptr[i]!=-1) {
			print(block[ilist->inode[inode_no].ptr[i]],min(256,total_size-nwrite));
			nwrite+=min(256,total_size-nwrite);
			//nwrite += fwrite(block[ilist->inode[inode_no].ptr[i]],1,min(256,total_size-nwrite),fp);

    	}

	}
	//cout << "wegfw\n";
	if (ilist->inode[inode_no].ptr[8]!=-1) {
    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[8]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		//cout << "--------------------------------------------------------------------------------------\n";
	    		print(block[idb->ptr[i]],min(256,total_size-nwrite));
	    		nwrite+=min(256,total_size-nwrite);
	    		//nwrite += fwrite(block[idb->ptr[i]],1,min(256,total_size-nwrite),fp);

	    	}

		}
		//ilist->inode[inode_no].ptr[8]=-1;
	}
	//cout << "qefeq\n";
	if (ilist->inode[inode_no].ptr[9]!=-1) {

    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[9]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		indirect_block* iidb = (indirect_block*)block[idb->ptr[i]];
	    			    	//init_indirect_block(block_no_i);

		    	for (int i=0;i<64 ;i++) {
		    		if (iidb->ptr[i]!=-1) {
		    			//cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
		    			print(block[iidb->ptr[i]],min(256,total_size-nwrite));
		    			nwrite+=min(256,total_size-nwrite);
		    			//nwrite += fwrite(block[iidb->ptr[i]],1,min(256,total_size-nwrite),fp);

				    }

				}
				//idb->ptr[i]=-1;

			}

		}
		//ilist->inode[inode_no].ptr[9]=-1;
	}
	s=sem_post(&sem);
	return 1;
	//fclose(fp);
	//cout << "file written\n";
}

int copy_myfs2pc(char* source, char* dest) {
	int s=sem_wait(&sem);
	int flag=1;
	int inode_no=-1;
	for (int i=0;i<8,flag;i++) {
		if (ilist->inode[current_directory].ptr[i]!=-1) {
	    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
	    	//cout << ilist->inode[current_directory].ptr[i]<<endl;
	    	for (int j=0;j<8,flag;j++) {
	    		if (dir->entry[j].inode!=-1) {
	    			//cout << dir->entry[j].inode<<endl;
	    			if (strcmp(dir->entry[j].file_name,source)==0) {
	    				inode_no = dir->entry[j].inode;
	    				//dir->entry[j].inode=-1;
		    			//super->free_inodes.reset(inode_no);
		    			//super->current_inode--;
	    				flag=0;
	    				break;
	    			}
	    		}
	    	}
	    }
    }
    if (ilist->inode[current_directory].ptr[8]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[8]];
    	for (int i=0;i<64,flag;i++) {
    		if (idb->ptr[i]!=-1) {
		    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
		    	for (int j=0;j<8,flag;j++) {
		    		if (dir->entry[j].inode!=-1) {
		    			if (strcmp(dir->entry[j].file_name,source)==0) {
		    				inode_no = dir->entry[j].inode;
		    				//dir->entry[j].inode=-1;
		    				//super->free_inodes.reset(inode_no);
		    				//super->current_inode--;
		    				flag=0;
		    				break;
		    			}
		    		}
		    	}
		    }
    	}
    }
    if (ilist->inode[current_directory].ptr[9]!=-1&&flag) {
    	indirect_block* idb = (indirect_block*)block[ilist->inode[current_directory].ptr[9]];
    	for (int k=0;k<64,flag;k++) {
    		if (idb->ptr[k]!=-1) {
    			indirect_block* iidb = (indirect_block*)block[idb->ptr[k]];
		    	for (int i=0;i<64,flag;i++) {
    	    		if (iidb->ptr[i]!=-1) {
    			    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
    			    	for (int j=0;j<8,flag;j++) {
    			    		if (dir->entry[j].inode!=-1) {
    			    			if (strcmp(dir->entry[j].file_name,source)==0) {
				    				inode_no = dir->entry[j].inode;
				    				//dir->entry[j].inode=-1;
		    						//super->free_inodes.reset(inode_no);
		    						//super->current_inode--;
				    				flag=0;
				    				break;
				    			}
    			    		}
    			    	}
    			    }
		    	}
		    }
    	}
    }
    if (inode_no==-1) {
    	cout << "\nfile not found\n";
    	s=sem_post(&sem);
    	return -1;
    }
    else cout << "\nfile found\n";
    ilist->inode[inode_no].last_read = time(NULL);
    FILE *fp = fopen(dest, "w");
    int total_size = ilist->inode[inode_no].file_size;
    int nwrite = 0;
  //  fwrite (buf + 8, 1, n1 - 8, fp);

    for (int i=0;i<8 ;i++) {
		if (ilist->inode[inode_no].ptr[i]!=-1) {
			nwrite += fwrite(block[ilist->inode[inode_no].ptr[i]],1,min(256,total_size-nwrite),fp);

    	}

	}
	//cout << "wegfw\n";
	if (ilist->inode[inode_no].ptr[8]!=-1) {
    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[8]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		nwrite += fwrite(block[idb->ptr[i]],1,min(256,total_size-nwrite),fp);

	    	}

		}
		//ilist->inode[inode_no].ptr[8]=-1;
	}
	//cout << "qefeq\n";
	if (ilist->inode[inode_no].ptr[9]!=-1) {

    	indirect_block* idb =(indirect_block*)block[ilist->inode[inode_no].ptr[9]];
    	//init_indirect_block(block_no);
    	for (int i=0;i<64 ;i++) {

	    	if (idb->ptr[i]!=-1) {
	    		indirect_block* iidb = (indirect_block*)block[idb->ptr[i]];
	    			    	//init_indirect_block(block_no_i);

		    	for (int i=0;i<64 ;i++) {
		    		if (iidb->ptr[i]!=-1) {

		    			nwrite += fwrite(block[iidb->ptr[i]],1,min(256,total_size-nwrite),fp);

				    }

				}
				//idb->ptr[i]=-1;

			}

		}
		//ilist->inode[inode_no].ptr[9]=-1;
	}
	fclose(fp);
	cout << "file written\n";
	s=sem_post(&sem);
	return 1;
}

int copy_pc2myfs(char* source , char* dest) {
	int s=sem_wait(&sem);
	FILE *filefd = fopen(source, "rb");
    int file_size;
    fseek(filefd, 0, SEEK_END);
    file_size = ftell(filefd);
    fclose(filefd);
    int inode_num ;
    for (int i=0;i<inodes_ub;i++) {
    	if (super->free_inodes[i]==0) {
    		inode_num = i;
    		super->free_inodes.set(i);
    		super->current_inode++;
    		break;
    	}
    }
    int flag=0;
    for (int i=0;i<8;i++) {
    	if (ilist->inode[current_directory].ptr[i]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[i] = j;
		    		init_dir(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
    	directory_type* dir = (directory_type*)block[ilist->inode[current_directory].ptr[i]];
    	for (int j=0;j<8;j++) {
    		if (dir->entry[j].inode==-1) {
    			dir->entry[j].inode = inode_num;
    			strcpy(dir->entry[j].file_name , dest);
    			//cout << dir->entry[j].file_name<<endl;
    			//cout << dir->entry[j].inode<<endl;
    			//cout << ilist->inode[current_directory].ptr[i]<<endl;
    			flag=1;
    			break;
    		}
    	}
    	if (flag)break;
    }
    //cout << "lol\n";
    if (flag==0) {
    	if (ilist->inode[current_directory].ptr[8]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[8] = j;
		    		init_indirect_block(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
		indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[8]];
		for (int i=0;i<64;i++) {
			if (idb->ptr[i]==-1) {
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		idb->ptr[i] = j;
			    		init_dir(j);
			    		super->free_blocks.set(j);
			    		super->current_block++;
			    		break;
			    	}
		    	}
			}
	    	directory_type* dir = (directory_type*)block[idb->ptr[i]];
	    	for (int j=0;j<8;j++) {
	    		if (dir->entry[j].inode==-1) {
	    			dir->entry[j].inode = inode_num;
	    			strcpy(dir->entry[j].file_name , dest);
	    			flag=1;
	    			break;
	    		}
	    	}
	    	if (flag)break;
		}
    }
    if (flag==0) {
    	if (ilist->inode[current_directory].ptr[9]==-1) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		ilist->inode[current_directory].ptr[9] = j;
		    		init_indirect_block(j);
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
		}
		indirect_block* idb =(indirect_block*)block[ilist->inode[current_directory].ptr[9]];
		for (int k=0;k<64;k++) {
			if (idb->ptr[k]==-1) {
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		idb->ptr[k] = j;
			    		init_indirect_block(j);
			    		super->free_blocks.set(j);
			    		super->current_block++;
			    		break;
			    	}
		    	}
			}
			indirect_block* iidb =(indirect_block*)block[idb->ptr[k]];
			for (int i=0;i<64;i++) {
				if (iidb->ptr[i]==-1) {
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		iidb->ptr[i] = j;
				    		init_dir(j);
				    		super->free_blocks.set(j);
				    		super->current_block++;
				    		break;
				    	}
			    	}
				}
		    	directory_type* dir = (directory_type*)block[iidb->ptr[i]];
		    	for (int j=0;j<8;j++) {
		    		if (dir->entry[j].inode==-1) {
		    			dir->entry[j].inode = inode_num;
		    			strcpy(dir->entry[j].file_name , dest);
		    			flag=1;
		    			break;
		    		}
		    	}
		    	if (flag)break;
			}
			if (flag)break;
		}
    }
    ilist->inode[inode_num].file_type = 0;
	ilist->inode[inode_num].file_size = file_size;
	ilist->inode[inode_num].last_mod = time(NULL);
	ilist->inode[inode_num].last_read = time(NULL);
	ilist->inode[inode_num].acess_perm[0]=6;
	ilist->inode[inode_num].acess_perm[1]=6;
	ilist->inode[inode_num].acess_perm[2]=4;
	for (int i=0;i<10;i++)ilist->inode[inode_num].ptr[i]=-1;
	//ifstream file;
	//file.open(source, ios::binary);
	//cout << endl<<endl<<endl;


	FILE *fp = fopen (source, "rb");
    int nread =0;
	//file.unsetf(ios_base::skipws);
	int block_no;
	if (fp!=NULL) {
		for (int i=0;i<8 && !feof(fp);i++) {

			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	nread = fread (block[block_no], 1, 256, fp);
	    	//file  >> block[block_no]->val;
	    	//cout << block_no<<endl;
	    	ilist->inode[inode_num].ptr[i]=block_no;

		}
		if (!feof(fp)) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	//cout << block_no<<endl;
	    	ilist->inode[inode_num].ptr[8]=block_no;
	    	indirect_block* idb =(indirect_block*)block[block_no];
	    	init_indirect_block(block_no);
	    	for (int i=0;i<64 && !feof(fp);i++) {
	    		int block_no_i;
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		 block_no_i = j;
			    		 super->free_blocks.set(j);
			    		 super->current_block++;
		    		break;
			    	}
		    	}
		    	//cout << block_no_i<<endl;
		    	nread = fread (block[block_no_i], 1, 256, fp);
		    	//file  >> block[block_no_i]->val;
		    	//cout << "\n---------------------------------------------------------------------------------------------------------------------------------------------------\n";
		    	idb->ptr[i]=block_no_i;

			}
		}
		if (!feof(fp)) {
			for (int j=0;j<super->max_blocks;j++) {
		    	if (super->free_blocks[j]==0) {
		    		block_no = j;
		    		super->free_blocks.set(j);
		    		super->current_block++;
		    		break;
		    	}
	    	}
	    	//cout << block_no<<endl;
	    	ilist->inode[inode_num].ptr[9]=block_no;
	    	indirect_block* idb =(indirect_block*)block[block_no];
	    	init_indirect_block(block_no);
	    	for (int i=0;i<64 && !feof(fp);i++) {
	    		int block_no_i;
				for (int j=0;j<super->max_blocks;j++) {
			    	if (super->free_blocks[j]==0) {
			    		 block_no_i = j;
			    		 super->free_blocks.set(j);
			    		 super->current_block++;
		    		break;
			    	}
		    	}
		    	//cout << block_no_i<<endl;
		    	idb->ptr[i]=block_no_i;
		    	indirect_block* iidb = (indirect_block*)block[block_no_i];
		    	init_indirect_block(block_no_i);

		    	for (int i=0;i<64 && !feof(fp);i++) {
		    		int block_no_ii;
					for (int j=0;j<super->max_blocks;j++) {
				    	if (super->free_blocks[j]==0) {
				    		 block_no_ii = j;
				    		 super->free_blocks.set(j);
				    		 super->current_block++;
		    		break;
				    	}
			    	}
			    	//cout << block_no<<endl;
			    	nread = fread (block[block_no_ii], 1, 256, fp);
			    	//file  >> block[block_no_ii]->val;
			    	//cout << "\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
			    	iidb->ptr[i]=block_no_ii;

				}

			}
		}



	}
	fclose(fp);
	s=sem_post(&sem);
	return 0;
}

int create_myfs(int size) {
	int s=sem_init(&sem,0,1);
	s=sem_wait(&sem);
	//MB to bytes
	size*=(1<<20);
	if (size < super_size_occupied + inode_list_size_occupied + 256) {
		cout << "The alloted size can not generate the file system";
		s=sem_post(&sem);
		return -1;
	}

	mem_start = new char[size];

	super = (super_type*)mem_start;
	super->total_size = size;
	super->max_inodes = inodes_ub;
	super->current_inode = 1; //root
	super->max_blocks = size/256;
	super->current_block = super_blocks + inode_list_blocks + 1; //block after the utilization for super and inode list
	super->free_inodes.set(0); //sets first bit to 1;

	for (int i=0;i<super_blocks + inode_list_blocks ;i++) {
		super->free_blocks.set(i); //set all the utilized blocks to 1
	}

	ilist = (inode_list_type*)(mem_start + super_size_occupied);
	ilist->inode[0].file_type = 1; //root directory
	ilist->inode[0].file_size = 0;
	ilist->inode[0].last_mod = time(NULL);
	ilist->inode[0].last_read = time(NULL);
	ilist->inode[0].acess_perm[0]=7;
	ilist->inode[0].acess_perm[1]=7;
	ilist->inode[0].acess_perm[2]=5;
	for (int i=0;i<super->max_blocks;i++)
		if (super->free_blocks[i]==0) {
			ilist->inode[0].ptr[0]=i ;
			super->free_blocks.set(i);
			break;
		}

	for (int i=1;i<10;i++)
		ilist->inode[0].ptr[i]=-1;

	for (int i=0;i<super->max_blocks;i++) {
		block[i]=(block_type*)mem_start+i;
		//cout << block[i]<<endl;
	}
	init_dir(ilist->inode[0].ptr[0]);
	current_directory =0;
	s=sem_post(&sem);
	return 0;
}

void testCase1 () {
    int a;
	a=create_myfs(10);
	char c[4];
	for(int i=0;i<12;i++){
		cout<<"copying linear_regression.py to myfs as "<<i+1<<endl;
		sprintf(c,"%d",i+1);
		a=copy_pc2myfs((char *) "linear_regression.py",c);
	}
	a=ls_myfs();
	cout<<"enter the name of the file to delete \n";
	string d;
	cin>>d;
	a=rm_myfs((char*)(d.c_str()));
	a=ls_myfs();
}

void testCase2 () {
    int a;
	a=create_myfs(10);
	int fd= open_myfs((char *) "mytest.txt",'w');
	int num[100];
	for (int i = 0; i < 100; i++) {
		num[i]=rand();
	}
	string s="";
	for(int i=0;i<100;i++) {
		s+="--"+to_string(num[i]);
	}
	char ch[s.length()+1];
	memcpy(ch, s.c_str(), s.size() + 1);
	a=write_myfs(fd,s.length(),ch);
	a=close_myfs(fd);
	cout<<"enter the number of copies to be generated\n";
	int n;
	cin>>n;
	string name;
	char nm[name.length()+1];
	fd= open_myfs((char *) "mytest.txt",'r');
	a=read_myfs(fd,s.length(),ch);
	a=close_myfs(fd);
	for(int i=0;i<n;i++){
		name="mytest-"+to_string(i+1);
		memcpy(nm, name.c_str(), name.size() + 1);
		fd=open_myfs(nm,'w');
		a=write_myfs(fd,s.length(),ch);
		a=close(fd);
	}
	a=ls_myfs();
	a=dump_myfs((char *) "mydump-41.backup");
}

void testCase3 () {
    int a;
	a=restore_myfs((char *) "mydump-41.backup");
	a=ls_myfs();
	int fd;
	fd=open_myfs((char *) "mytest.txt",'r');
	char ch[10000];
	int n=read_myfs(fd,10000,ch);
	a=close_myfs(fd);
	print(ch,n);
	cout<<endl;
	std::string::size_type sz;
	vector<int>vec;
	int z;
	string temp;
	for(int i=0;i<n;i++){
		if(ch[i]=='-'){
			temp="";
			for(int j=i+2;j<n;j++){
				if(ch[j]!='-')temp+=ch[j];
				else break;
			}
			z=std::stoi(temp);
			vec.push_back(z);
			i++;
		}
	}
	sort(vec.begin(),vec.end());

	fd= open_myfs((char *) "sorted.txt",'w');
	string s="";
	for(int i=0;i<vec.size();i++){
		s+="--"+to_string(vec[i]);
	}

	memcpy(ch, s.c_str(), s.size() + 1);
	a=write_myfs(fd,s.length(),ch);
	a=close_myfs(fd);
	a=ls_myfs();
	a=showfile_myfs((char *) "sorted.txt");
}

int testCase4 () {
    int a;
	a=create_myfs(10);
	a=mkdir_myfs((char *) "mydocs");
	a=mkdir_myfs((char *) "mycode");
	a=chdir_myfs((char *) "mydocs");
	a=mkdir_myfs((char *) "mytext");
	a=mkdir_myfs((char *) "mypapers");
	a=chdir_myfs((char *) "dotdot");
	a=ls_myfs();
	pid_t p = fork();
	if(p<0)
	{
		fprintf(stderr, "fork Failed" );
    	return 1;
	}
	else if(p==0)
	{
		chdir_myfs((char *) "mydocs");
		chdir_myfs((char *) "mytext");
		int fd = open_myfs((char *) "file.txt",'w');
		char c[26];
		for(int i=0;i<26;i++){
			c[i]='A'+i;
		}
		int a=write_myfs(fd,26,c);
		a=close_myfs(fd);
		a=showfile_myfs((char *) "file.txt");
		cout<<endl;
		exit(0);
	}

	else
	{
		wait(NULL);
		a=chdir_myfs((char *) "mycode");
		a=copy_pc2myfs((char *) "makefile", (char *) "testfile");
		a=ls_myfs();
		a=showfile_myfs((char *) "testfile");
	}
}

int main () {
    cout << "Test case 1" << endl;
    testCase1 ();
    cout << "Test case 2" << endl;
    testCase2 ();
    cout << "Test case 3" << endl;
    testCase3 ();
    cout << "Test case 4" << endl;
    testCase4 ();
    return 0;
}
