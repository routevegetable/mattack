#pragma once
#include <sys/types.h>

#define MATTACK_XFER_MAX 32

enum mattack_command_t
{
    ma_IDLE,
    ma_READ,
    ma_WRITE,
    ma_KILL
};

struct mattack_t
{
    int child_pid;
    enum mattack_command_t cmd;
    void *addr;
    char buff[MATTACK_XFER_MAX];
    int len;
};

void mattack_init(struct mattack_t *ma);

void mattack_maps_updated(struct mattack_t *ma);

void mattack_read(struct mattack_t *ma, void *from, void *buff, size_t len);

void mattack_write(struct mattack_t *ma, void *to, void *buff, size_t len);
