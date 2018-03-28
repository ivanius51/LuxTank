#ifndef TARGET_VER_H
#define TARGET_VER_H
// Включение SDKDDKVer.h обеспечивает определение самой последней доступной платформы Windows.

// Если требуется выполнить сборку приложения для предыдущей версии Windows, включите WinSDKVer.h и
// задайте для макроса _WIN32_WINNT значение поддерживаемой платформы перед включением SDKDDKVer.h.

#define _WIN32_WINNT 0x0500

//#include <SDKDDKVer.h>
#endif //TARGET_VER_H