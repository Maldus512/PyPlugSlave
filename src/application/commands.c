#include <string.h>
#include <stdio.h>
#include <libopencm3/stm32/gpio.h>

#include "MCP9800/MCP9800.h"
#include "peripherals/i2c_peripherals.h"
#include "peripherals/load.h"
#include "peripherals/serial.h"
#include "model/model.h"

static void strip_newline(char *str);
static int  cmd_temp(char *resp, model_t *model);
static int  cmd_on(char *resp, model_t *model);
static int  cmd_off(char *resp, model_t *model);
static int  cmd_read(char *resp, model_t *model);

static struct {
    char *cmd;
    int (*cb)(char *, model_t *);
} cmds[] = {
    {"ON", cmd_on},
    {"OFF", cmd_off},
    {"READ", cmd_read},
    {"TEMP", cmd_temp},
};

int process_command(char *cmd, model_t *model, char *resp) {
    strip_newline(cmd);

    for (unsigned int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        if (strcmp(cmd, cmds[i].cmd) == 0)
            return cmds[i].cb(resp, model);
    }

    return -1;
}

static void strip_newline(char *str) {
    int len = strlen(str);
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
}

static int cmd_on(char *resp, model_t *model) {
    (void)model;
    load_on();
    resp[0] = '\0';
    return 0;
}

static int cmd_off(char *resp, model_t *model) {
    (void)model;
    load_off();
    resp[0] = '\0';
    return 0;
}

static int cmd_read(char *resp, model_t *model) {
    // sprintf(resp, "%4f (%i)", model->current, model->calibration);
    sprintf(resp, "%4f", model->current);
    return 0;
}

static int cmd_temp(char *resp, model_t *model) {
    (void)model;
    i2c_driver_t tempdr = {0x90, i2c_custom_transfer, NULL};
    sprintf(resp, "%4f", MCP9800_read_temperature(tempdr));
    return 0;
}