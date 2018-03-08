
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include<dirent.h>

int main(void) 
{ 
  int lg,run,nr_c; 
  char input[70],cmd[10],prm[60],c;
  run=1;
  lg=0;

  nr_c=0;

  while(run)
  {
    prm[0]='\0';
    cmd[0]='\0';
    input[0]='\0';
    printf("\nCOMANDA(cu tot cu parametrii):");
    scanf("%[^\n]%*c",input);
    separa (input,cmd,prm);
    if(input!='\0')
      nr_c=verificare(cmd,lg,run); 

   

    if(nr_c==5)
    { nr_c=0;
      lg=0;
      printf("\nTe-ai delogat cu succes!\n");
    }
    else if(nr_c==4)//quit
    {
      printf("\nLa revedere!\n\n");
      exit(0);
    }
      else if(nr_c==1)//login
      {
       int pfd[2];
       int sok[2];
       char rsp[2];
       if (socketpair(AF_UNIX, SOCK_STREAM, 0, sok) < 0) 
       { 
        perror("Err... socketpair"); 
        exit(1); 
      }
      if(!fork())
      {
        int file,i=0,ok;
        file=open("user.txt",O_RDONLY);
        char user[20],ch,sign;
        ok=0;
        while(read(file,&ch,1) && ok==0)
        {
          if(ch=='\n')
          {
            user[i]='\0';
            if(strcmp(prm,user)==0)
              ok=1; 
            i=0;
          }
          else
          {
            user[i]=ch;
            i++;
          }
        }
        close(file);
        close(sok[1]);
        //printf("\nOK:%d\n>",ok);
        if(ok==1)
          if(write(sok[0], "1", 1)<0)perror("ER1 W PIPE:");
        else
          if(write(sok[0], "0", 1)<0)perror("ER0 W PIPE:");
        close(sok[0]);
        close(sok[0]);
        exit(1);

      }
      else
      {

        close(sok[0]);
        
        read(sok[1],rsp, 1);
        rsp[1]='\0';
        //printf("RSP:%s\n");
        if(strcmp(rsp,"1")==0)
          lg=1;
        close(sok[1]);

        if(lg==1)
        {
          for(int j=0;j<strlen(prm)+6;j++)
            printf("-");
          printf("\nBuna, %s!\n",prm);
          for(int j=0;j<strlen(prm)+6;j++)
            printf("-");

        }
        else
          printf("\n Logarea a esuat, cont inexistent!\n");

      }
         //stop login
    }
    else if(nr_c==3)
    {
                              //myfind
      int ppfd[2],ftd;
      pipe(ppfd);
      char dirn[50],ch;
      ch=NULL;
      if(!fork())
      {
        ftd = open("fif.txt", O_WRONLY | O_CREAT | O_TRUNC);
        getwd(&dirn);
        close(1);
        dup(ftd);
        close(0);
        cautare(dirn,prm);

        exit(1);
      }
      wait(NULL);
      mknod("fif.txt", S_IFIFO | 0666, 0);
      ftd = open("fif.txt", O_RDONLY);
      while(read(ftd,&ch,1))
      {
        printf("%c",ch);
      }
      if(ch==NULL)
        printf("\n Fisierul nu a fost gasit.\n");
      else
      {
        nr_c=2;
      }

      close(ppfd[0]);

    }
if(nr_c==2)//mystat
{

 int pcp[2],i;
 char ch;
 char dim[15],data[15],drp[22],af[10];

 int tmp=0;
 time_t t;

 if (pipe(pcp) < 0) 
 { 
  perror("ErrPCP... socketpair"); 
  exit(1); 
}


if(!fork())
{  
  struct stat detaliFisier;
  close(pcp[0]);
  if(stat(prm,&detaliFisier) < 0) 
  {
    perror("#ERR MYSTAT:");
    if(write(pcp[1],"0",1)<0)perror("ERRRR WRITE:");
    exit(0);

  }
  else
  {  
   if(write(pcp[1],"1",1)<0)perror("ERRRR WRITE:");

   struct tm *tmpp;
   t = time(NULL);
   tmpp=localtime(&t);
   tmp=detaliFisier.st_size;
   sprintf(dim, "%d",tmp);


   if (write(pcp[1],dim,sizeof(dim)) < 0) perror("[copil]Err...write1");

   if(strftime(data, 15,"%d . %m . 20%y", tmpp)==0)perror("#ERR STRFIME:");
   if (write(pcp[1],data,sizeof(data)) < 0) perror("[copil]Errd...write");
   sprintf(drp, "%d",detaliFisier.st_mode);
   if (write(pcp[1],drp,sizeof(drp)) < 0) perror("[copil]ErrR...write");
   close(pcp[1]);
   exit(1);

 }


}
else
{
  close(pcp[1]);
  if(read(pcp[0],&ch,1)<0)perror("err READ:");
  if(ch=='1')
  {
    printf("\n           Detalii %s:",prm);
    printf("\n------------------------------------\n");
    ;
    i=0;
    while(read(pcp[0],&ch ,1)&& ch>='0')
    { 
      dim[i]=ch;
      i++;
    }
    dim[i]='\0';
    tmp=atoi(dim);
    printf("Marimea:%d bytes\n", tmp); 
    ch=0;
    i=0;
    while(read(pcp[0],&ch ,1)&&i<10)
    { 
      if(ch>='0'&&ch<='9'||ch=='.')
      {
        data[i]=ch;
        i++;}
      }
      data[i]='\0';
      printf("Data ultimei modificari:%s\n",data);
      i=0;
      while(read(pcp[0],&ch ,1)&& ch>='0'&&ch<='9')
      {
        drp[i]=ch;
        i++;
      }
      drp[i]='\0';
      tmp=atoi(drp);
      printf("Timpul:");
      if(S_ISDIR(tmp)) printf("director\n");
      if(S_ISREG(tmp)) printf("fisier\n");
      printf("Drepturi:");
      
      if(tmp & S_IRUSR)printf("r");else printf("-");
      if(tmp & S_IWUSR)printf("w");else printf("-");
      if(tmp & S_IXUSR)printf("x");else printf("-");
      if(tmp & S_IRGRP)printf("r");else printf("-");
      if(tmp & S_IWGRP)printf("w");else printf("-");
      if(tmp & S_IXGRP)printf("x");else printf("-");
      if(tmp & S_IROTH)printf("r");else printf("-");
      if(tmp & S_IWOTH)printf("w");else printf("-");
      if(tmp & S_IXOTH)printf("x");else printf("-");
      printf("\n------------------------------------\n");

     // wait(NULL);
    }
    close(pcp[0]);
  }

}


} 

return 0;
}

