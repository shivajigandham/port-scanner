#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <map>
#include <iomanip>
#include <atomic>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

// Color codes for Windows console (using ANSI escape codes)
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

std::mutex mtx;
std::atomic<int> completed_ports(0);
int total_ports = 0;

// Common services map
std::map<int, std::string> services = {
    {21, "FTP"}, {22, "SSH"}, {23, "Telnet"}, {25, "SMTP"},
    {53, "DNS"}, {80, "HTTP"}, {110, "POP3"}, {143, "IMAP"},
    {443, "HTTPS"}, {3306, "MySQL"}, {3389, "RDP"}, {5432, "PostgreSQL"},
    {8080, "HTTP-Proxy"}, {27017, "MongoDB"}
};

struct ScanResult {
    int port;
    bool open;
    std::string service;
    std::string banner;
};

std::vector<ScanResult> open_ports;

void banner_grab(SOCKET s, ScanResult& result) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    
    // Set a short timeout for banner grabbing
    DWORD timeout = 1000; // 1 second
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    int bytesReceived = recv(s, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        result.banner = std::string(buffer, bytesReceived);
        // Clean banner string (remove newlines/tabs)
        result.banner.erase(std::remove(result.banner.begin(), result.banner.end(), '\n'), result.banner.end());
        result.banner.erase(std::remove(result.banner.begin(), result.banner.end(), '\r'), result.banner.end());
    } else {
        result.banner = "No banner";
    }
}

void scan_port(const std::string& ip, int port, int timeout_ms) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    // Set non-blocking mode to implement custom timeout
    u_long mode = 1;
    ioctlsocket(s, FIONBIO, &mode);

    connect(s, (struct sockaddr*)&addr, sizeof(addr));

    fd_set set;
    FD_ZERO(&set);
    FD_SET(s, &set);
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    if (select(s + 1, NULL, &set, NULL, &tv) > 0) {
        ScanResult res;
        res.port = port;
        res.open = true;
        res.service = services.count(port) ? services[port] : "Unknown";
        
        // Try banner grab for common ports
        banner_grab(s, res);

        std::lock_guard<std::mutex> lock(mtx);
        open_ports.push_back(res);
    }

    closesocket(s);
    completed_ports++;
}

void print_progress(int current, int total) {
    float progress = (float)current / total;
    int barWidth = 50;

    std::cout << "\r" << WHITE << "[" << CYAN;
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << WHITE << "] " << int(progress * 100.0) << "% " << RESET << std::flush;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    std::string target;
    int start_port, end_port, thread_count;

    std::cout << CYAN << "========================================" << RESET << std::endl;
    std::cout << MAGENTA << "   Advanced Multi-Threaded Port Scanner " << RESET << std::endl;
    std::cout << CYAN << "========================================" << RESET << std::endl;

    std::cout << YELLOW << "Target IP: " << RESET;
    std::cin >> target;
    std::cout << YELLOW << "Start Port: " << RESET;
    std::cin >> start_port;
    std::cout << YELLOW << "End Port: " << RESET;
    std::cin >> end_port;
    std::cout << YELLOW << "Threads (e.g. 100): " << RESET;
    std::cin >> thread_count;

    total_ports = end_port - start_port + 1;
    std::vector<std::thread> workers;

    auto start_time = std::chrono::high_resolution_clock::now();

    std::cout << "\nScanning " << target << "..." << std::endl;

    for (int i = start_port; i <= end_port; ++i) {
        workers.emplace_back(scan_port, target, i, 1000);
        
        if (workers.size() >= thread_count || i == end_port) {
            for (auto& t : workers) {
                if (t.joinable()) t.join();
            }
            workers.clear();
            print_progress(completed_ports, total_ports);
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "\n\n" << GREEN << "Scan Results for " << target << ":" << RESET << std::endl;
    std::cout << std::left << std::setw(10) << "PORT" << std::setw(15) << "SERVICE" << "BANNER" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    for (const auto& res : open_ports) {
        std::cout << CYAN << std::left << std::setw(10) << res.port 
                  << YELLOW << std::setw(15) << res.service 
                  << WHITE << res.banner << RESET << std::endl;
    }

    if (open_ports.empty()) {
        std::cout << RED << "No open ports found." << RESET << std::endl;
    }

    std::cout << "\nScan completed in " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds." << std::endl;

    WSACleanup();
    return 0;
}
