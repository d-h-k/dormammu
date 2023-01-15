#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include "cJSON.h"
#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE 1
#ifdef _WIN32
#undef snprintf
#define snprintf _snprintf
#endif
#define _status_item 28

typedef struct tm tm;
int old_min = 0;
struct _dev *dev_shm;
void *dev_shared_memory = (void *)0;
#define DEV_SHM_KEY 3131
struct _dev
{
    char polling_data[2000];
    int polling_flag;
    char ack_data[2000];
    int ack_flag;
};
struct _timer *timer_shm;
void *timer_shared_memory = (void *)0;
#define TIMER_SHM_KEY 2121
struct _timer
{
    int yyyy;
    int mm;
    int md;
    int hour;
    int min;
    int sec;
};
struct _set_Value *set_shm;
void *set_shared_memory = (void *)0;
#define SET_SHM_KEY 1111
struct _set_Value
{
    int sensor1;
    int sensor2;
    int sensor3;
    int sensor4;
    int sensor5;
};
/* Post JSON data to a server.
name and value must be UTF-8 strings.
Returns TRUE on success, FALSE on failure.
*/
// test용
//============================================================== == == == == ==
char *key_name[] = {"systemDate", "systemId", "sensor1",
                    "sensor2", "sensor3", "sensor4", "sensor5"};
struct MemoryStruct
{
    char *memory;
    size_t size;
};
static size_t WriteMemoryCallback(void *contents, size_t size,
                                  size_t nmemb, void *userp)
{
    size_t realsize = size = nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct
                                    *)userp;
    mem->memory = realloc(mem->memory, mem->size +
                                           realsize + 1);
    if (mem->memory == NULL)
    {
        printf("memory error\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}
int PostJSON(int item)
{
    int retcode = FALSE;
    cJSON *root = NULL;
    char *json = NULL;
    CURL *curl = NULL;
    CURLcode res = CURLE_FAILED_INIT;
    char errbuf[CURL_ERROR_SIZE] = {
        0,
    };
    struct curl_slist *headers = NULL;
    char agent[1024] = {
        0,
    };
    int i = 0;
    char timer[18];
    char *p;
    char buff[400];
    long statLong;
    FILE *fp;
    fp = fopen("/home/ubuntu/log/post.txt", "a+");
    sprintf(timer, "%04d%02d%02d %02d:%02d:%02d", timer_shm->yyyy, timer_shm->mm, timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec);
    printf("%s\n", timer);
    root = cJSON_CreateObject();
    if (!root)
    {
        fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d]Error: cJSON_CreateObject failed.\n ",
                timer_shm->yyyy,
                timer_shm->mm,
                timer_shm->md, timer_shm->hour, timer_shm->min,
                timer_shm->sec);
        goto cleanup;
    }
    cJSON_AddStringToObject(root, key_name[i++], timer);
    memset(buff, 0x00, sizeof(buff));
    memcpy(buff, dev_shm->polling_data, strlen(dev_shm->polling_data));
    p = strtok(buff, ", ");
    while (p != NULL)
    {
        if (i < 7)
        {
            if ((strcmp(p, "ex") != 0)) // ETX 테스트용
            {
                cJSON_AddStringToObject(root,
                                        key_name[i++], p);
            }
        }
        p = strtok(NULL, ", ");
    }
    json = cJSON_PrintUnformatted(root);
    if (!json)
    {
        fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d]Error: cJSON_PrintUnformatted failed.\n ", timer_shm->yyyy,
                timer_shm->mm,
                timer_shm->md, timer_shm->hour, timer_shm->min,
                timer_shm->sec);
        goto cleanup;
    }
    curl = curl_easy_init();
    if (!curl)
    {
        fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d]Error: curl_easy_init failed.\n", timer_shm->yyyy, timer_shm->mm, timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec);
        goto cleanup;
    }

    // debug
    // printf("json = %s\n", json);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "tset.crt");
    snprintf(agent, sizeof(agent), "libcurl/%s",
             curl_version_info(CURLVERSION_NOW)->version);
    agent[sizeof(agent) - 1] = 0;
    curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);
    headers = curl_slist_append(headers, "Expect:");
    headers = curl_slist_append(headers, "Content-Type:application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
    /* This is a test server, it fakes a reply as if the
    json object were
    created */
    curl_easy_setopt(curl, CURLOPT_URL,
                     "https://xxxxxxxxx/xxx.com/xx/xxxxxx");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        size_t len = strlen(errbuf);
        fprintf(fp, "\nlibcurl: (%d)\n ", res);
        if (len)
            fprintf(fp, "[%04d-%02d-%02d%02d:%02d:%02d]%s%s\n", timer_shm->yyyy, timer_shm->mm,
                    timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec,
                    errbuf, ((errbuf[len - 1] != '\n') ? "\n" : ""));
        fprintf(fp, "[%04d-%02d-%02d%02d:%02d:%02d]%s\n\n", timer_shm->yyyy, timer_shm->mm, timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec, curl_easy_strerror(res));
        // goto cleanup;
    }
    else
    {
        if (CURLE_OK == curl_easy_getinfo(curl,
                                          CURLINFO_HTTP_CODE, &statLong))
        {
            // printf("[%04d-%02d-%02d%02d:%02d:%02d] RCV_HTTP_CODE : %ld\n", timer_shm->yyyy,timer_shm->mm, timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec,statLong);
            fprintf(fp, "[%04d-%02d-%02d%02d:%02d:%02d] RCV_HTTP_CODE : %ld\n", timer_shm->yyyy, timer_shm->mm, timer_shm->md, timer_shm->hour, timer_shm->min, timer_shm->sec, statLong);
            dev_shm->polling_flag = 0;
        }
    }
    retcode = TRUE;
