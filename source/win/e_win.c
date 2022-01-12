
#include "event/win/e_win.h"

int socket_errno(){
  return WSAGetLastError();
}