#include <dirent.h>
#include <stdio.h>
	
main( )
{
  DIR *dp;
  struct dirent *dir;

  if((dp = opendir(".")) == NULL) {
    fprintf(stderr, "directory open error\n");
    exit(-1);
  }

  while((dir = readdir(dp)) != NULL) {
   if(dir->d_ino == 0) continue;
     printf("%s\n", dir->d_name);
  }
  closedir(dp);
}
