#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <libelf.h>

/* from stevens. */
#define	MAXLINE		4096	/* max text line length */

#define ERR_DO_ERRNO 1
#define ERR_DO_ELF 2

static void
err_doit(int errnoflag, const char *fmt, va_list ap)
{
  int errno_save, n;
  char buf[MAXLINE];

  errno_save = errno;		/* value caller might want printed */
  vsnprintf(buf, sizeof(buf), fmt, ap);	/* this is safe */
  n = strlen(buf);
  if (errnoflag == ERR_DO_ERRNO)
    snprintf(buf+n, sizeof(buf)-n, ": %s", strerror(errno_save));
  else if (errnoflag == ERR_DO_ELF)
    snprintf(buf+n, sizeof(buf)-n, ": %s", elf_errmsg(elf_errno()));
  strcat(buf, "\n");

  fflush(stdout);         /* in case stdout and stderr are the same */
  fputs(buf, stderr);
  fflush(stderr);

  return;
}

static void
err_sys(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(ERR_DO_ERRNO, fmt, ap);
  va_end(ap);
  exit(1);
}

static void
err_elf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(ERR_DO_ELF, fmt, ap);
  va_end(ap);
  exit(1);
}

static void
err_quit(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, fmt, ap);
  va_end(ap);
  exit(1);
}

static ssize_t
readn(int fd, void *ptr, size_t n)
{
  unsigned char *cptr = ptr;
  size_t	nleft;
  ssize_t	nread;

  nleft = n;
  while (nleft > 0) {
    if ( (nread = read(fd, cptr, nleft)) < 0) {
      if (nleft == n)
        return -1;	/* error, nothing read */
      else
        break;		/* error, return amount read so far */
    } else if (nread == 0) {
        break;		/* EOF */
    }
    nleft -= nread;
    cptr  += nread;
  }
  return(n - nleft);
}

static ssize_t
writen(int fd, void *ptr, size_t n)
{
  unsigned char *cptr = ptr;
  size_t	nleft;
  ssize_t	nwritten;

  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, cptr, nleft)) < 0) {
      if (nleft == n)
        return -1;	/* error, nothing read */
      else
        break;		/* error, return amount read so far */
    } else if (nwritten == 0) {
        break;		/* EOF */
    }
    nleft -= nwritten;
    cptr  += nwritten;
  }
  return(n - nleft);
}

/* end stevens */

int
main(int argc, char *argv[])
{
  int binfd, rawfd;

  Elf *elf;
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;

  int i;

  static ssize_t text_start;
  static ssize_t text_size;

  unsigned char *text;

  if (argc != 3)
    err_quit("Usage: dump <foo>.bin <foo>.raw");

  if ( (binfd = open(argv[1], O_RDONLY, 0)) < 0)
    err_sys("Failed to open binary %s", argv[1]);

  if ( (rawfd = creat(argv[2], S_IRUSR | S_IWUSR)) < 0)
    err_sys("Failed to create raw %s", argv[2]);

  if (elf_version(EV_CURRENT) == EV_NONE)
    err_elf("Failed to set ELF version");
  if ( (elf = elf_begin(binfd, ELF_C_READ, NULL)) == NULL)
    err_elf("Failed to begin ELF reading");

  if ( (ehdr = elf64_getehdr(elf)) == NULL)
    err_elf("Failed to get eheader");
  if ( (phdr = elf64_getphdr(elf)) == NULL)
    err_elf("Failed to get pheader");

  for (i = 0; i < ehdr->e_phnum; i++)
    if (phdr[i].p_type  == PT_LOAD &&
        (phdr[i].p_flags == (PF_X|PF_R) ||
         phdr[i].p_flags == (PF_X|PF_R|PF_W)) )
      break;
  if (i == ehdr->e_phnum)
    err_quit("Failed to find text segment");

  text_start = phdr[i].p_offset;
  text_size = phdr[i].p_filesz;

  elf_end(elf);

  if (NULL == (text = malloc(text_size)))
    err_quit("Failed to allocate text buffer");

  if (-1 == lseek(binfd, text_start, SEEK_SET))
    err_sys("Failed to seek binary file to text segment");

  if ( text_size != readn(binfd, text, text_size) )
    err_quit("Failed to read text contents from binary file");

  if ( text_size != writen(rawfd, text, text_size) )
    err_quit("Failed to write text contents to raw file");

  if (-1 == close(binfd))
    err_sys("Failed to close binary file");
  if (-1 == close(rawfd))
    err_sys("Failed to close raw file");
}
