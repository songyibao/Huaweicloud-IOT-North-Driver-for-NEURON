//
// Created by root on 8/9/24.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "utils.h"
#include "../client/client.h"
int tag_ut_array_to_neu_json_read_resp_t(UT_array *tags, neu_json_read_resp_t *json)
{
    int index = 0;

    if (0 == utarray_len(tags)) {
        return 0;
    }

    json->n_tag = utarray_len(tags);
    json->tags  = (neu_json_read_resp_tag_t *) calloc(json->n_tag, sizeof(neu_json_read_resp_tag_t));
    if (NULL == json->tags) {
        return -1;
    }

    utarray_foreach(tags, neu_resp_tag_value_meta_t *, tag_value)
    {
        neu_tag_value_to_json(tag_value, &json->tags[index]);
        index += 1;
    }

    return 0;
}
char* transform_json_to_properties(const char* json_str) {
    // 解析传入的 JSON 串
    cJSON *root = cJSON_Parse(json_str);
    if (root == NULL) {
        printf("Error parsing JSON.\n");
        return NULL;
    }

    // 创建 properties 对象
    cJSON *properties = cJSON_CreateObject();

    // 获取原 JSON 中的 tags 数组
    cJSON *tags = cJSON_GetObjectItem(root, "tags");
    if (cJSON_IsArray(tags)) {
        cJSON *tag;
        cJSON_ArrayForEach(tag, tags) {
            cJSON *name = cJSON_GetObjectItem(tag, "name");
            cJSON *value = cJSON_GetObjectItem(tag, "value");

            if (cJSON_IsString(name) && cJSON_IsNumber(value)) {
                // 将值格式化为两位小数
                char formatted_value[50];
                sprintf(formatted_value, "%.2f", value->valuedouble);

                // 将格式化后的值添加到 properties 中
                cJSON_AddNumberToObject(properties, name->valuestring, atof(formatted_value));
            }
        }
    }

    // 转化 properties 结构为字符串
    char *result = cJSON_PrintUnformatted(properties);

    // 释放所有的 JSON 对象
    cJSON_Delete(root);
    cJSON_Delete(properties);

    return result;
}
int handle_trans_data(neu_plugin_t *plugin, neu_reqresp_head_t *head, neu_reqresp_trans_data_t *trans_data)
{
    int                  ret             = 0;
    char                *json_str        = NULL;
    char                *transformed_str = NULL;
    neu_json_read_resp_t resp            = { 0 };

    tag_ut_array_to_neu_json_read_resp_t(trans_data->tags, &resp);
    for (int i = 0; i < resp.n_tag; i++) {
        if (resp.tags[i].error != 0) {
            plog_error(plugin, "tag %s error: %ld", resp.tags[i].name, resp.tags[i].error);
            return -1;
        }
    }
    ret = neu_json_encode_by_fn(&resp, neu_json_encode_read_resp, &json_str);
    if(resp.tags!=NULL){
        free(resp.tags);
    }
    if (ret != 0) {
        plog_notice(plugin, "parse json failed");
        return -1;
    }
    // need free
    char *properties = transform_json_to_properties(json_str);
//    plog_debug(plugin, "parse json str succeed: %s", json_str);
    ret = message_send(plugin->service_id,properties);
    free(json_str);
    cJSON_free(properties);
    return ret;
}
// Function to start a process in a specified directory
pid_t start_process(const char *cmd, const char *dir) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (chdir(dir) != 0) {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }
        execlp(cmd, cmd, (char *)NULL);
        // If execlp returns, it must have failed.
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }

    // Parent process returns the PID of the child
    return pid;
}

// Function to monitor a process
int monitor_process(pid_t pid) {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        return -1;
    }

    if (WIFEXITED(status)) {
        printf("Process %d exited with status %d\n", pid, WEXITSTATUS(status));
        return WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        printf("Process %d killed by signal %d\n", pid, WTERMSIG(status));
        return -1;
    }

    return -1; // Should not reach here normally
}

// Function to stop a process
int stop_process(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("Process %d terminated\n", pid);
        return 0;
    } else {
        perror("Failed to terminate process");
        return -1;
    }
}