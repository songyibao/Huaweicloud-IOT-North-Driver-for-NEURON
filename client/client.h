//
// Created by root on 8/9/24.
//

#ifndef HUAWEICLOUD_IOT_CLIENT_H
#define HUAWEICLOUD_IOT_CLIENT_H
#include "../hwiot_plugin.h"
#include "iota_init.h"
#include "iota_login.h"
#include "iota_datatrans.h"
#include "iota_defaultCallback.h"
#include "string_util.h"
#include "log_util.h"
#include "iota_cfg.h"
#include "subscribe.h"


void TimeSleep(int ms);

static void MyPrintLog(int level, char *format, va_list args);

// Message downstream sending callback
static void HandleMessageDown(EN_IOTA_MESSAGE *rsp, void *mqttv5);


// Connection failure callback
static void HandleConnectFailure(EN_IOTA_MQTT_PROTOCOL_RSP *rsp);

static void SubscribeDeviceTpoic(void);

static void HandleConnectSuccess(EN_IOTA_MQTT_PROTOCOL_RSP *rsp);

// ---------------------------- secret authentication --------------------------------------
static void mqttDeviceSecretInit(char *address, char *port, char *deviceId, char *password);

// ---------------------------- Certificate authentication --------------------------------------
void mqttDeviceCertInit(char *address, char *port, char *deviceId, char *deviceKyePassword);
int client_init(neu_plugin_t *plugin);
int client_uinit(neu_plugin_t *plugin);
void message_send(char *service_id,char *properties);
#endif //HUAWEICLOUD_IOT_CLIENT_H
