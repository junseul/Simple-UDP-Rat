#include "handler.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
  LPSTR server_address = "127.0.0.1";
  DWORD port = 4433;
  uint8_t key[32] = {43, 92,  182, 48, 39,  5,  84, 17, 38,  58, 82,
                     37, 183, 2,   9,  85,  74, 53, 92, 106, 94, 28,
                     1,  94,  58,  92, 215, 43, 11, 94, 10,  9};

  HANDLE mutex = CreateMutex(FALSE, 0, server_address);
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    exit(0);
  }

  Communicator communicator(server_address, port, key);
  communicator.Handler();
  CloseHandle(mutex);
}
