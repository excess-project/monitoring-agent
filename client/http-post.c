/** \file http-post.c

\author Anthony Sulistio
\date April 2013
Copyright 2014 University of Stuttgart 
*/

#include "http-post.h"
#include "monitoring-excess.h"

/* ptr - curl output
   stream - data or string to be received */
size_t get_stream_data(void *ptr, size_t size, size_t count, void *stream)
{
    size_t total = size * count;

    printf("\n--> Sent Output: %s -- len = %d", (char*) ptr, (int) total);
    memcpy(stream, ptr, total);
    return total;
}

int send_monitoring_data(char *URL, char *data)
{
    CURLcode res;
    int result = SEND_SUCCESS;


    /* perform some error checking */
    if (URL == NULL || strlen(URL) == 0) 
    {
        fprintf(stderr, "send_monitoring_data(): Error - the given url is empty.\n");
        return SEND_FAILED;
    }

    /*****  // Not working well for query the results
    if (data == NULL || strlen(data) == 0) 
    {
        fprintf(stderr, "send_monitoring_data(): Error - the monitoring data is empty.\n");
        return SEND_FAILED;
    }
    ******/

    /* init libcurl if not already done */

    if (curl_ == NULL) {
        init_curl();
    }

    printf("curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
    printf("Msg = %s -- len: %d\n", data, (int) strlen(data));

    curl_easy_setopt(curl_, CURLOPT_URL, URL);
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, (long) strlen(data));

    /* get the result or output */
    /*******
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_stream_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
    ********/

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl_);
    /*printf("\nResult = %d -- ID: %s -- len: %d\n", res, str, (int)strlen(str));*/

    /* Check for errors */
    if (res != CURLE_OK) 
    {
        result = SEND_FAILED;
        fprintf(stderr, "send_monitoring_data() failed: %s\n", curl_easy_strerror(res));

    }

    /* in a loop, do a reset instead */
    curl_easy_reset(curl_);
    return result;
}

/* sleep in second */
void delay_time(time_t delay)
{
    time_t timer0, timer1;
    
    if (delay <= 0) {
        return;
    }

    time( &timer0 );

    do {
        time( &timer1 );
    }
    while ( (timer1 - timer0) < delay);
}

void init_curl()
{
    if (curl_ != NULL) {
        return;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curl_ = curl_easy_init();

    headers_ = curl_slist_append(headers_, "Accept: application/json");
    headers_ = curl_slist_append(headers_, "Content-Type: application/json");
    headers_ = curl_slist_append(headers_, "charsets: utf-8");
}

void cleanup_curl()
{
    curl_easy_cleanup(curl_);
    curl_slist_free_all(headers_); /* free the header list */
    curl_global_cleanup();
}

char* get_execution_id(char *URL, char *msg)
{
    CURLcode res;

    /* perform some error checking */
    if (URL == NULL || strlen(URL) == 0) 
    {
        fprintf(stderr, "get_execution_id(): Error - the given url is empty.\n");
        return NULL;
    }

    if (msg == NULL || strlen(msg) == 0) 
    {
        fprintf(stderr, "get_execution_id(): Error - empty message is going to be sent.\n");
        return NULL;
    }

    if (execID_ != NULL && strlen(execID_) > 0) {

        return execID_;
    }

    if (curl_ == NULL) {
        init_curl();
    }

    printf("\nSending curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
    printf("Msg = %s -- len: %d\n", msg, (int) strlen(msg));

    curl_easy_setopt(curl_, CURLOPT_URL, URL);
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, msg);
    curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, (long) strlen(msg));

    /* get the execution ID from the stream data */
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, get_stream_data);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &execID_);

    res = curl_easy_perform(curl_);
    printf("\n-- Execution ID: %s -- len: %d\n", execID_, (int) strlen(execID_));

    if (res != CURLE_OK) 
    {
        fprintf(stderr, "get_execution_id() failed: %s\n", curl_easy_strerror(res));
        return NULL;

    }

    curl_easy_reset(curl_);
    return execID_;
}

/*********************************************************************/
/* these functions should be integrated into monitoring-excess.c */

int is_empty(void)
{
    int result = DATA_AVAILABLE;

    /* could also be END_INDEX ?? */
    if (BEGIN_INDEX == CURRENT_INDEX) {
        result = DATA_EMPTY;
    }
    return result; 
}

/* dummy or redundant function */
void print_sensor_data(sensor_msg_t data)
{
    printf("{\"t_mem\":\"%lu\",\"mem_used\":\"%d\",\"mem_avail\":\"%d\"}\n", 
           data.mem_time.tv_sec, data.ram_used, data.ram_avail);

    printf("{\"t_cpu\":\"%lu\",\"cpu_load\":\"%f\",\"cpu_avail\":\"%f\",\"t_cpu_waiting_io\":\"%f\"}\n", 
           data.cpu_time.tv_sec, data.cpu_used, data.cpu_avail, data.cpu_wa_io);
}

