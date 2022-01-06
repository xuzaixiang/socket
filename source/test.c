//
// Created by xuzaixiang on 2022/1/6.
//

#include <unistd.h>
#include "stdio.h"
#include <sys/epoll.h>

int main(){

    printf("----------%d",epoll_create(1));
    return 0;
}