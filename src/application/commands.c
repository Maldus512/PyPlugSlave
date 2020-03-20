#include <string.h>
#include <stdio.h>
#include <libopencm3/stm32/gpio.h>

#include "peripherals/load.h"
#include "peripherals/serial.h"
#include "model/model.h"

static void strip_newline(char *str);
static int  cmd_on(model_t *model);
static int  cmd_off(model_t *model);
static int  cmd_read(model_t *model);

static struct {
    char *cmd;
    int (*cb)(model_t *);
} cmds[] = {
    {"ON", cmd_on},
    {"OFF", cmd_off},
    {"READ", cmd_read},
};

int process_command(char *cmd, model_t *model) {
    strip_newline(cmd);

    for (unsigned int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); i++) {
        if (strcmp(cmd, cmds[i].cmd) == 0)
            return cmds[i].cb(model);
    }

    return -1;
}

static void strip_newline(char *str) {
    int len = strlen(str);
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
}

static int cmd_on(model_t *model) {
    (void)model;
    load_on();
    return 0;
}

static int cmd_off(model_t *model) {
    (void)model;
    load_off();
    return 0;
}

static int cmd_read(model_t *model) {
    char res[128];
    snprintf(res, 128, "%4f (%i)\n", model->current, model->calibration);
    serial_print(res);
    return 0;
}