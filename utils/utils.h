//
// Created by root on 8/9/24.
//

#ifndef PLUGIN_HUAWEI_IOT_UTILS_H
#define PLUGIN_HUAWEI_IOT_UTILS_H
#include "../hwiot_plugin.h"
int handle_trans_data(neu_plugin_t *plugin, neu_reqresp_head_t *head, neu_reqresp_trans_data_t *trans_data);
pid_t start_process(const char *cmd, const char *dir);

// Function to monitor a process
int monitor_process(pid_t pid);

// Function to stop a process
int stop_process(pid_t pid);
#endif //PLUGIN_HUAWEI_IOT_UTILS_H
