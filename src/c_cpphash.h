#ifndef C_CPPHASH_H
#define C_CPPHASH_H

#ifdef __cplusplus
#  define EXTERNC extern "C"
#else
#  define EXTERNC
#endif

EXTERNC int find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header);

#endif // C_CPPHASH_H
