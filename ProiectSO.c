#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>

void enunturi(char *path, char *name, struct stat *st_file)
{
  char abc[3000];

//nume pt fisier
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode) && strstr(path,".bmp"))) strcpy(abc,"nume fisier: ");  

  //nume pt legatura
  if(S_ISLNK(st_file->st_mode)) strcpy(abc,"nume legatura: ");

  //nume pt director
  if(S_ISDIR(st_file->st_mode)) strcpy(abc,"nume director: ");

  strcat(abc,name);
  strcat(abc,"\n");

  //dimensiuni fisier
  char cont[100];
  if(S_ISREG(st_file->st_mode)&& strstr(path,".bmp"))
    {
      int fd;

      //verificare deschidere
      fd=open(path,O_RDONLY);
      if(fd==-1)
        {
          perror("eroare deschidere");
          exit(1);
        }


      //verificare inaltime
      int inaltime;
      lseek(fd,18,SEEK_SET);
      if(read(fd,&inaltime,sizeof(int))!=sizeof(int))
        {
          perror("eroare citire");
          if(close(fd)==-1)
          {
            perror("eroare inchidere");
            exit(1);
          }
          exit(1);
        }

        //afisare inaltime
      strcat(abc,"inaltime: ");
      sprintf(cont,"%d\n",inaltime);
      strcat(abc,cont);

        //verificare lungime
        int lungime;
      if(read(fd,&lungime,sizeof(int))!=sizeof(int))
        {
          perror("eroare citire");
          if(close(fd)==-1)
          {
            perror("eroare inchidere");
            exit(1);
          }
          exit(1);
        }

      //afisare lungime
      strcat(abc,"lungime: ");
      sprintf(cont,"%d\n",lungime);
      strcat(abc,cont);

      //verificare inchidere
      if(close(fd)==-1)
        {
          perror("eroare inchidere");
          exit(1);
        }
    }

  //legatura - dimensiune
    struct stat st_link;
  if(lstat(path,&st_link) == -1)
    {
      perror("eroare stat");
      exit(1);
    }

  if(S_ISLNK(st_link.st_mode))
    {
      sprintf(cont,"dimensiune legatura: %ld\n",st_link.st_size);  //afisarea dimensiunii legaturii simbolice
      strcat(abc,cont);
    }

  //dim fisier bmp sau legatura sim
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")) || S_ISLNK(st_file->st_mode))
    {
      strcat(abc,"dimensiune: ");
      sprintf(cont,"%ld\n",st_file->st_size);
      strcat(abc,cont);
    }

  //ID fisier, director
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")) || S_ISDIR(st_file->st_mode))
    {
      strcat(abc,"identificatorul utilizatorului: ");
      sprintf(cont,"%d\n",st_file->st_uid);
      strcat(abc,cont);
    }

  //timp fisier 
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")))
    {
      strcat(abc,"timpul ultimei modificari: ");
      strcat(abc,ctime(&st_file->st_mtime));
    }

  //contorul de legaturi fisier
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")))
  {
    strcat(abc,"contorul de legaturi: ");
    sprintf(cont,"%ld\n",st_file->st_nlink);
    strcat(abc,cont);
  }

  //drepturi de acces 
  
  //user
  strcat(abc,"drepturi de acces user: ");
  sprintf(cont,"%s%s%s\n", st_file->st_mode & S_IRUSR ? "R" : "-",st_file->st_mode & S_IWUSR ? "W" : "-",st_file->st_mode & S_IXUSR ? "X" : "-");
  strcat(abc,cont);

    //grup
  strcat(abc,"drepturi de acces grup: ");
  sprintf(cont,"%s%s%s\n", st_file->st_mode & S_IRGRP ? "R" : "-",st_file->st_mode & S_IWGRP ? "W" : "-",st_file->st_mode & S_IXGRP ? "X" : "-");
  strcat(abc,cont);

    //altii
  strcat(abc,"drepturi de acces altii: ");
  sprintf(cont,"%s%s%s\n",st_file->st_mode & S_IROTH ? "R" : "-",st_file->st_mode & S_IWOTH ? "W" : "-",st_file->st_mode & S_IXOTH ? "X" : "-");
  strcat(abc,cont);
  strcat(abc,"\n");

//fisier statistica
  char *output="statistica.txt";
  struct stat st_output;

  if(stat(output,&st_output)==-1){
    perror("eroare");
    exit(1);
  }

  int f1;
  f1=open(output,O_WRONLY | O_APPEND);

  if(f1==-1)
    {
      perror("eroare deschidere");
      exit(1);
    }

  if(write(f1,abc,strlen(abc))==-1)
    {
      perror("eroare scriere");
      exit(1);
    }

  if(close(f1)==-1)
    {
      perror("eroare inchidere");
      exit(1);
    }
}

//citirea directorului
void citireD(char *D)
{

  //deschidem directorul si verificam daca e null
  DIR *dir;
  dir=opendir(D);
  if(dir==NULL)
    {
      perror("eroare deschidere dir");
      exit(1);
    }

  struct dirent *entry=NULL;

  while((entry=readdir(dir))!=NULL)
    {
      if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0)
        {
          struct stat file;
          char path[300]={};
          sprintf(path,"%s/%s",D,entry->d_name);

          if(stat(path,&file)==-1)
            {
              perror("eroare stat");
              exit(1);
            }
          if(S_ISDIR(file.st_mode))
            {
              enunturi(path, entry->d_name, &file);
              citireD(path);
            }
          else
              enunturi(path,entry->d_name,&file);
        }
    }
//verificare inchidere director
  if(closedir(dir)==-1){
    perror("eroare inchidere dir");
    exit(1);
  }
}

int main(int argc,char*argv[])
{
  if(argc!=2)
    {
      perror("nr de argumente invalid");
      exit(1);
    }
  citireD(argv[1]);
  return 0;
}