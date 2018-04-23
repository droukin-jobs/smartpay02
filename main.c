/* Feel free to use this example code in any way
   you see fit (Public Domain) */

#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//helper functions
#include "json.h"
#include "url.h"
//terinal management
#include "terminal.h"

#define MAX_PAGE_SIZE MAX_TERMINALS * MAX_TERMINAL_SIZE + 1024

#define MAX_BUFFER 1024

#if defined(_MSC_VER) && _MSC_VER+0 <= 1800
/* Substitution is OK while return value is not used */
#define snprintf _snprintf
#endif

#define PORT            8888
//#define POSTBUFFERSIZE  512
#define POSTBUFFERSIZE  1024
#define MAXNAMESIZE     20
//#define MAXANSWERSIZE   512
#define MAXANSWERSIZE   1024

#define GET             0
#define POST            1

extern char cards[3][11];
extern char accts[3][11];

const char *json_page = "{%s}\n";

struct connection_info_struct
{
  int citype;
  char *cidata;
  struct MHD_PostProcessor *pp;
};



static int
send_page (struct MHD_Connection *connection, const char *page)
{
  int ret;
  struct MHD_Response *response;

	printf("PAGE %s\n",page);	

  response =
    MHD_create_response_from_buffer (strlen (page), (void *) page,
				     MHD_RESPMEM_PERSISTENT);
  if (!response){
		printf("response = null\n");
		return MHD_NO;

  }

  MHD_add_response_header(response, "Content-Type","application/json");
  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}


static int
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size)
{
  struct connection_info_struct *con_info = coninfo_cls;
  (void)kind;               /* Unused. Silent compiler warning. */
  (void)filename;           /* Unused. Silent compiler warning. */
  (void)content_type;       /* Unused. Silent compiler warning. */
  (void)transfer_encoding;  /* Unused. Silent compiler warning. */
  (void)off;                /* Unused. Silent compiler warning. */

  //if (0 == strcmp (key, "json"))
    {
		printf("iteratro JSON data %s len %d\n",data,size);
		printf("JSON debug sizeof(size_t) %d\n",sizeof(size_t));
      if ((size > 0) && (size <= POSTBUFFERSIZE))
        {
          char *cidata;
          cidata = malloc (MAXANSWERSIZE);
          if (!cidata){
            	printf("could not allocate cidata with %d bytes\n",MAXANSWERSIZE);
			  return MHD_NO;
		  }
			printf("iterator will copy dat to cidata\n");
          snprintf (cidata, MAXANSWERSIZE, json_page, data);
          con_info->cidata = cidata;
		  printf("iterator cidata=%s\n",cidata);
        }
      else{
        con_info->cidata = NULL;
	  	printf("set cidata = NULL\n");
	  }

      return MHD_NO;
    }

  return MHD_YES;
}

static void
request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *con_cls;
  (void)cls;         /* Unused. Silent compiler warning. */
  (void)connection;  /* Unused. Silent compiler warning. */
  (void)toe;         /* Unused. Silent compiler warning. */

  if (NULL == con_info)
    return;

  if (con_info->citype == POST)
    {
		printf("destroy POST processor\n");
      MHD_destroy_post_processor (con_info->pp);
      if (con_info->cidata)
        free (con_info->cidata);
    }

  free (con_info);
  *con_cls = NULL;
  printf("====== request completed =============\n");
}


