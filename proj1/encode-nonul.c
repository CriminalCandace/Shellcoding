#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

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
  int fd;
  struct stat statbuf;
  ssize_t len;
  ssize_t words;
  unsigned char *bufA, *bufB;
  ssize_t i;

  if (argc != 2)
    err_quit("Usage: check [-u] <foo>.raw");

  if ( (fd = open(argv[1], O_RDONLY, 0)) < 0)
    err_sys("Failed to open %s", argv[1]);

  if (-1 == fstat(fd, &statbuf))
    err_sys("Failed to stat %s", argv[1]);

  len = statbuf.st_size;

  if (len == 0)
    err_quit("File is empty");
  if (len < 0 || len > SSIZE_MAX-15)
    err_quit("Failed to do math");

  words = (len+7) / 8;

  if ( (bufA = malloc((words+1)*8)) == NULL)
    err_quit("Failed to allocate shellcode buffer A");

  if ( (bufB = malloc((words+1)*8)) == NULL)
    err_quit("Failed to allocate shellcode buffer B");

  memset(bufA, '\0', (words+1)*8);
  memset(bufB, '\0', (words+1)*8);

  if ( len != readn(fd, bufA, len) )
    err_quit("Failed to read shellcode contents from file");

  if (-1 == close(fd))
    err_sys("Failed to close file");

  // last word in bufA is word count
  bufA[words*8]   = (uint64_t)words         & 0xff;
  bufA[words*8+1] = ((uint64_t)words >> 8)  & 0xff;
  bufA[words*8+2] = ((uint64_t)words >> 16) & 0xff;
  bufA[words*8+3] = ((uint64_t)words >> 24) & 0xff;
  bufA[words*8+4] = ((uint64_t)words >> 32) & 0xff;
  bufA[words*8+5] = ((uint64_t)words >> 40) & 0xff;
  bufA[words*8+6] = ((uint64_t)words >> 48) & 0xff;
  bufA[words*8+7] = ((uint64_t)words >> 56) & 0xff;

  // actually do the encoding
  for (i = 0; i < (words+1)*8; i++)
    {
      if (bufA[i] == 0xff)
        {
          bufB[i] = 0xf0;
          bufA[i] ^= 0xf0;
        }
      else
        {
          bufB[i] = 0xff;
          bufA[i] ^= 0xff;
        }
    }

  printf("\t.dword ");

  for (i = words*8; i >= 0; i -= 8)
    {
      printf("0x%02x%02x%02x%02x%02x%02x%02x%02x, ",
             bufA[i+7], bufA[i+6], bufA[i+5], bufA[i+4], 
             bufA[i+3], bufA[i+2], bufA[i+1], bufA[i]);
      printf("0x%02x%02x%02x%02x%02x%02x%02x%02x",
             bufB[i+7], bufB[i+6], bufB[i+5], bufB[i+4], 
             bufB[i+3], bufB[i+2], bufB[i+1], bufB[i]);
      if (i > 0)
        printf(", ");
    }
  printf("\n");

  return 0;
}
