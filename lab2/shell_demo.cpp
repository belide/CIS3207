#include <stdio.h>
#include <cstdlib>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define BUFFERSIZE 200

char buffer[BUFFERSIZE];
char cwd[BUFFERSIZE];
char *prompt;
char *a = ">";

int hasPrefix(char const *p, char const *q)
{
  int i = 0;
  for(i = 0;q[i];i++)
  {
      if(p[i] != q[i])
          return -1;
  }
  return 0;
}

int cd(char *pth)
{
  char path[BUFFERSIZE];
  strcpy(path,pth);

  char cwd[BUFFERSIZE];
  if(pth[0] != '/')
  {// true for the dir in cwd
    getcwd(cwd,sizeof(cwd));
    strcat(cwd,"/");
    strcat(cwd,path);
    int success=chdir(cwd);
    if(success!=0)printf("not found\n");
  }else{//true for dir w.r.t. /
    int success=chdir(pth);
    if(success!=0)printf("not found\n");
  }

  return 0;
}

void listenviron(char **envp)
{
    int i = 0;
  /*printf("%d\n",argc);
  for(int i=0;i<argc;i++)
  {
    printf("%s\n",argv[i]);
  }*/
  i = 1;
  char *s = *envp;
  while(s)
  {
    printf("%s\n", s);
    s = *(envp+i);
    i++;
  }
}

void ls()
{
  DIR *mydir;
  struct dirent *myfile;
  struct stat mystat;
  char buf[512];
  prompt=getcwd(cwd,sizeof(cwd));
  mydir = opendir(prompt);
  while((myfile = readdir(mydir)) != NULL)
  {
    sprintf(buf, "%s/%s", prompt, myfile->d_name);
    stat(buf, &mystat);
    printf("%zu",mystat.st_size);
    printf(" %s\n", myfile->d_name);
  }
  closedir(mydir);
}

int main ( int argc, char **argv,char ** envp ) {
  // code



  char *tok;
  tok = strtok (buffer," ");


  while(buffer != NULL)
  {
    bzero(buffer, BUFFERSIZE);
    prompt=getcwd(cwd,sizeof(cwd));
    printf("%s%s",prompt,a);
    fgets(buffer, BUFFERSIZE, stdin);
    if(hasPrefix(buffer,"cd") == 0)
    {
      tok = strchr(buffer,' '); //use something more powerful
      if(tok) {
        char *tempTok = tok + 1;
        tok = tempTok;
        char *locationOfNewLine = strchr(tok, '\n');
        if(locationOfNewLine) {
            *locationOfNewLine = '\0';
        }
        cd(tok);
      }
    }
    else if(hasPrefix(buffer,"ls") == 0)
    {
        ls();
    }
    else if(hasPrefix(buffer,"env") == 0)
    {
        listenviron(envp);
    }
    else if(hasPrefix(buffer,"quit") == 0)
    {
        break;
    }
  }
  return 0; // Indicates that everything went well.
}