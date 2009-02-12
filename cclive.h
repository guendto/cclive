#ifndef cclive_h
#define cclive_h

#include "cmdline.h"
#include "llst.h"

#define ToMB(b) (b/0x100000)

#define FREE(p) \
    do { free(p); p=0; } while(0)

struct _mem_s {
    long size;
    char *p;
};
typedef struct _mem_s *mem_t;

/* should contain only vars expected to last through the program lifespan */ 
struct cclive_s {
    struct gengetopt_args_info gi;
    llst_node_t fnames;
    struct CURL *curl;
    char *curl_errmsg;
};
extern struct cclive_s cc;

int
fetch_link(const char *url, mem_t page, const int enable_log);

char *
strsub(const char *s, const char *from, const char *to);

char *
strrepl(const char *s, const char *what, const char *with);

void
strrmch(char *s, const char c);

size_t
strlcat(char *dst, const char *src, size_t size);

int
handle_host(const char *url);

int
cc_log(const char *fmt, ...);

void
parse_opts(const int argc, char **argv);

double
file_exists(const char *path);

int
prep_video(const char *xurl, const char *id, const char *host);

void
list_hosts(void);

int
login_youtube(void);

size_t
writemem_cb(void *p, size_t size, size_t nmemb, void *data);

#endif