sensor_msg_t dequeue(void)
{
    int i = 0;
    sensor_msg_t data;

    printf("\n\ndequeue(): queue empty = %d, where 0 means not empty\n", is_empty());

    if (is_empty() != DATA_EMPTY)
    {
        data = to_send_msg[BEGIN_INDEX];

        printf("--- Current data ....\n");
        print_sensor_data(data);

        i = BEGIN_INDEX;
        to_send_msg[i].mem_time.tv_sec = 0;
        to_send_msg[i].ram_used = 0;
        to_send_msg[i].ram_avail = 0;
        to_send_msg[i].cpu_time.tv_sec = 0;
        to_send_msg[i].cpu_used = 0;
        to_send_msg[i].cpu_avail = 0;
        to_send_msg[i].cpu_wa_io = 0;

        printf("--- After data is deleted ....\n");
        print_sensor_data(to_send_msg[BEGIN_INDEX]);

        BEGIN_INDEX++;

        /* need to wrap this begin index */
        if (BEGIN_INDEX >= BUFFER_SIZE) {
            BEGIN_INDEX = 0;
        }
    }

    return data;
}


/*********************************************************************/
/* From below onwards are dummy functions to test libcurl */

void print_monitoring_data(void)
{
    int i = 0;
    for (i = 0; i < END_INDEX; i++)
    {
        printf("i = %d, t_mem = %lu, mem_used = %d, mem_avail = %d\n", 
                i, to_send_msg[i].mem_time.tv_sec, to_send_msg[i].ram_used,
                to_send_msg[i].ram_avail);
    }

}

void init_monitoring_data(void)
{
    int i = 0;
    int num = 0;
    double dnum = 0;
    time_t wall_time = time(NULL);
    srand ( wall_time );   /* initialize random seed: */

    BEGIN_INDEX = 0;
    END_INDEX = 0;
    CURRENT_INDEX = 0;

    for (i = 0; i < 10; i++)
    {
        num = rand() % 90; /* random int between 0 and 89 */
        to_send_msg[i].ram_used = num;
        to_send_msg[i].ram_avail = 100 - num;
        to_send_msg[i].mem_time.tv_sec = wall_time;

        dnum = ((double) rand() / RAND_MAX) * 100;
        to_send_msg[i].cpu_used = dnum;
        to_send_msg[i].cpu_avail = 100 - dnum;
        to_send_msg[i].cpu_wa_io = (dnum / 100);
        to_send_msg[i].cpu_time.tv_sec  = wall_time;
        
        /*******
        to_send_msg[i]. = ;
        to_send_msg[i]. = ;
        *****/
        wall_time++;
        END_INDEX++;
        CURRENT_INDEX++;

    }

    /*print_monitoring_data();*/
}

void send_dummy_data(char *URL)
{
    char msg[500] = "";
    int i = 0;
    sensor_msg_t data;


    printf("\n");
    for (i = 0; i < END_INDEX; i++)
    {
        data = dequeue();

        /* send memory info */
        sprintf(msg, "{\"t_mem\":\"%lu\",\"mem_used\":\"%d\",\"mem_avail\":\"%d\"}", 
                data.mem_time.tv_sec, data.ram_used, data.ram_avail);
                /***
                to_send_msg[i].mem_time.tv_sec, to_send_msg[i].ram_used,
                to_send_msg[i].ram_avail);
                ****/

        printf("\n\n-> Sending: %s -- len: %d\n", msg, (int) strlen(msg));
        send_monitoring_data(URL, msg);


        /* send CPU info */
        sprintf(msg,
                "{\"t_cpu\":\"%lu\",\"cpu_load\":\"%f\",\"cpu_avail\":\"%f\",\"t_cpu_waiting_io\":\"%f\"}", 
                data.cpu_time.tv_sec, data.cpu_used, data.cpu_avail, data.cpu_wa_io);
                /****
                to_send_msg[i].cpu_time.tv_sec, to_send_msg[i].cpu_used,
                to_send_msg[i].cpu_avail, to_send_msg[i].cpu_wa_io);
                *****/


        printf("\n\n-> Sending: %s -- len: %d\n", msg, (int) strlen(msg));
        send_monitoring_data(URL, msg);
    }
}

int main(void)
{
    char addr[100] = "http://141.58.5.230:3000/executions/";
    char addr2[100] = "http://141.58.5.230:9200/";
    char str[1000] = "";   /* storing the execution ID -- UUID is 36 chars */
    char msg[1000] = "{\"Name\":\"Execution1\",\"Description\":\"Test\",\"nodes\":[\"node1\",\"node2\",\"node3\"]}";
    int i = 0;

    /* init curl libs */
    init_curl();
    strcpy(str, get_execution_id(addr, msg)); /* get the execution ID */
    strcat(addr, str);  /* append the ID to the end of the URL or IP address */

    /* generate dummy data and send them */
    init_monitoring_data();
    send_dummy_data(addr);

    /**************************
    memset(msg, 0, sizeof(msg));
    strcpy(msg, "{\"Timestamp\":\"1398169317\",\"cpu\":\"0.55\",\"ram\":\"125\"}");
    printf("\n-- New msg: [%s] -- len: %d\n", msg, (int) strlen(msg));
    send_monitoring_data(addr, msg);

    **************************/
    /* http://141.58.5.220:9200/b5fe7pnyqzqsitnqxb3n1a/_search?pretty=true -- lower case for ID */

    /* change to lower case */
    i = 0;
    while (str[i]) 
    {
        str[i] = tolower(str[i]);
        i++;
    }
    sleep(2);  /* in second */
    strcat(addr2, str);
    strcat(addr2, "/_search?pretty=true");
    printf("\n\n---------------------------\nCheck the data for ID: %s\n", str);
    memset(msg, 0, sizeof(msg));
    send_monitoring_data(addr2, msg);

    /* don't forget to clean up the curl libs */
    cleanup_curl();
    return 0;
}

