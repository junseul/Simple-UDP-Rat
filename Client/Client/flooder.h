#pragma once
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <ctime>

class Flooder {
 public:
  Flooder(LPSTR target, DWORD port, DWORD interval, DWORD duration);
  ~Flooder();
  void Worker();
  std::thread WorkerThread();

 private:
  inline CHAR* malloc_lpstr(DWORD size) {
    return (CHAR*)malloc(size * sizeof(CHAR));
  }

  LPSTR target_ = malloc_lpstr(16);
  DWORD port_ = 0;
  DWORD interval_ = 0;
  DWORD duration_ = 0;

  WSADATA wsadata_;
  SOCKET flood_socket_ = NULL;
};