#ifndef __DCF_INTERFACE_H__
#define __DCF_INTERFACE_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define EXPORT_API
typedef enum { DCF_ROLE_UNKNOWN=0, DCF_ROLE_LEADER, DCF_ROLE_FOLLOWER, DCF_ROLE_LOGGER, DCF_ROLE_PASSIVE, DCF_ROLE_PRE_CANDIDATE, DCF_ROLE_CANDIDATE } dcf_role_t;
typedef enum { DCF_RUNNING_NORMAL=0, DCF_EXCEPTION_MISSING_LOG } dcf_exception_t;
typedef enum { DCF_INDEX_UNKNOWN=0, DCF_LOCAL_COMMIT_INDEX, DCF_LEADER_COMMIT_INDEX, DCF_CONSENSUS_COMMIT_INDEX } dcf_commit_index_type_t;
typedef enum { WM_NORMAL=0, WM_MINORITY } dcf_work_mode_t;
typedef int (*usr_cb_election_notify_t)(unsigned int, unsigned int);
typedef int (*usr_cb_msg_proc_t)(unsigned int, unsigned int, const char*, unsigned int);
typedef int (*usr_cb_after_writer_t)(unsigned int, unsigned long long, const char*, unsigned int, unsigned long long, int);
typedef int (*usr_cb_consensus_notify_t)(unsigned int, unsigned long long, const char*, unsigned int, unsigned long long);
typedef int (*usr_cb_status_notify_t)(unsigned int, dcf_role_t);
typedef void (*usr_cb_log_output_t)(int, int, const char*, unsigned int, const char*, const char*, ...);
typedef int (*usr_cb_decrypt_pwd_t)(const char*, unsigned int, char*, unsigned int);
typedef int (*usr_cb_exception_notify_t)(unsigned int, dcf_exception_t);
typedef void (*usr_cb_thread_memctx_init_t)();
EXPORT_API int dcf_set_param(const char*, const char*);
EXPORT_API int dcf_get_param(const char*, char*, unsigned int);
EXPORT_API int dcf_register_after_writer(usr_cb_after_writer_t);
EXPORT_API int dcf_register_consensus_notify(usr_cb_consensus_notify_t);
EXPORT_API int dcf_register_status_notify(usr_cb_status_notify_t);
EXPORT_API int dcf_register_log_output(usr_cb_log_output_t);
EXPORT_API int dcf_register_exception_report(usr_cb_exception_notify_t);
EXPORT_API int dcf_register_election_notify(usr_cb_election_notify_t);
EXPORT_API int dcf_register_msg_proc(usr_cb_msg_proc_t);
EXPORT_API int dcf_register_thread_memctx_init(usr_cb_thread_memctx_init_t);
EXPORT_API int dcf_register_decrypt_pwd(usr_cb_decrypt_pwd_t);
EXPORT_API int dcf_start(unsigned int, const char*);
EXPORT_API int dcf_write(unsigned int, const char*, unsigned int, unsigned long long, unsigned long long*);
EXPORT_API int dcf_universal_write(unsigned int, const char*, unsigned int, unsigned long long, unsigned long long*);
EXPORT_API int dcf_read(unsigned int, unsigned long long, char*, unsigned int);
EXPORT_API int dcf_stop();
EXPORT_API int dcf_truncate(unsigned int, unsigned long long);
EXPORT_API int dcf_set_applied_index(unsigned int, unsigned long long);
EXPORT_API int dcf_get_cluster_min_applied_idx(unsigned int, unsigned long long*);
EXPORT_API int dcf_get_leader_last_index(unsigned int, unsigned long long*);
EXPORT_API int dcf_get_last_index(unsigned int, unsigned long long*);
EXPORT_API int dcf_query_cluster_info(char*, unsigned int);
EXPORT_API int dcf_query_stream_info(unsigned int, char*, unsigned int);
EXPORT_API int dcf_query_leader_info(unsigned int, char*, unsigned int, unsigned int*, unsigned int*);
EXPORT_API int dcf_get_errorno();
EXPORT_API const char* dcf_get_error(int);
EXPORT_API const char* dcf_get_version();
EXPORT_API int dcf_get_node_last_disk_index(unsigned int, unsigned int, unsigned long long*);
EXPORT_API int dcf_add_member(unsigned int, unsigned int, const char*, unsigned int, dcf_role_t, unsigned int);
EXPORT_API int dcf_remove_member(unsigned int, unsigned int, unsigned int);
EXPORT_API int dcf_change_member_role(unsigned int, unsigned int, dcf_role_t, unsigned int);
EXPORT_API int dcf_change_member(const char*, unsigned int);
EXPORT_API int dcf_promote_leader(unsigned int, unsigned int, unsigned int);
EXPORT_API int dcf_timeout_notify(unsigned int);
EXPORT_API int dcf_node_is_healthy(unsigned int, dcf_role_t*, unsigned int*);
EXPORT_API int dcf_set_work_mode(unsigned int, dcf_work_mode_t, unsigned int);
EXPORT_API int dcf_query_statistics_info(char*, unsigned int);
EXPORT_API int dcf_check_if_all_logs_applied(unsigned int, unsigned int*);
EXPORT_API int dcf_set_trace_key(unsigned long long);
void dcf_set_exception(int, dcf_exception_t);
EXPORT_API int dcf_send_msg(unsigned int, unsigned int, const char*, unsigned int);
EXPORT_API int dcf_broadcast_msg(unsigned int, const char*, unsigned int);
EXPORT_API int dcf_pause_rep(unsigned int, unsigned int, unsigned int);
EXPORT_API int dcf_demote_follower(unsigned int);
EXPORT_API int dcf_get_data_commit_index(unsigned int, dcf_commit_index_type_t, unsigned long long*);
EXPORT_API int dcf_get_current_term_and_role(unsigned int, unsigned long long*, dcf_role_t*);
EXPORT_API int dcf_set_election_priority(unsigned int, unsigned long long);
EXPORT_API void dcf_set_timer(void*);
#ifdef __cplusplus
}
#endif
#endif
