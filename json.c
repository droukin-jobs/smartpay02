#include <stdio.h>
#include <stdlib.h>
#include "json.h"

//json helper functions
//produces int, float and string elements
//error creates 'error' message
inline void json_int(char* tmp, const char* var, const int n){
	sprintf(tmp,"\"%s\":\"%d\"",var,n);
}
inline void json_float(char* tmp, const char* var, const float n){
	sprintf(tmp,"\"%s\":\"%f\"",var,n);
}
inline void json_str(char* tmp, const char* var, const char* n){
	sprintf(tmp,"\"%s\":\"%s\"",var,n);
}
inline void json_error(char* tmp, const char* n){
	sprintf(tmp, "\"error\":\"%s\"",n);
}

