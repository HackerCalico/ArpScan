#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

struct ArpSendPara {
    char* srcIP;
    int ipElem1;
    int ipElem2;
    int ipElem3;
    int ipElem4;
};

void SetDelay(int* pDelay, int delay) {
    *pDelay = delay;
}

void SetDelayThreadNum(int* pDelayThreadNum, int delayThreadNum) {
    *pDelayThreadNum = delayThreadNum;
}

void SetPause(int* pPause, int pause) {
    *pPause = pause;
}

void SetClose(int* pClose, int close) {
    *pClose = close;
}

void ArpSend(ArpSendPara* pArpSendPara) {
    char destIP[16];
    if (sprintf_s(destIP, sizeof(destIP), "%d.%d.%d.%d", pArpSendPara->ipElem1, pArpSendPara->ipElem2, pArpSendPara->ipElem3, pArpSendPara->ipElem4) == -1) {
        free(pArpSendPara);
        return;
    }
    ULONG macAddr[2];
    ULONG physAddrLen = 6;
    in_addr srcAddr, destAddr;
    if (inet_pton(AF_INET, pArpSendPara->srcIP, &srcAddr) != 1 || inet_pton(AF_INET, destIP, &destAddr) != 1) {
        free(pArpSendPara);
        return;
    }
    printf("ArpScan IP: %s\n", destIP);
    if (SendARP(destAddr.S_un.S_addr, srcAddr.S_un.S_addr, macAddr, &physAddrLen) == NO_ERROR) {
        printf("Host alive: %s - MAC: %02X-%02X-%02X-%02X-%02X-%02X\n", destIP, macAddr[0] & 0xFF, (macAddr[0] >> 8) & 0xFF, (macAddr[0] >> 16) & 0xFF, (macAddr[0] >> 24) & 0xFF, macAddr[1] & 0xFF, (macAddr[1] >> 8) & 0xFF);
    }
    free(pArpSendPara);
}

void ArpScan(char* srcIP, int* pDelayThreadNum, int* pDelay, int* pPause, int* pClose) {
    int ipElems[4];
    if (sscanf_s(srcIP, "%d.%d.%d.%d", &ipElems[0], &ipElems[1], &ipElems[2], &ipElems[3]) == -1) {
        return;
    }
    // 打乱扫描顺序
    srand(time(NULL));
    int hostIdList[256];
    for (int i = 0; i < 256; i++) {
        hostIdList[i] = i;
    }
    for (int i = 0; i < 256; i++) {
        int temp;
        int index1 = rand() % 256;
        int index2 = rand() % 256;
        temp = hostIdList[index1];
        hostIdList[index1] = hostIdList[index2];
        hostIdList[index2] = temp;
    }
    // 扫描 C 段
    int threadNum = 0;
    for (int i = 0; i < 256; i++) {
        if (*pClose) {
            return;
        }
        while (*pPause) {
            Sleep(1000);
        }
        if (hostIdList[i] != ipElems[3]) {
            ArpSendPara* pArpSendPara = (ArpSendPara*)malloc(sizeof(ArpSendPara));
            *pArpSendPara = { srcIP, ipElems[0], ipElems[1], ipElems[2], hostIdList[i] };
            if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ArpSend, pArpSendPara, 0, NULL) == NULL) {
                i--;
                Sleep(1000);
                continue;
            }
            if (++threadNum >= *pDelayThreadNum) {
                threadNum = 0;
                Sleep(*pDelay);
            }
        }
    }
}

// https://github.com/HackerCalico/ArpScan
int main() {
    char srcIP[] = "192.168.111.1"; // 当前主机 IP
    int delay = 1000; // 扫描延迟
    int delayThreadNum = 50; // 延迟线程数
    int pause = 0; // 暂停扫描
    int close = 0; // 停止扫描
    ArpScan(srcIP, &delayThreadNum, &delay, &pause, &close); // 这种传递指针的形式便于无全局变量的 BOF 改造
    getchar(); // 等待线程结束
}