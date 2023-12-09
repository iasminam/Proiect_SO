#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/wait.h>

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

void statistici(char *nume_fis, char *abc, char *dir_out)
{
  // fisier statistica

  char nume[300] = {};

  //deschide dir scriere si verifica daca e null
  DIR *dirOut = NULL;
  dirOut = opendir(dir_out);

  if (dirOut == NULL)
    {
      perror("eroare deschidere director scriere\n");
      exit(1);
    }

  // Construieste calea pentru fisierul de statistici
  strcpy(nume, dir_out);
  strcat(nume, "/");
  strcat(nume, nume_fis);
  strcat(nume, "_statistica.txt");

  char *output = nume;  //salveaza calea intr un pointer
  struct stat st_output;

  // Deschide fisierul de statistici pentru scriere, creandu-l daca nu exista
  int f1;
  f1 = open(output, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IWGRP | S_IWOTH | S_IRUSR | S_IRGRP | S_IROTH);

  if (f1 == -1)
    {
      perror("eroare deschidere statistici\n");
      exit(1);
    }

  // Obtine info despre fiserul de statistici
  if (stat(output, &st_output) == -1)
    {
      perror("eroare stat (statistici)\n");
      exit(1);
    }

  // Scrie info statistice in fisier
  if (write(f1, abc, strlen(abc)) == -1)
    {
      perror("eroare scriere\n");
      exit(1);
    }

  // inchide fisierul de statistici
  if (close(f1) == -1)
    {
      perror("eroare inchidere statistici\n");
      exit(1);
    }

  //inchide dir
  if (closedir(dirOut) == -1)
    {
      perror("eroare inchidere director");
      exit(1);
    }
}

void fisibmp(char *path, char *name, struct stat *st_file, char *dir_out, int *nr_linii)
{
  char abc[3000];

  // nume pt fisier
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp")) strcpy(abc, "nume fisier bmp: ");
  *nr_linii = 10;
  strcat(abc, name);
  strcat(abc, "\n");

  // dimensiuni fisier
  char cont[100];
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp"))
    {
      int fd;

      // verificare deschidere
      fd = open(path, O_RDONLY);
      if (fd == -1)
	{
	  perror("eroare deschidere");
	  exit(1);
	}

      // verificare inaltime
      int inaltime;
      lseek(fd, 18, SEEK_SET);
      if (read(fd, &inaltime, sizeof(int)) != sizeof(int))
	{
	  perror("eroare citire");
	  if (close(fd) == -1)
	    {
	      perror("eroare inchidere");
	      exit(1);
	    }
	  exit(1);
	}

      // afisare inaltime
      strcat(abc, "inaltime: ");
      sprintf(cont, "%d\n", inaltime);
      strcat(abc, cont);

      // verificare lungime
      int lungime;
      if (read(fd, &lungime, sizeof(int)) != sizeof(int))
	{
	  perror("eroare citire");
	  if (close(fd) == -1)
	    {
	      perror("eroare inchidere");
	      exit(1);
	    }
	  exit(1);
	}

      // afisare lungime
      strcat(abc, "lungime: ");
      sprintf(cont, "%d\n", lungime);
      strcat(abc, cont);

      // verificare inchidere
      if (close(fd) == -1)
	{
	  perror("eroare inchidere");
	  exit(1);
	}
    }

  // dim fisier bmp
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp"))
    {
      strcat(abc, "dimensiune: ");
      sprintf(cont, "%ld\n", st_file->st_size);
      strcat(abc, cont);
    }

  // ID fisier, director
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp"))
    {
      strcat(abc, "identificatorul utilizatorului: ");
      sprintf(cont, "%d\n", st_file->st_uid);
      strcat(abc, cont);
    }

  // timp fisier
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp"))
    {
      strcat(abc, "timpul ultimei modificari: ");
      strcat(abc, ctime(&st_file->st_mtime));
    }

  // contorul de legaturi fisier
  if (S_ISREG(st_file->st_mode) && strstr(path, ".bmp"))
    {
      strcat(abc, "contorul de legaturi: ");
      sprintf(cont, "%ld\n", st_file->st_nlink);
      strcat(abc, cont);
    }

  // drepturi de acces
  drepturi(abc, st_file->st_mode);

  // statistici
  statistici(name, abc, dir_out);
}

