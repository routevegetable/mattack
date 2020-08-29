#include <unistd.h>

#include "mattack.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/prctl.h>
#include <signal.h>
#include <assert.h>

void mattack_init(struct mattack_t *ma)
{
    mattack_maps_updated(ma);
}

void mattack_maps_updated(struct mattack_t *ma)
{
    if(ma->child_pid)
    {
        ma->cmd = ma_KILL;
        while(ma->cmd != ma_IDLE);
    }

    int ppid = getpid();
    int pid = fork();
    if(!pid)
    {
        assert(prctl(PR_SET_PDEATHSIG, SIGHUP) == 0);
        if(getppid() != ppid)
        {
            exit(0);
        }

        /* Child */
        while(1)
        {
            /* Wait for a command */
            while(ma->cmd == ma_IDLE) asm("pause");

            switch(ma->cmd)
            {

            case ma_READ:
                memcpy(ma->buff, ma->addr, ma->len);
                break;

            case ma_WRITE:
                memcpy(ma->addr, ma->buff, ma->len);
                break;

            case ma_KILL:
                ma->cmd = ma_IDLE;
                exit(0);
            }

            ma->cmd = ma_IDLE;
        }

        exit(0);
    }
    ma->child_pid = pid;
}

void mattack_read(struct mattack_t *ma, void *from, void *buff, size_t len)
{
    ma->addr = from;
    ma->len = len;
    ma->cmd = ma_READ;
    while(ma->cmd != ma_IDLE) asm("pause");
    memcpy(buff, ma->buff, len);
}

void mattack_write(struct mattack_t *ma, void *to, void *buff, size_t len)
{
    memcpy(ma->buff, buff, len);
    ma->addr = to;
    ma->len = len;
    ma->cmd = ma_WRITE;
    while(ma->cmd != ma_IDLE) asm("pause");
}
