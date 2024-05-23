#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; // cтруктура для хранения информации о Windows Sockets API
    ADDRINFO hints; // cтруктура для указания критериев для функции getaddrinfo
    ADDRINFO* addrResult = nullptr; //  указатель на то, что получится в getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; // cокет для сервера
    char recvBuffer[512]; // для получения данных

    // 2 сообщения для отправки на сервер
    const char* sendBuffer1 = "Hello from client 1";
    const char* sendBuffer2 = "Hello from client 2";

    // инициализация библиотеки Winsock
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    // структура hints -  hints является структурой типа ADDRINFO, которая используется для указания параметров для поиска адресной информации
    ZeroMemory(&hints, sizeof(hints)); // очистка hints
    hints.ai_family = AF_INET; // использование IPv4
    hints.ai_socktype = SOCK_STREAM; // потоковый сокет
    hints.ai_protocol = IPPROTO_TCP; // протокол TCP

    // получение адреса и порта сервера
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // очистка ресурсов библиотеки WSA - она предоставляет функции и структуры для работы с сетевыми соединениями и взаимодействием по сети, поддерживает протоколы
        return 1;
    }

    // создание клиентского сокета
    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed, error: " << WSAGetLastError() << endl; // Получение последней ошибки Winsock
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // очистка ресурсов WSA
        return 1;
    }

    // подключение к серверу
    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server, error: " << WSAGetLastError() << endl; // Получение последней ошибки Winsock
        closesocket(ConnectSocket); // закрытие сокета
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // очистка ресурсов WSA
        return 1;
    }

    // отправка первого сообщения серверу
    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << WSAGetLastError() << endl; // Получение последней ошибки Winsock
        closesocket(ConnectSocket); // закрытие сокета
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // очистка ресурсов WSA
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // отправка второго сообщения серверу
    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << WSAGetLastError() << endl; // Получение последней ошибки Winsock
        closesocket(ConnectSocket); // закрытие сокета
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // Очистка ресурсов WSA
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    // завершение отправки данных
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << WSAGetLastError() << endl; // Получение последней ошибки Winsock
        closesocket(ConnectSocket); // закрытие сокета
        freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
        WSACleanup(); // очистка ресурсов WSA
        return 1;
    }

    // получение данных от сервера
    do {
        ZeroMemory(recvBuffer, 512); // очистка буфера для получения данных
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl; // получение последней ошибки Winsock
        }
    } while (result > 0);

    // закрытие сокета
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult); // очистка памяти, занятой функцией getaddrinfo
    WSACleanup(); // очистка ресурсов WSA
    return 0;
}
