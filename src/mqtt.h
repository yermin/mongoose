#pragma once

#include "net.h"
#include "str.h"

#define MQTT_CMD_CONNECT 1
#define MQTT_CMD_CONNACK 2
#define MQTT_CMD_PUBLISH 3
#define MQTT_CMD_PUBACK 4
#define MQTT_CMD_PUBREC 5
#define MQTT_CMD_PUBREL 6
#define MQTT_CMD_PUBCOMP 7
#define MQTT_CMD_SUBSCRIBE 8
#define MQTT_CMD_SUBACK 9
#define MQTT_CMD_UNSUBSCRIBE 10
#define MQTT_CMD_UNSUBACK 11
#define MQTT_CMD_PINGREQ 12
#define MQTT_CMD_PINGRESP 13
#define MQTT_CMD_DISCONNECT 14

#define MQTT_QOS(qos) ((qos) << 1)
#define MQTT_GET_QOS(flags) (((flags) &0x6) >> 1)
#define MQTT_SET_QOS(flags, qos) (flags) = ((flags) & ~0x6) | ((qos) << 1)

struct mg_mqtt_opts {
  struct mg_str user;          // Username, can be empty
  struct mg_str pass;          // Password, can be empty
  struct mg_str client_id;     // Client ID
  struct mg_str will_topic;    // Will topic
  struct mg_str will_message;  // Will message
  uint8_t qos;                 // Quality of service
  bool will_retain;            // Retain last will
  bool clean;                  // Use clean session, 0 or 1
  uint16_t keepalive;          // Keep-alive timer in seconds
};

struct mg_mqtt_message {
  struct mg_str topic;  // Parsed topic
  struct mg_str data;   // Parsed message
  struct mg_str dgram;  // Whole MQTT datagram, including headers
  uint16_t id;  // Set for PUBACK, PUBREC, PUBREL, PUBCOMP, SUBACK, PUBLISH
  uint8_t cmd;  // MQTT command, one of MQTT_CMD_*
  uint8_t qos;  // Quality of service
  uint8_t ack;  // Connack return code. 0 - success
};

struct mg_connection *mg_mqtt_connect(struct mg_mgr *, const char *url,
                                      struct mg_mqtt_opts *opts,
                                      mg_event_handler_t fn, void *fn_data);
struct mg_connection *mg_mqtt_listen(struct mg_mgr *mgr, const char *url,
                                     mg_event_handler_t fn, void *fn_data);
void mg_mqtt_login(struct mg_connection *c, struct mg_mqtt_opts *opts);
void mg_mqtt_pub(struct mg_connection *c, struct mg_str *topic,
                 struct mg_str *data, int qos, bool retain);
void mg_mqtt_sub(struct mg_connection *, struct mg_str *topic, int qos);
int mg_mqtt_parse(const uint8_t *buf, size_t len, struct mg_mqtt_message *m);
void mg_mqtt_send_header(struct mg_connection *, uint8_t cmd, uint8_t flags,
                         uint32_t len);
size_t mg_mqtt_next_sub(struct mg_mqtt_message *msg, struct mg_str *topic,
                        uint8_t *qos, size_t pos);
size_t mg_mqtt_next_unsub(struct mg_mqtt_message *msg, struct mg_str *topic,
                          size_t pos);
void mg_mqtt_ping(struct mg_connection *);
void mg_mqtt_pong(struct mg_connection *);
void mg_mqtt_disconnect(struct mg_connection *);