void fisireg(char *path, char *name, struct stat *st_file, char *dir_out, int *nr_linii)
{
  char abc[3000];
  char cont[100];
  // nume pt fisier
  if (S_ISREG(st_file->st_mode)) strcpy(abc, "nume fisier reg: ");
  *nr_linii = 8;
  strcat(abc, name);
  strcat(abc, "\n");

  // dimensiuni fisier
  if (S_ISREG(st_file->st_mode))
    {
      strcat(abc, "dimensiune: ");
      sprintf(cont, "%ld\n", st_file->st_size);
      strcat(abc, cont);
    }

  // ID fisier, director
  if (S_ISREG(st_file->st_mode))
    {
      strcat(abc, "identificatorul utilizatorului: ");
      sprintf(cont, "%d\n", st_file->st_uid);
      strcat(abc, cont);
    }

  // timp fisier
  if (S_ISREG(st_file->st_mode))
    {
      strcat(abc, "timpul ultimei modificari: ");
      strcat(abc, ctime(&st_file->st_mtime));
    }

  // contorul de legaturi fisier
  if (S_ISREG(st_file->st_mode))
    {
      strcat(abc, "contorul de legaturi: ");
      sprintf(cont, "%ld\n", st_file->st_nlink);
      strcat(abc, cont);
    }

  // drepturi de acces
  drepturi(abc, st_file->st_mode);

  // statistici
  statistici(name, abc, dir_out);
}

void legatura(char *path, char *name, struct stat *st_file, struct stat *st_leg, char *dir_out, int *nr_linii)
{
  char abc[3000];
  char cont[100];
  
  struct stat st_link;
  if (lstat(path, &st_link) == -1)
    {
      perror("eroare stat");
      exit(1);
    }
  
  // nume pt legatura
  if (S_ISLNK(st_link.st_mode)) strcpy(abc, "nume legatura: ");
  *nr_linii = 6;
  strcat(abc, name);
  strcat(abc, "\n");

  // legatura - dimensiune
  if (S_ISLNK(st_link.st_mode))
    {
      sprintf(cont, "dimensiune legatura: %ld\n", st_link.st_size); // afisarea dimensiunii legaturii simbolice
      strcat(abc, cont);
    }

  // drepturi de acces
  drepturi(abc, st_leg->st_mode);

  // statistici
  statistici(name, abc, dir_out);
}

void director(char *name, struct stat *st_file, char *dir_out, int *nr_linii)
{
  char abc[3000];
  char cont[100];
  // nume pt director
  if (S_ISDIR(st_file->st_mode)) strcpy(abc, "nume director: ");
  *nr_linii = 5;
  strcat(abc, name);
  strcat(abc, "\n");

  // ID  director
  strcat(abc, "identificatorul utilizatorului: ");
  sprintf(cont, "%d\n", st_file->st_uid);
  strcat(abc, cont);

  // drepturi de acces
  drepturi(abc, st_file->st_mode);

  // statistici
  statistici(name, abc, dir_out);
}

void wait_process(char *dir_out)
{
  int PID;
  int index;

  //Asteapta terminarea proces copil
  while ((PID = wait(&index)) != -1)
    {
      //Verifica daca procesul s-a incheiat normal
      if (WIFEXITED(index))
	{
	  //Afiseaza informatii despre proces
	  printf("S-a incheiat procesul cu pid-ul %d si codul %d\n", PID, WEXITSTATUS(index));
	  printf("Nr linii scrise: %d.\n", WEXITSTATUS(index));
	}
      else
	//Afiseaza un mesaj in cazul in care procesul nu s-a incheiat corect
	printf("Procesul cu PID %d nu s-a terminat corect\n", PID);
    }
}

int Ffork()
{
  //Creeaza un proces copil si obtine PID-ul
  int PID = fork();

  // Verifica daca fork a fost realizat cu succes
  if (PID < 0)
    {
      // in caz de eroare, afiseaza un mesaj de eroare utilizand perror
      perror("Eroare fork\n");
      exit(-1);
    }
  // Returneaza PID-ul procesului copil catre apelantul functiei
  return PID;
}

