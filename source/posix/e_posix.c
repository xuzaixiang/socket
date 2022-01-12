
#include "event/posix/e_posix.h"
#include <errno.h>

int socket_errno(){
  return errno;
}