cleanup:
    fclose(fp);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    cJSON_Delete(root);
    free(json);
    return retcode;
}
void timer_count()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    timer_shm->yyyy = tm.tm_year + 1900;
    timer_shm->mm = tm.tm_mon + 1;
    timer_shm->md = tm.tm_mday;
    timer_shm->hour = tm.tm_hour;
    timer_shm->min = tm.tm_min;
    timer_shm->sec = tm.tm_sec;
}

int main(int argc, char *argv[])
{
    int dev_shmid;
    int timer_shmid;
    int set_shmid;

    dev_shmid = shmget((key_t)DEV_SHM_KEY, sizeof(char), 0666 | IPC_CREAT);
    if (dev_shmid == -1)
    {
        perror("dev shmget fail");
        exit(0);
    }
    dev_shared_memory = shmat(dev_shmid, (void *)0, 0);
    if (dev_shared_memory == (void *)-1)
    {
        perror("dev shmat fail");
        exit(0);
    }
    dev_shm = (struct _dev *)dev_shared_memory;
    //============================================================== == == == == == == == == == == == == == =
    //============================================================== == == == == == == == == == == == == == =
    timer_shmid = shmget((key_t)TIMER_SHM_KEY,
                         sizeof(char), 0666 | IPC_CREAT);
    if (timer_shmid == -1)
    {
        perror("timer shmget fail");
        exit(0);
    }
    timer_shared_memory = shmat(timer_shmid, (void *)0,
                                0);
    if (timer_shared_memory == (void *)-1)
    {
        perror("timer shmat fail");
        exit(0);
    }
    timer_shm = (struct _timer *)timer_shared_memory;
    //============================================================== == == == == == == == == == == == == == =
    //============================================================== == == == == == == == == == == == == == =
    set_shmid = shmget((key_t)SET_SHM_KEY, sizeof(char),
                       0666 | IPC_CREAT);
    if (set_shmid == -1)
    {
        perror("set shmget fail");
        exit(0);
    }
    set_shared_memory = shmat(set_shmid, (void *)0, 0);
    if (set_shared_memory == (void *)-1)
    {
        perror("set shmat fail");
        exit(0);
    }
    set_shm = (struct _set_Value *)set_shared_memory;
    //============================================================== == == == == == == == == == == == == == =
    while (1)
    {
        sleep(1);
        timer_count();
        if (timer_shm->min != old_min)
        {
            old_min = timer_shm->min;
            if (curl_global_init(CURL_GLOBAL_ALL))
            {
                fprintf(stderr, "Fatal: The initialization of libcurl has failed.\n");
                return EXIT_FAILURE;
            }
            if (atexit(curl_global_cleanup))
            {
                fprintf(stderr, "Fatal: atexit failed to register curl_global_cleanup.\n");
                curl_global_cleanup();
                return EXIT_FAILURE;
            }
            if (dev_shm->polling_flag == 1)
            {
                if (!PostJSON(_status_item))
                {
                    fprintf(stderr, "Fatal: PostJSON failed.\n");
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}