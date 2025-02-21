//
// Created by root on 8/9/24.
//
//
// Created by songyibao on 24-2-29.
//

#include "hwiot_plugin.h"
#include "neuron.h"
#include <pthread.h>
#include <stdlib.h>
#include "utils/utils.h"
#include "client/client.h"
#include "iota_init.h"
#include "iota_login.h"
static const neu_plugin_intf_funs_t plugin_intf_funs = {
        .open    = driver_open,
        .close   = driver_close,
        .init    = driver_init,
        .uninit  = driver_uninit,
        .start   = driver_start,
        .stop    = driver_stop,
        .setting = driver_config,
        .request = driver_request,

        .driver.validate_tag = driver_validate_tag,
        .driver.group_timer  = driver_group_timer,
        .driver.write_tag    = driver_write,
};

const neu_plugin_module_t neu_plugin_module = {
        .version         = NEURON_PLUGIN_VER_1_0,
        .schema          = "hw-iot",
        .module_name     = "Huaweicloud IOT",
        .module_descr    = "Huaweicloud IOT Plugin",
        .module_descr_zh = "华为云 IOT 插件",
        .intf_funs       = &plugin_intf_funs,
        .kind            = NEU_PLUGIN_KIND_SYSTEM,
        .type            = NEU_NA_TYPE_APP,
        .display         = true,
        .single          = false,
};

static neu_plugin_t *driver_open(void)
{

    neu_plugin_t *plugin = calloc(1, sizeof(neu_plugin_t));

    neu_plugin_common_init(&plugin->common);
    plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;

    return plugin;
}

static int driver_close(neu_plugin_t *plugin)
{
    free(plugin);

    return 0;
}
// driver_init -> driver_config -> driver_start
static int driver_init(neu_plugin_t *plugin, bool load)
{
    (void) load;
    plog_notice(plugin,
                "============================================================"
                "\ninitialize "
                "plugin============================================================\n");

    return 0;
}

static int driver_config(neu_plugin_t *plugin, const char *setting)
{
    plog_notice(plugin,
                "============================================================\nconfig "
                "plugin============================================================\n");
    int   ret       = 0;
    char *err_param = NULL;

    neu_json_elem_t host            = { .name = "host", .t = NEU_JSON_STR };
    neu_json_elem_t port            = { .name = "port", .t = NEU_JSON_STR };
    neu_json_elem_t service_id        = { .name = "service_id", .t = NEU_JSON_STR };
    neu_json_elem_t username          = { .name = "username", .t = NEU_JSON_STR };
    neu_json_elem_t password       = { .name = "password", .t = NEU_JSON_STR };
    ret = neu_parse_param(setting, &err_param, 5, &host, &port, &service_id, &username, &password);
    if (ret < 0) {
        plog_error(plugin, "parse setting failed: %s", err_param);
        goto error;
    }
    // host, required
    if (0 == strlen(host.v.val_str)) {
        plog_error(plugin, "setting invalid host: `%s`", host.v.val_str);
        goto error;
    }

    // port, required
    long tport = strtol(port.v.val_str,NULL,10);
    if (1 > tport || tport > 65535) {
        plog_error(plugin, "setting invalid port: %" PRIi64, tport);
        goto error;
    }

    plugin->host = host.v.val_str;
    plugin->port = port.v.val_str;
    plugin->username        = username.v.val_str;
    plugin->password          = password.v.val_str;
    plugin->service_id       = service_id.v.val_str;
    plog_notice(plugin, "config server_ip         : %s", plugin->host);
    plog_notice(plugin, "config port              : %s", plugin->port);
    plog_notice(plugin, "config username          : %s", plugin->username);
    plog_notice(plugin, "config password          : %s", plugin->password);
    plog_notice(plugin, "config service_id        : %s", plugin->service_id);

    return 0;

error:
    return -1;
}

static int driver_start(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nstart "
                "plugin============================================================\n");

    plugin->started          = true;
    plugin->common.link_state = NEU_NODE_LINK_STATE_CONNECTED;
//    plugin->pid = start_process("/opt/huaweicloud-iot-device-sdk-c-1.2.0/MQTT_Demo","/opt/huaweicloud-iot-device-sdk-c-1.2.0/");
    // 创建线程
    plog_info(plugin,"创建华为 iot client 线程");
    client_init(plugin);
    return 0;
}

static int driver_stop(neu_plugin_t *plugin)
{
    plugin->started = false;
    plugin->common.link_state = NEU_NODE_LINK_STATE_DISCONNECTED;
    plog_notice(plugin,
                "============================================================\nstop "
                "plugin============================================================\n");

//    stop_process(plugin->pid);
    client_uinit(plugin);
    return 0;
}

static int driver_uninit(neu_plugin_t *plugin)
{
    plog_notice(plugin,
                "============================================================\nuninit "
                "plugin============================================================\n");
    nlog_debug("uninit success");
    return NEU_ERR_SUCCESS;
}

static int driver_request(neu_plugin_t *plugin, neu_reqresp_head_t *head, void *data)
{
    plog_debug(plugin,
                "============================================================request "
                "plugin============================================================\n");
    if (plugin->started == false) {
        return 0;
    }
    int res=0;
    if(data==NULL){
        plog_error(plugin,"data is null");
        return -1;
    }
    switch (head->type) {
        case NEU_REQRESP_TRANS_DATA:
            neu_reqresp_trans_data_t *trans_data = data;
            plog_debug(plugin,"开始数据上报");

            res=handle_trans_data(plugin, head, trans_data);
            if(res!=0){
                plog_error(plugin,"new 数据上报失败");
            }else{
                plog_notice(plugin,"数据上报成功");
            }
            break;
        default:
            break;
    }
    return 0;
}

static int driver_validate_tag(neu_plugin_t *plugin, neu_datatag_t *tag)
{
    plog_notice(plugin, "validate tag: %s", tag->name);

    return 0;
}

static int driver_group_timer(neu_plugin_t *plugin, neu_plugin_group_t *group)
{
    (void) plugin;
    (void) group;

    return 0;
}

static int driver_write(neu_plugin_t *plugin, void *req, neu_datatag_t *tag, neu_value_u value)
{
    (void) plugin;
    (void) req;
    (void) tag;
    (void) value;

    return 0;
}