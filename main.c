#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
//#define MHD_PLATFORM_H
//microhttpd page advises to use the above, but compilation fails
#include <string.h>
#include <microhttpd.h>
#include <stdlib.h>

#include <stdio.h>
#define PORT 8888

//helper functions
#include "json.h"
#include "url.h"
//terminal management
#include "terminal.h"

#include <stdbool.h> //needed for post processing, can do without it though

//should be able to display all terminals on one page
//since there is no pagination (yet)
#define MAX_PAGE_SIZE MAX_TERMINALS * MAX_TERMINAL_SIZE + 1024


#define MAX_BUFFER 1024

//struct for post processing callback
struct cinfo_struct
{
	struct MHD_PostProcessor *pp;
	char *data;
};
//struct to keep post status
struct postStatus {
	bool status;
	char *data;
};


static int
POST_iterator (void *cls,
               enum MHD_ValueKind kind,
               const char *key,
               const char *filename,
               const char *content_type,
               const char *transfer_encoding,
               const char *data, uint64_t off, size_t size)
{
  struct cinfo_struct *cinfo = cls;
  //struct Session *session = request->session;
  (void)kind;              /* Unused. Silent compiler warning. */
  (void)filename;          /* Unused. Silent compiler warning. */
  (void)content_type;      /* Unused. Silent compiler warning. */
  (void)transfer_encoding; /* Unused. Silent compiler warning. */

	printf("inside post processor\n");
	if(NULL != cinfo){
		if(cinfo->data != NULL)
			printf("data %s\n", cinfo->data);
		else
			printf("data null\n");
	
	}else{
		printf("cinfo null\n");
	}

  return MHD_YES;
}


//response handler
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
		const char *url, const char *method,
		const char *version, const char *upload_data,
		size_t *upload_data_size, void **con_cls)
{
	
	char tmp[MAX_PAGE_SIZE]; 						//temporary buffer to store data
	strcpy(tmp,"\"BUFFER\":\"UNINITIALISED\""); 	//if data has not been handled we will get this
	int uinfo = url_get_info(url,method); 			
	if(uinfo & URL_ERROR) json_error(tmp,"Invalid URL");
	else if (uinfo & METHOD_POST){					//process POST request
		//try example from documentation
		struct cinfo_struct *cinfo;
		cinfo = *con_cls;
		if(cinfo == NULL){
			printf("create cinfo\n");
			cinfo = calloc(0,sizeof (struct cinfo_struct));
			*con_cls = cinfo;
			cinfo->pp = MHD_create_post_processor(connection, MAX_BUFFER, &POST_iterator, (void*)cinfo);
			return MHD_YES;
		}
		printf("start PP\n");
		MHD_post_process(cinfo->pp, upload_data, *upload_data_size);
		if(*upload_data_size != 0){
			*upload_data_size = 0;
			return MHD_YES;
		}
		printf("destroing post processor\n");
		MHD_destroy_post_processor(cinfo->pp);
		
		
		//prepare post data
		
/* The post processsing functionality is extremely unstable
 * Segfaults are common and pretty random - no idea what causes it
 * No time to dal with this, will have to skip this functionality
 */
/*		printf("process post");
		char* post_data;
		struct postStatus *post = NULL;
		post = (struct postStatus*)*con_cls;

		if(post == NULL) {
			if(*upload_data_size == 0 ) printf ("datasize 0\n");
			post = malloc(sizeof(struct postStatus));
			post->status = false;
			*con_cls = post;
		}


		if(!post->status) {
			post->status = true;
			return MHD_YES;
		} else {
			if(*upload_data_size != 0) {
				printf("data [%s] size %d\n",upload_data, *upload_data_size);
				post->buff = (char*)malloc(*upload_data_size + 1);
				sprintf(post->buff, "%s",upload_data);
				printf("postbuff [%s] size: %d\n",post->buff,strlen(post->buff));
				*upload_data_size = 0;
				return MHD_YES;
			} else {
				if(post->buff != NULL){
					printf("Post data: [%s]\n",post->buff);
					post_data=(char*)malloc(strlen(post->buff)+2);
					sprintf(post_data,"{%s}",post->buff);
					free(post->buff);
				}else{
					post_data=(char*)malloc(strlen("No data")+1);
					sprintf(post_data,"No data");
				}
			}
		} 

		//if(post != NULL){
		//	free(post);
		//}
*/		if(uinfo & URL_TERMINAL || uinfo == METHOD_POST){//empty POST will create terminal	
			int id = add_terminal();
			if(id == -1) json_error(tmp,"Could not create terminal");
			else json_int(tmp,"terminalID",id);
		}else if(uinfo & URL_TERMINAL_ID){		//reserved for POST method to update terminal
												//not implemented
			json_str(tmp,"POST update terminal","This feature is not yet implemented"); 
		}else if(uinfo & URL_TERMINALS){		//cannot use POST for listing info
			json_error(tmp,"use GET method");
		}
	}else{
		if(uinfo & URL_TERMINAL) json_error(tmp,"Use POST method");
												//cant create terminal with GET
		else if(uinfo & URL_TERMINAL_ID){
			int id = url_get_id(url);
			if(id == -1) json_error(tmp,"Invalid terminal ID");
			else show_terminal_info(tmp,id);
		}else if(uinfo & URL_TERMINALS){		//list terminals or display error if none
			if(list_terminals(tmp, MAX_PAGE_SIZE) == -1) json_error(tmp,"No terminals");
		}

	}

	char *data=(char*)malloc(strlen(tmp) + 3);	//prepare response buffer
	sprintf(data,"{%s}",tmp);
	struct MHD_Response *response;
	int ret;
	response = MHD_create_response_from_buffer (strlen (data), (void *) data,
			MHD_RESPMEM_PERSISTENT);
	//add json headers
	MHD_add_response_header(response,"Content-Type", "application/json");
	MHD_add_response_header(response,"Accept", "application/json");
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);
	//free(data); - causes memory corruption and accasional segfault 
	return ret;
}

int main ()
{
	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon (MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL,
			&answer_to_connection, NULL, MHD_OPTION_END);
	if (NULL == daemon)
		return 1;
	(void) getchar ();
	MHD_stop_daemon (daemon);
	return 0;
}
