// ESP32 stub definitions for ZeroTier Metrics
// Replaces Metrics.cpp which uses full prometheus library
#include "Metrics.hpp"

namespace ZeroTier {
namespace Metrics {

prometheus::simpleapi::counter_family_t packets;
prometheus::simpleapi::counter_metric_t pkt_nop_in;
prometheus::simpleapi::counter_metric_t pkt_error_in;
prometheus::simpleapi::counter_metric_t pkt_ack_in;
prometheus::simpleapi::counter_metric_t pkt_qos_in;
prometheus::simpleapi::counter_metric_t pkt_hello_in;
prometheus::simpleapi::counter_metric_t pkt_ok_in;
prometheus::simpleapi::counter_metric_t pkt_whois_in;
prometheus::simpleapi::counter_metric_t pkt_rendezvous_in;
prometheus::simpleapi::counter_metric_t pkt_frame_in;
prometheus::simpleapi::counter_metric_t pkt_ext_frame_in;
prometheus::simpleapi::counter_metric_t pkt_echo_in;
prometheus::simpleapi::counter_metric_t pkt_multicast_like_in;
prometheus::simpleapi::counter_metric_t pkt_network_credentials_in;
prometheus::simpleapi::counter_metric_t pkt_network_config_request_in;
prometheus::simpleapi::counter_metric_t pkt_network_config_in;
prometheus::simpleapi::counter_metric_t pkt_multicast_gather_in;
prometheus::simpleapi::counter_metric_t pkt_multicast_frame_in;
prometheus::simpleapi::counter_metric_t pkt_push_direct_paths_in;
prometheus::simpleapi::counter_metric_t pkt_user_message_in;
prometheus::simpleapi::counter_metric_t pkt_remote_trace_in;
prometheus::simpleapi::counter_metric_t pkt_path_negotiation_request_in;
prometheus::simpleapi::counter_metric_t pkt_nop_out;
prometheus::simpleapi::counter_metric_t pkt_error_out;
prometheus::simpleapi::counter_metric_t pkt_ack_out;
prometheus::simpleapi::counter_metric_t pkt_qos_out;
prometheus::simpleapi::counter_metric_t pkt_hello_out;
prometheus::simpleapi::counter_metric_t pkt_ok_out;
prometheus::simpleapi::counter_metric_t pkt_whois_out;
prometheus::simpleapi::counter_metric_t pkt_rendezvous_out;
prometheus::simpleapi::counter_metric_t pkt_frame_out;
prometheus::simpleapi::counter_metric_t pkt_ext_frame_out;
prometheus::simpleapi::counter_metric_t pkt_echo_out;
prometheus::simpleapi::counter_metric_t pkt_multicast_like_out;
prometheus::simpleapi::counter_metric_t pkt_network_credentials_out;
prometheus::simpleapi::counter_metric_t pkt_network_config_request_out;
prometheus::simpleapi::counter_metric_t pkt_network_config_out;
prometheus::simpleapi::counter_metric_t pkt_multicast_gather_out;
prometheus::simpleapi::counter_metric_t pkt_multicast_frame_out;
prometheus::simpleapi::counter_metric_t pkt_push_direct_paths_out;
prometheus::simpleapi::counter_metric_t pkt_user_message_out;
prometheus::simpleapi::counter_metric_t pkt_remote_trace_out;
prometheus::simpleapi::counter_metric_t pkt_path_negotiation_request_out;
prometheus::simpleapi::counter_family_t packet_errors;
prometheus::simpleapi::counter_metric_t pkt_error_obj_not_found_in;
prometheus::simpleapi::counter_metric_t pkt_error_unsupported_op_in;
prometheus::simpleapi::counter_metric_t pkt_error_identity_collision_in;
prometheus::simpleapi::counter_metric_t pkt_error_need_membership_cert_in;
prometheus::simpleapi::counter_metric_t pkt_error_network_access_denied_in;
prometheus::simpleapi::counter_metric_t pkt_error_unwanted_multicast_in;
prometheus::simpleapi::counter_metric_t pkt_error_authentication_required_in;
prometheus::simpleapi::counter_metric_t pkt_error_internal_server_error_in;
prometheus::simpleapi::counter_metric_t pkt_error_obj_not_found_out;
prometheus::simpleapi::counter_metric_t pkt_error_unsupported_op_out;
prometheus::simpleapi::counter_metric_t pkt_error_identity_collision_out;
prometheus::simpleapi::counter_metric_t pkt_error_need_membership_cert_out;
prometheus::simpleapi::counter_metric_t pkt_error_network_access_denied_out;
prometheus::simpleapi::counter_metric_t pkt_error_unwanted_multicast_out;
prometheus::simpleapi::counter_metric_t pkt_error_authentication_required_out;
prometheus::simpleapi::counter_metric_t pkt_error_internal_server_error_out;
prometheus::simpleapi::counter_family_t data;
prometheus::simpleapi::counter_metric_t udp_send;
prometheus::simpleapi::counter_metric_t udp_recv;
prometheus::simpleapi::counter_metric_t tcp_send;
prometheus::simpleapi::counter_metric_t tcp_recv;
prometheus::simpleapi::gauge_metric_t network_num_joined;
prometheus::simpleapi::gauge_family_t network_num_multicast_groups;
prometheus::simpleapi::counter_family_t network_packets;
static prometheus::BuilderT<prometheus::Histogram<uint64_t>> _peer_latency_impl;
prometheus::CustomFamily<prometheus::Histogram<uint64_t>>& peer_latency = _peer_latency_impl;
prometheus::simpleapi::gauge_family_t peer_path_count;
prometheus::simpleapi::counter_family_t peer_packets;
prometheus::simpleapi::counter_family_t peer_packet_errors;
prometheus::simpleapi::gauge_metric_t network_count;
prometheus::simpleapi::gauge_metric_t member_count;
prometheus::simpleapi::counter_metric_t network_changes;
prometheus::simpleapi::counter_metric_t member_changes;
prometheus::simpleapi::counter_metric_t member_auths;
prometheus::simpleapi::counter_metric_t member_deauths;
prometheus::simpleapi::gauge_metric_t network_config_request_queue_size;
prometheus::simpleapi::counter_metric_t sso_expiration_checks;
prometheus::simpleapi::counter_metric_t sso_member_deauth;
prometheus::simpleapi::counter_metric_t network_config_request;
prometheus::simpleapi::gauge_metric_t network_config_request_threads;
prometheus::simpleapi::counter_metric_t db_get_network;
prometheus::simpleapi::counter_metric_t db_get_network_and_member;
prometheus::simpleapi::counter_metric_t db_get_network_and_member_and_summary;
prometheus::simpleapi::counter_metric_t db_get_member_list;
prometheus::simpleapi::counter_metric_t db_get_network_list;
prometheus::simpleapi::counter_metric_t db_member_change;
prometheus::simpleapi::counter_metric_t db_network_change;

} // namespace Metrics
} // namespace ZeroTier
