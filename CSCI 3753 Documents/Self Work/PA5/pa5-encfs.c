/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.

*/

/*
Sources: 
Collaborated with Monte Anderson and Chris Rhoda
*/

#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
#include "aes-crypt.h"

#define NEW_DATA ((new_state *) fuse_get_context()->private_data)


typedef struct {
    char *mirror_dir;
    char *encrypt_key;
}new_state;

char* new_path(const char *old_path){
	int size = strlen(old_path) + strlen(NEW_DATA->mirror_dir) + 1;	
	
	char* dir_path = malloc(sizeof(char) * size);	//allocate correct space for the new directory path			
	
	return strcat(strcpy(dir_path, NEW_DATA->mirror_dir), old_path);
	
}

static int xmp_getattr(const char *old_path, struct stat *stbuf)
{
	char* path = new_path(old_path);
	int res;
	res = lstat(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *old_path, int mask)
{
	char* path = new_path(old_path);
	int res;

	res = access(path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *old_path, char *buf, size_t size)
{
	char* path = new_path(old_path);
	int res;
	

	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *old_path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char* path = new_path(old_path);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *old_path, mode_t mode, dev_t rdev)
{
	char* path = new_path(old_path);
	int res;

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *old_path, mode_t mode)
{
	char* path = new_path(old_path);
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *old_path)
{
	char* path = new_path(old_path);
	int res;

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *old_path)
{
	char* path = new_path(old_path);
	int res;

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *old_path, mode_t mode)
{
	char* path = new_path(old_path);
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *old_path, uid_t uid, gid_t gid)
{
	char* path = new_path(old_path);
	int res;

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *old_path, off_t size)
{
	char* path = new_path(old_path);
	int res;

	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *old_path, const struct timespec ts[2])
{
	char* path = new_path(old_path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *old_path, struct fuse_file_info *fi)
{
	char* path = new_path(old_path);
	int res;

	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *old_path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{

	/*
	One of the three main methods we need modify for file system encryption
	The other two are write, and create.
	
	All read does is decrypt the file when we want to read it through a command
	such as echo or cat.
	*/
	
	const char* path = new_path(old_path);
	int res;
	
	/*
	We originally check if the xattribute of the file is greater or equal to 0
	This means that the folowing file is encrypted and we will need to decrypt
	and then read the file.
	*/
	if(getxattr(path, "user.pa5-encfs.encryption", NULL, 0) >= 0)
	{
		//Initialize file variables
		FILE* inFile, *temp;
		(void) fi;
	
		/*
		We create a temporary file since we don't want to decrypt the encrypted file
		permenantly, so we place the decrypted files into a temporary file then read it
		*/
		inFile = fopen(path, "r"); //Open the file with only read permissions, not updating
		temp = tmpfile(); //tmpfile() creates a temporary file
		
		//Decrypt the contents, and place the decryption in temp
		if(do_crypt(inFile, temp, 0, NEW_DATA->encrypt_key) == 0)
		{
			printf("Decrypt Failure!\n");
		}
		
		//Clear the buffer within temp
		fflush(temp);
		
		//fseek gets the very beginning of the file and starts to read
		//fseek with SEEK_SET does a similar thing as rewind() which goes to the beginning
		fseek(temp, offset, SEEK_SET);
		res = fread(buf, 1, size, temp); //Read the contents

		if (res == -1)
			res = -errno;
		
		//Close both files
		fclose(temp);
		fclose(inFile);
	
	}
	/*
	If the file is not encrypted after examining the xattribute, we
	just use the original code and just read the file 
	*/
	else 
	{
		int fd;
		fd = open(path, O_RDONLY);
		if (fd == -1) return -errno;
		
		res = pread(fd, buf, size, offset);
		if (res == -1) res = -errno;
		close(fd);
	}
	return res;
}

static int xmp_write(const char *old_path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	//Initialize variables
	const char* path = new_path(old_path);
	int fd;
	int res;
	(void) fi;
	
	/*
	Needed modification for file system encryption
	Similar to read above, but requires decryption AND encryption

	To write to a encrypted file, we would first need to decrypt the file
	write to it, THEN re-encrypt the file
	*/
	if(getxattr(path, "user.pa5-encfs.encryption", NULL, 0) >= 0) //Check xattribute to see if encrypted, which was set in xmp_create
	{
		//Create a temporary file again with similar intent as above
		FILE* inFile, *temp;
		temp = tmpfile();
		
		/*
		Open the file with r+, or read/update properties
		fseek to the beginning of the file, then start decrypting with the key
		*/
		inFile = fopen(path, "r+");	
		fseek(inFile, 0, SEEK_SET);
		
		do_crypt(inFile, temp, 0, NEW_DATA->encrypt_key);
		
		/*
		Once the file is decrypted, go again back to the beginning with fseek
		and then start writing the data to the file
		*/
		fseek(inFile, 0, SEEK_SET);
		res = fwrite(buf, 1, size, temp);
		
        	if (res == -1)
           		 return -errno;
        
		/*
		Once finished writing to the file, we re-encrypt the file with the key,
		first fseeking to the beginning, then encrypting.
		*/
       		fseek(temp, 0, SEEK_SET);    
        	do_crypt(temp, inFile, 1, NEW_DATA->encrypt_key);
        	
		//Close the files
        	fclose(inFile);
        	fclose(temp);
	}
	/*
	If the file is not encrypted from looking at the xattribute of the file,
	just run the original code to write to the file.
	*/
	else 
	{	
		fd = open(path, O_WRONLY);
		if (fd == -1)
			return -errno;

		res = pwrite(fd, buf, size, offset);
		if (res == -1)
			res = -errno;

		close(fd);
	}
	return res;
}

static int xmp_statfs(const char *old_path, struct statvfs *stbuf)
{
	char* path = new_path(old_path);
	int res;

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* old_path, mode_t mode, struct fuse_file_info* fi) {

	/*Initialize variables*/	
	char* path = new_path(old_path);
    	(void) fi;
	FILE* inFile;
    	int res;
    	res = creat(path, mode);
    	if(res == -1)
	{
		return -errno;
	}

	/*
	Open with the r+ argument, which stands for read/update 
	Allows for both new inputs and outputs
	*/
	inFile = fopen(path, "r+"); 
	
	/*
	Encrypt the following file with the key the user inputted
	do_crypt takes a in and out file, and either encrypts or decrypts
	the file with 1 or 0 respectively. crypt is done with the user
	inputted key
	*/
	if(do_crypt(inFile, inFile, 1, NEW_DATA->encrypt_key) == 0)
	{	
		//If the encryption fails
		printf("Encryption Method Error!\n");
		return 1;
	}

	fclose(inFile);
    	close(res);
    	
	/*
	We then set the xattribute to another number to signify that following file that
	we created in the mount point is encrypted. This allows for easier reading and writing of
	the file in the other two functions above.

	Basically lets us know if the file is encrypted or not. 
	*/
    	if(setxattr(path, "user.pa5-encfs.encryption", "true", sizeof(int), 0) == -1)
	{		
		printf("Failure to set xattribute in create!\n");
	}
    return 0;
}


static int xmp_release(const char *old_path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
	
	(void) old_path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *old_path, const char *name, const char *value,
			size_t size, int flags)
{
	char* path = new_path(old_path);
	int res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *old_path, const char *name, char *value,
			size_t size)
{
	char* path = new_path(old_path);
	int res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *old_path, char *list, size_t size)
{
	char* path = new_path(old_path);
	int res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *old_path, const char *name)
{
	char* path = new_path(old_path);
	int res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create         = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
};

int main(int argc, char *argv[])
{
	umask(0);
	
	if(argc < 4)
	{
		fprintf(stderr, "INPUT ERROR: Not enough arguments.\n");		
		fprintf(stderr,"Usage: ./pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point>\n");
		fprintf(stderr,"Usage 2 (foreground): ./pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point> -f \n");
		return EXIT_FAILURE;
	}
	if(argc > 4)
	{
		fprintf(stderr, "INPUT ERROR: Too many arguments.\n");
		fprintf(stderr, "Usage: ./pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point>\n");
		fprintf(stderr,"Usage 2 (foreground): ./pa5-encfs <Key Phrase> <Mirror Directory> <Mount Point> -f \n");
		return EXIT_FAILURE;
	}
	
	/*
	Create the path struct for the mirror directory, and allocate the memory require for the new struct.
	*/ 
	new_state* newDir = NULL;
	newDir = malloc(sizeof(new_state));
	
	/*If the memory allocation fails  */
	if(newDir == NULL){ 
		fprintf(stderr, "Memory allocation error.\n");
		return EXIT_FAILURE;
	}
	
	/*
	Set the pointers for the mirror directory struct
	We set the mirror_dir argument to be the path to the directory we want to mirror, using realpath() 
	which gets the path for us.

	We set the encrypt_key arugment to be the Key Phrase, arguments are modeled off the design in the argc 
	if statements above.
	*/
	newDir->mirror_dir = realpath(argv[2], NULL);
	newDir->encrypt_key = argv[1];
	
	/*
	The fuse_main() function first takes the argument count and arguments.
	In our case, we want the mirror directory and mount point, we are argv[2] and argv[3] respectively
	So argv+2 gets those two arguments.

	Then the pre-written xmp_oper struct above, and lastly, the newDir struct we created and added
	arguments too.
	
	*/
	return fuse_main(argc-2, argv+2, &xmp_oper, newDir);
}

