#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<time.h>
#include<sys/wait.h>

char* drepturi(char *abc, mode_t mod)
{
  char cont[100];
 
  //drepturi de acces
 
  //user
  strcat(abc,"drepturi de acces user: ");
  sprintf(cont,"%s%s%s\n", mod & S_IRUSR ? "R" : "-",mod & S_IWUSR ? "W" : "-",mod & S_IXUSR ? "X" : "-");
  strcat(abc,cont);
 
  //grup
  strcat(abc,"drepturi de acces grup: ");
  sprintf(cont,"%s%s%s\n", mod & S_IRGRP ? "R" : "-",mod & S_IWGRP ? "W" : "-",mod & S_IXGRP ? "X" : "-");
  strcat(abc,cont);
 
  //altii
  strcat(abc,"drepturi de acces altii: ");
  sprintf(cont,"%s%s%s\n",mod & S_IROTH ? "R" : "-",mod & S_IWOTH ? "W" : "-",mod & S_IXOTH ? "X" : "-");
  strcat(abc,cont);
  strcat(abc,"\n");
 
  return abc;
}

void statistici(char *nume_fis, char* abc, char *dir_out)
{
  //fisier statistica

  char nume[300]={};
  DIR *dirOut = NULL;
  dirOut = opendir(dir_out);
 
  if(dirOut == NULL)
    {
      perror("eroare deschidere director scriere\n");
      exit(1);
    }
 
  strcpy(nume,dir_out);
  strcat(nume,"/");
  strcat(nume,nume_fis);
  strcat(nume,"_statistica.txt");
 
  char *output=nume;
  struct stat st_output;
 
  int f1;
  f1=open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);
 
  if(f1==-1)
    {
      perror("eroare deschidere statistici\n");
      exit(1);
    }

  if(stat(output,&st_output)==-1)
    {
      perror("eroare stat (statistici)\n");
      exit(1);
    }
 
  if(write(f1,abc,strlen(abc))==-1)
    {
      perror("eroare scriere\n");
      exit(1);
    }
 
  if(close(f1)==-1)
    {
      perror("eroare inchidere statistici\n");
      exit(1);
    }
 
  if (closedir(dirOut) == -1)
    {
      perror("eroare inchidere director");
      exit(1);
    }
 
}

void fisier(char* path, char* name,struct stat *st_file,char *dir_out)
{
  char abc[3000];
 
  //nume pt fisier
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode) && strstr(path,".bmp"))) strcpy(abc,"nume fisier: ");  
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
 
  //dim fisier bmp
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")))
    {
      strcat(abc,"dimensiune: ");
      sprintf(cont,"%ld\n",st_file->st_size);
      strcat(abc,cont);
    }
 
  //ID fisier, director
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")))
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
  drepturi(abc, st_file->st_mode);
 
  //statistici
  statistici(name, abc, dir_out);
}

void legatura(char* path, char* name, struct stat *st_file, struct stat *st_leg, char *dir_out)
{
  char abc[3000];
  char cont[100];
 
  //nume pt legatura
  if(S_ISLNK(st_file->st_mode)) strcpy(abc,"nume legatura: ");
  strcat(abc,name);
  strcat(abc,"\n");
 
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
 
  //dim legatura sim
  if(S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode)&& strstr(path,".bmp")) || S_ISLNK(st_file->st_mode))
    {
      strcat(abc,"dimensiune: ");
      sprintf(cont,"%ld\n",st_file->st_size);
      strcat(abc,cont);
    }
 
  //drepturi de acces
  drepturi(abc, st_leg->st_mode);

  //statistici
  statistici(name, abc, dir_out);
}

void director(char *name, struct stat *st_file, char *dir_out)
{
  char abc[3000];
  char cont[100];
 
  //nume pt director
  if(S_ISDIR(st_file->st_mode)) strcpy(abc,"nume director: ");
  strcat(abc,name);
  strcat(abc,"\n");

  //ID  director

  strcat(abc,"identificatorul utilizatorului: ");
  sprintf(cont,"%d\n",st_file->st_uid);
  strcat(abc,cont);
   

  //drepturi de acces
  drepturi(abc, st_file->st_mode);

  //statistici
  statistici(name, abc, dir_out);
}

void wait_process(char *dir_out)
{
  int PID;
  int index;
 
  while((PID = wait(&index)) != -1)
  {
    /*
    if(PID == -1)
      {
        perror("Eroare process\n");
        exit(-1);
      }
    */

    if(WIFEXITED(index))
      printf("S-a incheiat procesul cu pid-ul %d si codul %d\n",PID,WEXITSTATUS(index));

    else printf("Procesul cu PID %d nu s-a terminat corect\n", PID);
  }
}

