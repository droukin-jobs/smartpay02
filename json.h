#ifndef _json_h
#define _json_h

//json helper functions
#define json_int(tmp, var, n) sprintf((tmp),"\"%s\":\"%d\"",(var),(n))

#define json_float(tmp, var, n) sprintf((tmp),"\"%f\":\"%d\"",(var),(n))

#define json_str(tmp, var, n) sprintf((tmp),"\"%s\":\"%s\"",(var),(n))

#define json_error(tmp, var) sprintf((tmp),"\"error\":\"%s\"",(var))

#endif //ifndef _json_h
