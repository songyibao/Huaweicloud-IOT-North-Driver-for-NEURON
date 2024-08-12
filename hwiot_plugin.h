//
// Created by root on 8/9/24.
//

#ifndef HUAWEICLOUD_IOT_HWIOT_PLUGIN_H
#define HUAWEICLOUD_IOT_HWIOT_PLUGIN_H

#include "neuron.h"

struct neu_plugin {
    neu_plugin_common_t common;
    char *work_path;
    pid_t pid;

    // You can get the access address from IoT Console "Overview" -> "Access Information"
    char *host;
    char *port;

    // deviceId, The mqtt protocol requires the user name to be filled in.
    // Here we use deviceId as the username
    char *username;
    char *password;
    char *service_id;
    // for batch properties report
    char *sub_device_id;

    // for upload file, change to your file
    char *upload_file_path;
    uint8_t connect_failed_times;
    pthread_t thread; // 定义线程变量

    // 插件连接状态
    bool connected;
    bool started;
};

static neu_plugin_t *driver_open(void);

static int driver_close(neu_plugin_t *plugin);

static int driver_init(neu_plugin_t *plugin, bool load);

static int driver_uninit(neu_plugin_t *plugin);

static int driver_start(neu_plugin_t *plugin);

static int driver_stop(neu_plugin_t *plugin);

static int driver_config(neu_plugin_t *plugin, const char *config);

static int driver_request(neu_plugin_t *plugin, neu_reqresp_head_t *head, void *data);

static int driver_validate_tag(neu_plugin_t *plugin, neu_datatag_t *tag);

static int driver_group_timer(neu_plugin_t *plugin, neu_plugin_group_t *group);

static int driver_write(neu_plugin_t *plugin, void *req, neu_datatag_t *tag, neu_value_u value);

#endif //HUAWEICLOUD_IOT_HWIOT_PLUGIN_H