void procesPixeli(char* name, char *path, char *dir_out)
{
  int PID = fork();
 
  if (PID < 0)
    {
      perror("Eroare fork\n");
      exit(-1);
    }
 
  char nume[300];
  int lungPH;
  int inaltPH;

  if(PID == 0)
    {
      strcpy(nume,path);
      strcat(nume,"/");
      strcat(nume,name);

      char *fis=path;
     
      int photo;
      photo = open(fis,O_RDWR);
     
      if(photo==-1)
{
 perror("eroare deschidere poza\n");
 exit(1);
}
     
      lseek(photo, 18, SEEK_SET);
     
      if(read(photo,&inaltPH, sizeof(int))!=sizeof(int))
{
 perror("eroare citire");
 
 if(close(photo)==-1)
   {
     perror("eroare inchidere");
     exit(1);
   }
 
 exit(1);
}
     
      if(read(photo,&lungPH, sizeof(int))!=sizeof(int))
{
 perror("eroare citire");
 
 if(close(photo)==-1)
   {
     perror("eroare inchidere");
     exit(1);
   }
 
 exit(1);
}
     
      int PIX = lungPH*inaltPH;
      char InfoHeader[54];
      unsigned char P_color[3];
      unsigned char P_gri;
     
      if (read(photo, InfoHeader, sizeof(InfoHeader)) != sizeof(InfoHeader))
{
 perror("eroare citire header\n");
 exit(1);
}
     
      for(int i=0; i<PIX; i++)
{
 P_gri=0.299*P_color[0]+0.587*P_color[1]+0.114*P_color[2];
 
 if(read(photo,P_color,sizeof(P_color))!=sizeof(P_color))
   {
     perror("Pixeli - eroare citire\n");
     exit(1);
   }
 
 lseek(photo,-3,SEEK_CUR);
 
 write(photo,&P_gri,sizeof(P_gri));
 write(photo,&P_gri,sizeof(P_gri));
 write(photo,&P_gri,sizeof(P_gri));
}
     
      if(close(photo)==-1)
{
 perror("error inchidere poza");
 exit(1);
}
     
      exit(-1);
    }
 
  else wait_process(dir_out);
}

void ProcesCreare(struct dirent *entry, char* path, char *name,struct stat *st_file, struct stat *st_leg, char *dir_out)
{
 
  int PID = fork();

  if (PID < 0)
    {
      perror("Eroare fork\n");
      exit(-1);
    }
 
  //verificari
  if (PID == 0)
    {
      //director
      if(S_ISDIR(st_file->st_mode)) director(name, st_file, dir_out);  
     
      else
{
 //legatura sim
 if(entry->d_type == DT_LNK) legatura(path, name, st_file, st_leg, dir_out);
 
 //fisier reg
 if(entry->d_type != DT_LNK && (S_ISREG(st_file->st_mode) || (S_ISREG(st_file->st_mode) && strcmp(entry->d_name + strlen(entry->d_name) - 4, ".bmp")==0)))
   {
     if(strcmp(entry->d_name + strlen(entry->d_name) - 4, ".bmp")==0)
procesPixeli(name, path, dir_out);
     fisier(path, name, st_file, dir_out);
   }
}
     
      exit(-1);
    }
 
  else wait_process(dir_out);
}

//citirea directorului
void citireD(char *dir_in, char *dir_out)
{
 
  //deschidem directorul si verificam daca e null
  DIR *dir;
  dir=opendir(dir_in);
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
          struct stat legatura;
          char path[300]={};
 
          sprintf(path,"%s/%s",dir_in,entry->d_name);
 
          if(stat(path,&file)==-1)
            {
              perror("eroare stat\n");
              exit(1);
            }
 
          if(lstat(path,&legatura)==-1)
            {
              perror("eroare stat legatura\n");
              exit(1);
            }
 
 //verificare director
          if(S_ISDIR(file.st_mode))
            {
              ProcesCreare(entry,path, entry->d_name, &file, &legatura, dir_out);
              citireD(path, dir_out);
            }
          else
              ProcesCreare(entry,path, entry->d_name, &file, &legatura, dir_out);
        }
    }
 
  //verificare inchidere director
  if(closedir(dir)==-1)
    {
      perror("eroare inchidere dir");
      exit(1);
    }
}

int main(int argc,char*argv[])
{
  if(argc!=4)
    {
      perror("nr de argumente invalid");
      exit(1);
    }
 
  citireD(argv[1], argv[2], argv[3]);
 
  return 0;
}