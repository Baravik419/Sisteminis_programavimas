#include <windows.h>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

using namespace std;

// 1 uzduotis - naudojamas FindFirstFile

vector<wstring> SearchForTXT(const wstring& catalogue) {
    vector<wstring> file;
    wstring path = catalogue + L"\\*.txt";

    WIN32_FIND_DATAW data;
    HANDLE search = FindFirstFileW(path.c_str(), &data);

    if (search == INVALID_HANDLE_VALUE) {
        wcerr << L"Unsuccessful file search" << catalogue << endl;
        return file;
    }

    do {
        if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            wstring fullPath = catalogue + L"\\" + data.cFileName;
            file.push_back(fullPath);
        }
    } while (FindNextFileW(search, &data));

    FindClose(search);
    return file;
}

// 2 uzduotis

queue<int> dataQueue;
CRITICAL_SECTION criticalSection;

DWORD WINAPI ProducerThread(LPVOID param) {
    vector<wstring>* file = static_cast<vector<wstring>*>(param);

    for (const auto& file : *file) {
        HANDLE hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            wcerr << L"Unsuccessful file opening: " << file << endl;
            continue;
        }

        LARGE_INTEGER size;
        GetFileSizeEx(hFile, &size);
        if (size.QuadPart == 0) {
            CloseHandle(hFile);
            continue;
        }

        unique_ptr<char[]> buffer(new char[size.QuadPart]);
        DWORD bytesRead;
        ReadFile(hFile, buffer.get(), size.LowPart, &bytesRead, NULL);
        CloseHandle(hFile);

        string turinys(buffer.get(), bytesRead);
        istringstream iss(turinys);
        int skaicius;

        while (iss >> skaicius) {
            EnterCriticalSection(&criticalSection);
            dataQueue.push(skaicius);
            LeaveCriticalSection(&criticalSection);
        }
    }

    return 0;
}

// 3 uzduotis

int minPrime = INT_MAX;
int maxPrime = INT_MIN;
bool producerDone = false;

CRITICAL_SECTION resultLock;

bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; ++i)
        if (n % i == 0)
            return false;
    return true;
}

DWORD WINAPI ConsumerThread(LPVOID) {
    while (true) {
        int number = -1;

        EnterCriticalSection(&criticalSection);
        if (!dataQueue.empty()) {
            number = dataQueue.front();
            dataQueue.pop();
        } else if (producerDone) {
            LeaveCriticalSection(&criticalSection);
            break;
        }
        LeaveCriticalSection(&criticalSection);

        if (number != -1 && isPrime(number)) {
            EnterCriticalSection(&resultLock);
            if (number < minPrime) minPrime = number;
            if (number > maxPrime) maxPrime = number;
            LeaveCriticalSection(&resultLock);
        }
    }

    return 0;
}



int main() {
    InitializeCriticalSection(&criticalSection);
    InitializeCriticalSection(&resultLock);

    wstring catalogue = L"C:\\Users\\erwin\\CLionProjects\\Sisteminis_programavimas\\3ND\\rand_files";
    vector<wstring> txtFiles = SearchForTXT(catalogue);

    for (size_t i = 0; i < txtFiles.size(); ++i) {
        wcout << L"[" << i + 1 << L"] " << txtFiles[i] << endl;
    }

    const int threadCount = 4;
    HANDLE consumers[threadCount];

    for (int i = 0; i < threadCount; ++i) {
        consumers[i] = CreateThread(NULL, 0, ConsumerThread, NULL, 0, NULL);
    }

    HANDLE hProducer = CreateThread(NULL, 0, ProducerThread, &txtFiles, 0, NULL);
    WaitForSingleObject(hProducer, INFINITE);
    producerDone = true;
    CloseHandle(hProducer);

    WaitForMultipleObjects(threadCount, consumers, TRUE, INFINITE);
    for (int i = 0; i < threadCount; ++i)
        CloseHandle(consumers[i]);

    cout << "\nMin Prime: " << minPrime << endl;
    cout << "Max Prime: " << maxPrime << endl;

    DeleteCriticalSection(&criticalSection);
    DeleteCriticalSection(&resultLock);

    return 0;
}
