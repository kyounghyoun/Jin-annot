#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<unistd.h>
       extern char *optarg;
       extern int optind, opterr, optopt;

typedef struct fqe
{
  int length;
  int entry_size;
  char *id;
  char *entry;
} fq_entry;

fq_entry*
get_fqentry(FILE* in)
{
  char*str;
  char *p;
  fq_entry* retv;
  int charindex;
  int curchar;
  const int bufsize = 4000;
  const int maxidbuf = 200;
  str = malloc(bufsize);
  p = fgets(str, bufsize, in);
  if(p == NULL){
    free(str);
    return NULL;
  }
  retv = malloc(sizeof(fq_entry));
  retv -> entry = str;
  retv -> id = malloc(maxidbuf);
  retv -> length = 0;
  charindex = 1;
  while(str[charindex] && !isspace(str[charindex]) && charindex < maxidbuf){
    retv->id[charindex-1]=str[charindex];
    charindex +=1;
  }
  retv->id[charindex-1]='\0';
  if(str[charindex] == '\0'){
    exit(EXIT_FAILURE);
  }
  while(str[charindex] != '\n'){
    charindex += 1;
  }
  while((curchar=fgetc(in))){
    charindex += 1;
    str[charindex] = curchar;
    if(curchar == '\n'){
      break;
    }
    retv->length += 1;
  }
  while((curchar=fgetc(in))){
    charindex += 1;
    str[charindex] = curchar;
    if(curchar == '\n'){
      break;
    }
  }
  while((curchar=fgetc(in))){
    charindex += 1;
    str[charindex] = curchar;
    if(curchar == '\n'){
      break;
    }
  }
  charindex += 1;
  retv->entry_size = charindex;
  str[charindex]='\0';
  return retv;
}
void free_fqentry(fq_entry* entry)
{
  free(entry->id);
  free(entry->entry);
  free(entry);
}

void usage()
{
  fputs("cutadapt-pe fq1 fq2 \n", stderr);
}
int
main(int argc, char**argv)
{
  FILE*file1;
  FILE*file2;
  fq_entry* fq1;
  fq_entry* fq2;
  int optchar;
  int threshold = 70;

  pid_t pid1;
  pid_t pid2;
  int pipe_r1[2];
  int pipe_r2[2];

  while((optchar = getopt(argc,argv,"t:"))!= -1){
    switch(optchar){
      case 't':
        threshold = strtol(optarg, NULL, 0);
        break;
    }
  }
  if(argc !=3){
    usage();
    exit(EXIT_FAILURE);
  }
/* Instead of simply opening the arguments, 
 * we will process the input through 
 * ~/cutadapt-1.2.1/bin/cutadapt -a CTGTCTCTTATACACATCT -a AGATGTGTATAAGAGACAG -a GATCGGAAGAGCACACGTCTGAACTCCAGTCAC \
 *  #{infastq2} 
 */
  
  if(pipe(pipe_r1)){
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }
  if(pipe(pipe_r2)){
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }

  pid1 = fork();
  if(pid1 == (pid_t) 0) {
    int fd;
/* This is the child process. Close other end first. */
    close(pipe_r1[0]);
    fd = dup2(pipe_r1[1],1);
    if(fd == -1) perror("dup2 1");
    execlp("cutadapt", "cutadapt0", 
         "-a", "GATCGGAAGAGCACACGTCTGAACTCCAGTCAC", 
         argv[1], (char *)NULL);
        return EXIT_FAILURE;
     //dup and exec
   }else if(pid1 < (pid_t) 0){
      /* The fork failed. */
     fprintf (stderr, "Fork failed.\n");
     return EXIT_FAILURE;
   }else{
  /* This is the parent process. Close other end first. */
     close (pipe_r1[1]);
     pid2 = fork(); /* second fork */
     if(pid2 == (pid_t) 0) {
       int fd2;
       close(pipe_r2[0]);
       fd2 = dup2(pipe_r2[1],1);
       if(fd2 == -1) perror("dup2 2");
       execlp("cutadapt", "cutadapt1", 
         "-a", "GATCGGAAGAGCACACGTCTGAACTCCAGTCAC", 
         argv[2], (char *)NULL);
        return EXIT_FAILURE;
     //dup and exec
     } else if (pid2 < (pid_t) 0) {
      /* The fork failed. */
       fprintf (stderr, "Fork failed.\n");
        return EXIT_FAILURE;
    } else {
    /* This is the parent process. Close other end first. */
      close (pipe_r2[1]);
      file1 = fdopen(pipe_r1[0], "r");
      file2 = fdopen(pipe_r2[0], "r");
      while((fq1 = get_fqentry(file1))){
        fq2 = get_fqentry(file2);
        if((fq1->length >= threshold) &&
           (fq2->length >= threshold)){
          fwrite(fq1->entry, fq1->entry_size, 1, stdout);
          fwrite(fq2->entry, fq2->entry_size, 1, stdout);
        }
        free_fqentry(fq1);
        free_fqentry(fq2);
      }
      return EXIT_SUCCESS;
    }
  }
}