void separa(char inp[70],char cmd[10], char prm[60])//separa inputul in comnada + parametrii
{


  int i,j;

  i=0;
  while(inp[i]!=' ' && inp[i]!=NULL)
  {
    cmd[i]=inp[i];
    i++;


  }
  cmd[i]='\0';
  if(inp[i]==' ')
    i++;
  j=0;

  while(inp[i]!=NULL)
  {
    prm[j]=inp[i];
    i++;
    j++;
  }
  prm[j]='\0';

}  

int verificare (char cmd[10],int lg,int run)//verifica ce comanda e si daca se poate apela
{ 
  if(strcmp(cmd,"login")==0)
  {
    if(lg==0)
    {
      return 1; 
    }
    else          
      printf("\n[COMANDA INVALIDA]<->Nu te poti loga peste un alt user!\n");
  }
  if(strcmp(cmd,"logout")==0)
  {
    if(lg==1)
    {
      return 5; 
    }
    else
      printf("\n[COMANDA INVALIDA]<->Nu te poti deloga deoarece nu esti logat!\n");
  }

  if (strcmp(cmd,"quit")==0)
  {
    lg=0;
    run=0;
    return 4;
    
  }

  if (strcmp(cmd,"myfind")==0)
  {
    if(lg==0)
      printf("\n[COMANDA INVALIDA]<->Nu esti logat.\n");
    else
    {
      return 3;
    }
  }

  if (strcmp(cmd,"mystat")==0)
  {
    if(lg==0)
      printf("\n[COMANDA INVALIDA]<->Nu esti logat.\n");
    else
    {
      return 2;
    }
  }
  if(strcmp(cmd,"login")!=0 && strcmp(cmd,"quit")!=0 && strcmp(cmd,"myfind")!=0 && strcmp(cmd,"mystat")!=0 && strcmp(cmd,"logout")!=0)
  {
    printf("\n[COMANDA INEXISTENTA]\n");
    return 0;
  }
  return 0;
}

int cautare(char dirn[50],char fil[50])
{
  int f;
  char dire[50],buff[50];
  DIR *dir = NULL;
  DIR *dir2=NULL;
  struct dirent *drnt = NULL;

  dir=opendir(dirn);
  if(dir)
  {
   while(drnt = readdir(dir))
   { 



    if(strcmp(fil,drnt->d_name)==0)
    {
      printf("\nFisierul a fost gasit.\n");
      return 1 ; 

    }

    if(is_dir(drnt->d_name)==1 && strcmp(drnt->d_name,".")!=0 && strcmp(drnt->d_name,"..")!=0)
      cautare( drnt->d_name,fil)  ;
  }
  closedir(dir);
}
else

  return 0;
}

int is_dir(const char *path)
{
  struct stat st_path;
  stat(path, &st_path);
  return S_ISDIR(st_path.st_mode);
}
