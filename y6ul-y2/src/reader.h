#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

int safe_asprintf(char **strp, const char *fmt, ...);
int safe_vasprintf(char **strp, const char *fmt, va_list ap);
void plog(const char *format, ...) ;
void pinfo(const char *format, ...) ;
void pflog(const char *format, ...);
void pfinfo(const char *format, ...);

#define MYD
#ifdef MYD
//MYD demo board
#define DEVICE "/dev/ttymxc1"
#define DEVICE_NAME "tmr:///dev/ttymxc1"
#else
//M28x demo board
#define DEVICE "/dev/ttySP0"
#define DEVICE_NAME "tmr:///dev/ttySP0"
#endif

#define DEBUG

#ifdef DEBUG
void pflog(const char *format, ...);
void pfinfo(const char *format, ...);
void plog(const char *format, ...);
void pinfo(const char *format, ...);
#define debug(fmt, args...) plog(fmt, ##args) 
#else
#define debug(fmt, args...) do{}while(0)
#endif

static pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * safe_asprintf();
 */
int safe_asprintf(char **strp, const char *fmt, ...) 
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = safe_vasprintf(strp, fmt, ap);
    va_end(ap);

    return retval;
}

/*
 * safe_vasprintf();
 */
int safe_vasprintf(char **strp, const char *fmt, va_list ap) 
{
    int retval;

    retval = vasprintf(strp, fmt, ap);
    if (retval == -1) 
    {
        printf("Failed to vasprintf: %s.  Bailing out\n", strerror(errno));
        return 1;
    }
    return retval;
}

/*
 * plog();
 */
void plog(const char *format, ...) 
{
    va_list vlist;
    char *fmt = NULL;

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);


    time_t timep;
    struct tm *ptm = NULL;
    time(&timep);
    ptm = localtime(&timep);
//    printf("%s[plog]%s", asctime(ptm), fmt);

   	printf("[%04d-%02d-%02d %s] %s\n", 
				ptm->tm_year + 1900, 
				ptm->tm_mon + 1,
				ptm->tm_mday, 
				__TIME__,
				fmt);

    free(fmt);
}

/*
 * pinfo();
 */
void pinfo(const char *format, ...) 
{
    va_list vlist;
    char *fmt = NULL;


    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        return;
    }

    printf("%s", fmt);

    free(fmt);
}

/*
 * pflog();
 */
void pflog(const char *format, ...) 
{

    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    // Open debug info output file.
    if (!(fp = fopen("log.txt", "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    time_t timep;
    struct tm *ptm = NULL;
    time(&timep);
    ptm = localtime(&timep);
    fprintf(fp, "[%04d-%02d-%02d-%02d-%02d-%02d] %s", 
            ptm->tm_year + 1900, 
            ptm->tm_mon + 1,
            ptm->tm_mday, 
            ptm->tm_hour, 
            ptm->tm_min, 
            ptm->tm_sec, 
            fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}

/*
 * pfinfo();
 */
void pfinfo(const char *format, ...) 
{
    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    // Open debug info output file.
    if (!(fp = fopen("log.txt", "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    fprintf(fp, "%s", fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}


