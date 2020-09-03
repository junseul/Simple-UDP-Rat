#include <Windows.h>

int HwidHandler() {
  SYSTEM_INFO siSysInfo;
  GetSystemInfo(&siSysInfo);

  int Info_1 = siSysInfo.dwOemId;
  int Info_2 = siSysInfo.dwNumberOfProcessors;
  int Info_3 = siSysInfo.dwProcessorType;
  int Info_4 = siSysInfo.dwActiveProcessorMask;
  int Info_5 = siSysInfo.wProcessorLevel;
  int Info_6 = siSysInfo.wProcessorRevision;

  int HWID_Calculator[6] = {Info_1, Info_2, Info_3, Info_4, Info_5, Info_6};
  int HWID = HWID_Calculator[0, 1, 2, 3, 4, 5] * 2 * 4 * 8 * 16 * 32 * 64 * 120;
  return HWID;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
  WCHAR message[128];
  wsprintf(message, TEXT("Your HWID: %d"), HwidHandler());
  MessageBox(NULL, message, TEXT("Hello"), MB_OK);
}