void procesPixeli(char *name, char *path, char *dir_out)
{
  //creeaza un proces copil
  int PID = Ffork();

  char nume[300];
  int lungPH;
  int inaltPH;

  if (PID == 0)   //proces copil
    {
      // Construieste calea completa pentru imagine
      strcpy(nume, path);
      strcat(nume, "/");
      strcat(nume, name);

      char *fis = path;

      int photo;
      photo = open(fis, O_RDWR);

      if (photo == -1)
	{
	  perror("eroare deschidere poza\n");
	  exit(1);
	}

      // verificare inaltime
      lseek(photo, 18, SEEK_SET);
      if (read(photo, &inaltPH, sizeof(int)) != sizeof(int))
	{
	  perror("eroare citire");
	  if (close(photo) == -1)
	    {
	      perror("eroare inchidere");
	      exit(1);
	    }
	  exit(1);
	}

      // verificare lungime
      if (read(photo, &lungPH, sizeof(int)) != sizeof(int))
	{
	  perror("eroare citire");
	  if (close(photo) == -1)
	    {
	      perror("eroare inchidere");
	      exit(1);
	    }
	  exit(1);
	}

      int PIX = lungPH * inaltPH;
      char InfoHeader[54];
      unsigned char P_color[3];
      unsigned char P_gri;

      //citirea header-ului imaginii
      if (read(photo, InfoHeader, sizeof(InfoHeader)) != sizeof(InfoHeader))
	{
	  perror("eroare citire header\n");
	  exit(1);
	}

      //proceseaza fiecare pixel si schimba in gri
      for (int i = 0; i < PIX; i++)
	{

	  if (read(photo, P_color, sizeof(P_color)) != sizeof(P_color))
	    {
	      perror("Pixeli - eroare citire\n");
	      exit(1);
	    }

	  //formula modificare
	  P_gri = 0.299 * P_color[0] + 0.587 * P_color[1] + 0.114 * P_color[2];

	  // Deplasare inapoi cu 3 octeti pentru a rescrie pixelul in tonuri de gri
	  lseek(photo, -3, SEEK_CUR);

	  // Scrie valoarea pixelului in tonuri de gri in locul pixelului original color
	  write(photo, &P_gri, sizeof(P_gri));
	  write(photo, &P_gri, sizeof(P_gri));
	  write(photo, &P_gri, sizeof(P_gri));
	}

      //inchide fisierul de imagine
      if (close(photo) == -1)
	{
	  perror("error inchidere poza");
	  exit(1);
	}

      exit(-1);      // Iesirea din procesul copil cu un cod de eroare (-1)
    }
}

