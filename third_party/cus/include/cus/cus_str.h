
#ifndef CUS_STR_H
#define CUS_STR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

char *cus_str_upper(char *str);
char *cus_str_lower(char *str);
char *cus_str_reverse(char *str);
bool cus_str_startswith(const char *str, const char *start);
bool cus_str_endswith(const char *str, const char *end);
bool cus_str_contains(const char *str, const char *sub);
// n = sizeof(dest_buf)
char *cus_strncpy(char *dest, const char *src, size_t n);
// n = sizeof(dest_buf)
char *cus_strncat(char *dest, const char *src, size_t n);


#ifdef __cplusplus
}
#endif

#endif //CUS_STR_H