static int
answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls){
  	(void)cls;               /* Unused. Silent compiler warning. */
  	(void)url;               /* Unused. Silent compiler warning. */
  	(void)version;           /* Unused. Silent compiler warning. */

	//check if session just started
	if (NULL == *con_cls){
		struct connection_info_struct *con_info;
		con_info = malloc (sizeof (struct connection_info_struct));
		if (NULL == con_info) //cant allocate memory
			return MHD_NO;
		con_info->cidata = NULL;
		if (0 == strcmp (method, "POST")){
			//setup post processor
			con_info->pp = MHD_create_post_processor (connection, POSTBUFFERSIZE, iterate_post, (void *) con_info);
			if (NULL == con_info->pp){
				free (con_info);
				return MHD_NO;
			}
			con_info->citype = POST;
		}else
			con_info->citype = GET;

		*con_cls = (void *) con_info;//update con_cls pointer for next callback
		return MHD_YES;
	}
	
	//prepare default value
	char *tmp = (char*)malloc(MAX_PAGE_SIZE);
	json_str(tmp,"buffer","uninitialized");
	int uinfo = url_get_info(url,method);
	printf("ULR %s info %d\n",url, uinfo);
	//process url options
	if(uinfo & URL_ERROR) json_error(tmp,"Invalid URL");
	else if (uinfo & METHOD_POST){                  //process POST request
		printf("processing POST\n");
		if(uinfo & URL_TERMINAL){//empty POST will create terminal  
			printf("will add terminal\n");
			int id = add_terminal();
			printf("new term id = %d\n",id);
			if(id == -1) json_error(tmp,"Could not create terminal");
			else json_int(tmp,"TerminalID",id);
		}else if(uinfo & URL_TERMINAL_ID){      //POST method to update terminal
			int id = url_get_id(url);
			if(id == -1) json_error(tmp,"Invalid terminal ID");
			else{
				struct connection_info_struct *con_info = *con_cls;
				//if there is still data - send to iterator
				if (*upload_data_size != 0){
					printf("Call MHD post process\n");
					MHD_post_process (con_info->pp, upload_data,*upload_data_size);
					*upload_data_size = 0;
					return MHD_YES;
				}else if (NULL != con_info->cidata){
					//dat ais ready to be processed
					char *d = (char*)malloc(strlen(con_info->cidata) + 1);
					sprintf(d,"%s",con_info->cidata);
					printf("data %s; len %d\n",d,strlen(d));
					//should be set as globals
					int i;
					int c_id=-1;
					int a_id=-1;
					//temporary vars to hold terminal data
					char c[50];
					char a[50];
					sprintf(c,"Unknown");
					sprintf(a,"Unknown");
					printf("JSON string: %s\n",d);
					for(i=0;i<3;i++){
						printf("check %d %s %s\n",i,cards[i],accts[i]);
						if(strstr(d,cards[i]) != NULL){
							c_id = i;
							printf("found %s\n",cards[i]);
							sprintf(c,"%s",cards[i]);
						}
					}
					for(i=0;i<3;i++){
						if(strstr(d,accts[i])!=NULL){
							a_id = i;
							printf("found %s\n",accts[i]);
							sprintf(a,"%s",accts[i]);
						}
					}
					//TODO if transactions oK - add transaction
					if(c_id != -1 && a_id != -1) {
						if(add_transaction(id,c_id,a_id)!=-1){
							show_terminal_info(tmp,id);
						}
						else json_error(tmp,"Could not create transaction");
					}else{
						json_error(tmp,"Invalid transaction");
					}	
				}
			}
		}else if(uinfo & URL_TERMINALS){        //cannot use POST for listing info
				json_error(tmp,"use GET method");
		}

	}
  	if (uinfo & METHOD_GET){
		if(uinfo & URL_TERMINAL) json_error(tmp,"Use POST method");
		//cant create terminal with GET
		else if(uinfo & URL_TERMINAL_ID){
			int id = url_get_id(url);
			if(id == -1) json_error(tmp,"Invalid terminal ID");
			else show_terminal_info(tmp,id);
		}else if(uinfo & URL_TERMINALS){        //list terminals or display error if none
			if(list_terminals(tmp, MAX_PAGE_SIZE) == -1) json_error(tmp,"No terminals");
		}
	}
	char tmptmp[MAX_PAGE_SIZE];
	sprintf(tmptmp,json_page, tmp);
    return send_page(connection, tmptmp);
}

int
main ()
{
  struct MHD_Daemon *daemon;
	init();//init terminal data
  daemon = MHD_start_daemon (MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                             &answer_to_connection, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, request_completed,
                             NULL, MHD_OPTION_END);
  if (NULL == daemon)
    return 1;

  (void) getchar ();

  MHD_stop_daemon (daemon);

  return 0;
}
