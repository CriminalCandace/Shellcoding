#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "utf8proc.h"

/* from stevens. */
#define	MAXLINE		4096	/* max text line length */

#define ERR_DO_ERRNO 1

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

/* end stevens */

int
main(int argc, char *argv[])
{
  int check_utf8;
  char *filename;
  
  int fd;
  struct stat statbuf;

  ssize_t len;
  unsigned char *shellcode;

  ssize_t i;
  ssize_t width;
  int32_t codepoint;
  const utf8proc_property_t *property;

  if (argc == 3 && 0 == strcmp(argv[1], "-u"))
    {
      check_utf8 = 1;
      filename = argv[2];
    }
  else if (argc == 2)
    {
      check_utf8 = 0;
      filename = argv[1];
    }
  else
    {
      err_quit("Usage: check [-u] <foo>.raw");
    }

  if ( (fd = open(filename, O_RDONLY, 0)) < 0)
    err_sys("Failed to open %s", filename);

  if (-1 == fstat(fd, &statbuf))
    err_sys("Failed to stat %s", filename);

  len = statbuf.st_size;

  if ( (shellcode = malloc(len)) == NULL)
    err_quit("Failed to allocate shellcode buffer");

  if ( len != readn(fd, shellcode, len) )
    err_quit("Failed to read shellcode contents from file");

  if (-1 == close(fd))
    err_sys("Failed to close file");


  for (i = 0; i < len; i++)
    {
      if (shellcode[i] == '\0')
        printf("NUL byte at index %ld!\n", i);
    }

  if (!check_utf8)
    exit(EXIT_SUCCESS);

  for (i = 0; i < len; i += width)
    {
      width = utf8proc_iterate(shellcode+i, len-i, &codepoint);
      if ((width < 0) || !utf8proc_codepoint_valid(codepoint))
        {
          printf("Invalid UTF-8 codepoint at index %ld!", i);
          printf(" (rest of file not parsed)\n");
          break;
        }
      if ( (property = utf8proc_get_property(codepoint)) == NULL)
        {
          err_quit("Failed to look up codepoint properties");
        }
      if (property->category == UTF8PROC_CATEGORY_CN)
        {
          printf("Unassigned UTF-8 codepoint %x at index %ld!\n",
                 codepoint, i);
        }
    }

  exit(EXIT_SUCCESS);
}