// citirea directorului
void citireD(char *dir_in, char *dir_out, char *c)
{

  // deschidem directorul si verificam daca e null
  DIR *dir;
  dir = opendir(dir_in);
  if (dir == NULL)
    {
      perror("eroare deschidere dir");
      exit(1);
    }

  int sum = 0;
  int numar = 0;
  int nr_linii = 0;
  struct dirent *entry = NULL;

  while ((entry = readdir(dir)) != NULL)
    {
      //Verifica daca numele intrarii curente nu este "." (directorul curent) si nu este ".." (directorul parinte)
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
	{
	  struct stat file;
	  struct stat legaturi;
	  char path[300] = {};
	  char name[300];
	  sprintf(path, "%s/%s", dir_in, entry->d_name);
	  strcpy(name, entry->d_name);

	  // Obtine info despre fisier si legaturi simbolice
	  if (stat(path, &file) == -1)
	    {
	      perror("eroare stat\n");
	      exit(1);
	    }

	  if (lstat(path, &legaturi) == -1)
	    {
	      perror("eroare stat legatura\n");
	      exit(1);
	    }

	  //Proceseaza legatura simbolica intr-un proces copil
	  if (S_ISLNK(legaturi.st_mode))
	    {
	      int PID = Ffork();

	      if (PID == 0)
		{
		  legatura(path, name, &file, &legaturi, dir_out, &nr_linii);
		  exit(nr_linii);
		}
	    }
	  else
	    {
	      //Proceseaza directorul intr-un proces copil
	      if (S_ISDIR(file.st_mode))
		{
		  int PID = Ffork();

		  if (PID == 0)
		    {
		      director(name, &file, dir_out, &nr_linii);
		      exit(nr_linii);
		    }
		}
	      else
		{
		  //Proceseaza fisierele bmp intr-un proces copil
		  if (S_ISREG(file.st_mode) && strstr(path, ".bmp"))
		    {
		      int PID = Ffork();

		      if (PID == 0)
			{
			  fisibmp(path, name, &file, dir_out, &nr_linii);
			  exit(nr_linii);
			}
		    }

		  // tonuri de gri
		  if (S_ISREG(file.st_mode) && strstr(path, ".bmp"))
		    {
		      procesPixeli(name, path, dir_out);
		    }

		  //Proceseaza fisierele regulate intr-un proces copil
		  if (S_ISREG(file.st_mode) && !(strstr(path, ".bmp")))
		    {
		      int pfd[2];
		      if (pipe(pfd) == -1)
			{
			  perror("error pipe");
			  exit(-1);
			}

		      int PID = Ffork();

		      if (PID == 0)
			{
			  // pipe 1
			  close(pfd[1]);
			  close(pfd[0]);
			  
			  char *fisi = path;
			  int fis;

			  fis = open(fisi, O_RDONLY);
			  if (fis == -1)
			    {
			      perror("error open fis cit linii");
			      exit(1);
			    }

			  fisireg(path, name, &file, dir_out, &nr_linii);

			  if (close(fis) == -1)
			    {
			      perror("error close fis pt pipe 1");
			      exit(1);
			    }

			  exit(nr_linii);
			}

		      //Proceseaza scriptul bash intr-un alt proces copil
		      int pfd2[2];

		      //Creeaza un pipe si verifica daca a fost creat cu succes.
		      if (pipe(pfd2) < 0)
			{
			  perror("error pipe");
			  exit(-1);
			}

		      int PID2 = Ffork();

		      if (PID2 == 0)
			{
			  // inchide scriere in procesul copil a pipe 1 si citirea pipe 2
			  close(pfd[1]);
			  close(pfd2[0]);

			  dup2(pfd[0], 0); // Redirectioneaza intrarea standard (0) sa citeasca din primul pipe
			  close(pfd[0]);  //inchide capatul de citire al primului pipe in procesul copil.

			  dup2(pfd2[1], 1); // Redirectioneaza iesirea standard (1) sa scrie in al doilea pipe
			  close(pfd2[1]);  //inchide capatul de scriere al celui de-al doilea pipe in procesul copil.

			  // Executa scriptul Bash "exA.sh" cu argumentul "c"
			  execlp("bash", "bash", "exA.sh", c, NULL);
			  perror("execlp\n"); //Afiseaza un mesaj de eroare daca executia scriptului Bash a esuat.
			  exit(-1);
			}

		      //inchidere pipe
		      close(pfd[0]);
		      close(pfd[1]);

		      // inchide capatul de scriere al pipe-ului pentru al doilea pipe
		      close(pfd2[1]);

		      // pipe 2
		      //Citeste rezultatele din stream si aduna la suma
		      FILE *str;
		      str = fdopen(pfd2[0], "r"); //deschide un stream (FILE *) pentru capatul de citire
		      while ((fscanf(str, "%d", &numar)) != EOF)
			{
			  //adauga la suma nr citit
			  sum = sum + numar;
			  printf("%s - %d\n", entry->d_name, numar);
			}
		      close(pfd2[0]);  //inchide capatul de citire al celui de-al doilea pipe
		    }
		}
	    }
	}
    }

  //Asteapta ca toate procesele copil sa se incheie inainte de a continua
  wait_process(dir_out);

  //afiseaza rezultatul final
  printf("Au fost identificate in total %d propozitii corecte care contin caracterul %s\n", sum, c);

  //inchide dir
  if (closedir(dir) == -1)
    {
      perror("error close director");
      exit(1);
    }
}

int main(int argc, char *argv[])
{
  // Verifica numarul corect de argumente de la linia de comanda
  if (argc != 4)
    {
      perror("nr de argumente invalid");
      exit(1);
    }

  // Apel functie pentru citirea si procesarea directorului
  citireD(argv[1], argv[2], argv[3]);

  // Returneaza 0 pentru a indica ca programul s-a incheiat cu succes
  return 0;
}
