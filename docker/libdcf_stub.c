#include <stdint.h>
#include <stddef.h>
typedef unsigned long long ull_t;
typedef enum { DCF_ROLE_UNKNOWN=0, DCF_ROLE_LEADER, DCF_ROLE_FOLLOWER, DCF_ROLE_LOGGER, DCF_ROLE_PASSIVE, DCF_ROLE_PRE_CANDIDATE, DCF_ROLE_CANDIDATE } dcf_role_t;
typedef enum { DCF_RUNNING_NORMAL=0, DCF_EXCEPTION_MISSING_LOG } dcf_exception_t;
typedef enum { DCF_INDEX_UNKNOWN=0, DCF_LOCAL_COMMIT_INDEX, DCF_LEADER_COMMIT_INDEX, DCF_CONSENSUS_COMMIT_INDEX } dcf_commit_index_type_t;
typedef enum { WM_NORMAL=0, WM_MINORITY } dcf_work_mode_t;
typedef int (*usr_cb_election_notify_t)(unsigned int, unsigned int);
typedef int (*usr_cb_msg_proc_t)(unsigned int, unsigned int, const char*, unsigned int);
typedef int (*usr_cb_after_writer_t)(unsigned int, ull_t, const char*, unsigned int, ull_t, int);
typedef int (*usr_cb_consensus_notify_t)(unsigned int, ull_t, const char*, unsigned int, ull_t);
typedef int (*usr_cb_status_notify_t)(unsigned int, dcf_role_t);
typedef void (*usr_cb_log_output_t)(int, int, const char*, unsigned int, const char*, const char*, ...);
typedef int (*usr_cb_decrypt_pwd_t)(const char*, unsigned int, char*, unsigned int);
typedef int (*usr_cb_exception_notify_t)(unsigned int, dcf_exception_t);
typedef void (*usr_cb_thread_memctx_init_t)();

int dcf_set_param(const char *a, const char *b) { (void)a;(void)b; return 0; }
int dcf_get_param(const char *a, char *b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_register_after_writer(void *f) { (void)f; return 0; }
int dcf_register_consensus_notify(void *f) { (void)f; return 0; }
int dcf_register_status_notify(void *f) { (void)f; return 0; }
int dcf_register_log_output(void *f) { (void)f; return 0; }
int dcf_register_exception_report(void *f) { (void)f; return 0; }
int dcf_register_election_notify(void *f) { (void)f; return 0; }
int dcf_register_msg_proc(void *f) { (void)f; return 0; }
int dcf_register_thread_memctx_init(void *f) { (void)f; return 0; }
int dcf_register_decrypt_pwd(void *f) { (void)f; return 0; }
int dcf_start(unsigned int a, const char *b) { (void)a;(void)b; return 0; }
int dcf_write(unsigned int a, const char *b, unsigned int c, ull_t d, ull_t *e) { (void)a;(void)b;(void)c;(void)d;if(e)*e=0; return 0; }
int dcf_universal_write(unsigned int a, const char *b, unsigned int c, ull_t d, ull_t *e) { (void)a;(void)b;(void)c;(void)d;if(e)*e=0; return 0; }
int dcf_read(unsigned int a, ull_t b, char *c, unsigned int d) { (void)a;(void)b;(void)c;(void)d; return 0; }
int dcf_stop() { return 0; }
int dcf_truncate(unsigned int a, ull_t b) { (void)a;(void)b; return 0; }
int dcf_set_applied_index(unsigned int a, ull_t b) { (void)a;(void)b; return 0; }
int dcf_get_cluster_min_applied_idx(unsigned int a, ull_t *b) { (void)a;if(b)*b=0; return 0; }
int dcf_get_leader_last_index(unsigned int a, ull_t *b) { (void)a;if(b)*b=0; return 0; }
int dcf_get_last_index(unsigned int a, ull_t *b) { (void)a;if(b)*b=0; return 0; }
int dcf_query_cluster_info(char *a, unsigned int b) { if(a)*a=0;(void)b; return 0; }
int dcf_query_stream_info(unsigned int a, char *b, unsigned int c) { (void)a;if(b)*b=0;(void)c; return 0; }
int dcf_query_leader_info(unsigned int a, char *b, unsigned int c, unsigned int *d, unsigned int *e) { (void)a;if(b)*b=0;(void)c;if(d)*d=0;if(e)*e=0; return 0; }
int dcf_get_errorno() { return 0; }
const char* dcf_get_error(int c) { (void)c; return ""; }
const char* dcf_get_version() { return "stub-0.1"; }
int dcf_get_node_last_disk_index(unsigned int a, unsigned int b, ull_t *c) { (void)a;(void)b;if(c)*c=0; return 0; }
int dcf_add_member(unsigned int a, unsigned int b, const char *c, unsigned int d, dcf_role_t e, unsigned int f) { (void)a;(void)b;(void)c;(void)d;(void)e;(void)f; return 0; }
int dcf_remove_member(unsigned int a, unsigned int b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_change_member_role(unsigned int a, unsigned int b, dcf_role_t c, unsigned int d) { (void)a;(void)b;(void)c;(void)d; return 0; }
int dcf_change_member(const char *a, unsigned int b) { (void)a;(void)b; return 0; }
int dcf_promote_leader(unsigned int a, unsigned int b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_timeout_notify(unsigned int a) { (void)a; return 0; }
int dcf_node_is_healthy(unsigned int a, dcf_role_t *b, unsigned int *c) { (void)a;if(b)*b=0;if(c)*c=1; return 0; }
int dcf_set_work_mode(unsigned int a, dcf_work_mode_t b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_query_statistics_info(char *a, unsigned int b) { if(a)*a=0;(void)b; return 0; }
int dcf_check_if_all_logs_applied(unsigned int a, unsigned int *b) { (void)a;if(b)*b=1; return 0; }
int dcf_set_trace_key(ull_t a) { (void)a; return 0; }
void dcf_set_exception(int a, dcf_exception_t b) { (void)a;(void)b; }
int dcf_send_msg(unsigned int a, unsigned int b, const char *c, unsigned int d) { (void)a;(void)b;(void)c;(void)d; return 0; }
int dcf_broadcast_msg(unsigned int a, const char *b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_pause_rep(unsigned int a, unsigned int b, unsigned int c) { (void)a;(void)b;(void)c; return 0; }
int dcf_demote_follower(unsigned int a) { (void)a; return 0; }
int dcf_get_data_commit_index(unsigned int a, dcf_commit_index_type_t b, ull_t *c) { (void)a;(void)b;if(c)*c=0; return 0; }
int dcf_get_current_term_and_role(unsigned int a, ull_t *b, dcf_role_t *c) { (void)a;if(b)*b=0;if(c)*c=0; return 0; }
int dcf_set_election_priority(unsigned int a, ull_t b) { (void)a;(void)b; return 0; }
void dcf_set_timer(void *a) { (void)a; }
