#ifndef cclive_h
#define cclive_h

#include "cmdline.h"

#define ToMB(b) (b/0x100000)

struct cclive_s {
    struct cmdline_parser_params *pp;
    struct gengetopt_args_info gi;
    char *http_proxy_env;
    struct CURL *curl;
    char *curl_errmsg;
};

extern struct cclive_s cc;

typedef struct cc_mem_s * cc_mem_t;

extern int
cc_fetch(char *url, cc_mem_t page, int enable_log);

extern long
cc_getlen(char *xurl, double *length, char **contenttype);

extern int
cc_get(char *xurl, char *fn, double initial, double total);

extern char *
cc_getfn(double *initial, double total, char *id, char *suffix, char *host);

extern char *
cc_strsub(const char *s, const char *from, const char *to);

extern char *
cc_strrepl(const char *s, const char *what, const char *with);

extern int
cc_strget(const char *from, const char *to, char *data, char *dst, size_t size);

extern int
cc_handle_host(char *url);

extern int
cc_log(const char *fmt, ...);

extern void
cc_parse_opts(int argc, char **argv);

extern double
cc_file_exists(char *path);

extern int
cc_extract_video(char *xurl, char *id, char *host);

extern void
cc_list_hosts(void);

extern int
cc_login_youtube(void);

#endif
