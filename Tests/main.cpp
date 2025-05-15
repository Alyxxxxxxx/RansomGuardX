#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

int main() {
    std::string filePath = "archivo.txt";
    
    while (true) {
        std::ofstream file(filePath, std::ios::app);  
        if (file.is_open()) {
            file << 'A';
            file.close();  
        } else {
            std::cerr << "No se pudo abrir el archivo" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
