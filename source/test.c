//
// Created by 20123460 on 2022/1/6.
//
#include "test.h"
#include <unistd.h>
#include "stdio.h"
#include <sys/epoll.h>
#include "event/eloop.h"
#include "iowatcher.h"

int main(){

    printf("----------%d",epoll_create(1));
    return 0;
}