#ifndef cclive_h
#define cclive_h

#include "cmdline.h"
#include "llst.h"

#define ToMB(b) (b/0x100000)

typedef struct cc_mem_s * cc_mem_t;

struct cclive_s {
    struct cmdline_parser_params *pp;
    struct gengetopt_args_info gi;
    char *http_proxy_env;
    llst_node_t fnames;
    struct CURL *curl;
    char *curl_errmsg;
    char exec_mode;
};
extern struct cclive_s cc;


int
fetch_link(char *url, cc_mem_t page, int enable_log);

int
dl_file(char *xurl, char *fn, double initial, double total);

char *
create_fname(double *initial, double total, char *id, char *suffix, char *host);

char *
strsub(const char *s, const char *from, const char *to);

char *
strrepl(const char *s, const char *what, const char *with);

void
strrmch(char *s, const char c);

int
handle_host(char *url);

int
cc_log(const char *fmt, ...);

void
parse_opts(int argc, char **argv);

double
file_exists(char *path);

int
prep_video(char *xurl, char *id, char *host);

void
list_hosts(void);

int
login_youtube(void);

#endif
