// g++ -std=c++17 Detection/*.cpp Strategies/*.cpp Utilities/*.cpp imgui/*.cpp main.cpp -Wall -O3 -o main -I C:/Users/Alyso/vcpkg/installed/x64-mingw-static/include -L C:/Users/Alyso/vcpkg/installed/x64-mingw-static/lib -lSDL2 -lopengl32 -lsetupapi -lole32 -lgdi32 -limm32 -lversion -lwinmm -loleaut32 -luuid -mwindows

#define SDL_MAIN_HANDLED
#include "Detection/RansomGuardX.h"
#include "Strategies/FileExtensionDetectionStrategy.h"
#include "Strategies/FileModificationDetectionStrategy.h"
#include "Strategies/EntropyDetectionStrategy.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>

class LogManager {
public:
    static std::vector<std::string> logs;
    static void loadLogs() {
        logs.clear();
        std::ifstream logFile("ransomware_log.txt");
        std::string line;
        while (std::getline(logFile, line)) {
            logs.push_back(line);
        }
        logFile.close();
    }
    static void addLog(const std::string& log) {
        logs.push_back(log);
        if (logs.size() > 100) logs.erase(logs.begin());
    }
};

std::vector<std::string> LogManager::logs;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error inicializando SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_Window* window = SDL_CreateWindow("RansomGuardX - Control Panel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Error creando ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    if (io.Fonts->Fonts.Size == 0) io.Fonts->AddFontDefault();
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(8, 5);
    style.FrameRounding = 6.0f;
    style.WindowRounding = 12.0f;
    style.ScrollbarRounding = 10.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.05f, 0.98f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.15f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.20f, 0.30f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.60f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.45f, 0.75f, 1.0f);

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    std::string monitoredPath = "";
    char pathBuffer[256] = "";
    char extensionFileBuffer[256] = "Resources/suspicious_extensions.txt";
    int modCountThreshold = 10;
    int modTimeThreshold = 10;
    float entropyThreshold = 7.0f;
    const int entropyFileSize = 10 * 1024 * 1024;

    RansomGuardX protector(monitoredPath);
    auto extensionScanner = std::make_shared<FileExtensionDetectionStrategy>(extensionFileBuffer);
    auto fileScanner = std::make_shared<FileModificationDetectionStrategy>(modCountThreshold, modTimeThreshold);
    auto entropyScanner = std::make_shared<EntropyDetectionStrategy>(entropyThreshold, entropyFileSize);
    protector.addStrategy(extensionScanner);
    protector.addStrategy(fileScanner);
    protector.addStrategy(entropyScanner);

    bool isScanning = false;
    bool running = true;
    std::thread scanThread;

    LogManager::loadLogs();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Ocupa toda la ventana
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGui::Begin("RansomGuardX - Panel de Control", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse);

        ImGui::Text("Directorio Monitoreado:");
        ImGui::InputText("##Directorio", pathBuffer, sizeof(pathBuffer));
        if (ImGui::Button("Aplicar Directorio", ImVec2(-1, 0))) {
            monitoredPath = pathBuffer;
            protector = RansomGuardX(monitoredPath);
            protector.addStrategy(extensionScanner);
            protector.addStrategy(fileScanner);
            protector.addStrategy(entropyScanner);
            protector.hasFinishedScan = false;
            LogManager::addLog("Directorio actualizado: " + monitoredPath);
        }

        ImGui::Separator();
        ImGui::Text("Configuración de Detección:");
        ImGui::InputText("Extensiones Sospechosas", extensionFileBuffer, sizeof(extensionFileBuffer));
        ImGui::SliderInt("Umbral de Archivos Modificados", &modCountThreshold, 1, 50);
        ImGui::SliderInt("Ventana de Tiempo (s)", &modTimeThreshold, 1, 60);
        ImGui::SliderFloat("Umbral de Entropía", &entropyThreshold, 0.0f, 8.0f);

        if (ImGui::Button("Aplicar Configuración", ImVec2(-1, 0))) {
            std::map<std::string, std::string> params {
                {"extensionsFilePath", extensionFileBuffer},
                {"threshold", std::to_string(modCountThreshold)},
                {"interval", std::to_string(modTimeThreshold)},
                {"initialEntropyThreshold", std::to_string(entropyThreshold)}
            };

            if (!extensionScanner) {
                extensionScanner = std::make_shared<FileExtensionDetectionStrategy>(extensionFileBuffer);
            } else {
                extensionScanner->resetDetection(params);
            }

            if (!fileScanner) {
                fileScanner = std::make_shared<FileModificationDetectionStrategy>(modCountThreshold, modTimeThreshold);
            } else {
                fileScanner->resetDetection(params);
            }

            if (!entropyScanner) {
                entropyScanner = std::make_shared<EntropyDetectionStrategy>(entropyThreshold, entropyFileSize);
            } else {
                entropyScanner->resetDetection(params);
            }

            protector = RansomGuardX(monitoredPath);
            protector.addStrategy(extensionScanner);
            protector.addStrategy(fileScanner);
            protector.addStrategy(entropyScanner);
            protector.hasFinishedScan = false;

            LogManager::addLog("Parámetros actualizados");
        }

        ImGui::Separator();

        ImGui::Text("Estado: ");
        ImGui::SameLine();
        ImGui::TextColored(isScanning ? ImVec4(0.9f, 0.7f, 0.2f, 1.0f) : (protector.hasFinishedScan ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.8f, 0.2f, 0.2f, 1.0f)),
            isScanning ? "Escaneando..." : (protector.hasFinishedScan ? "Escaneo Finalizado" : "Detenido"));

        if (ImGui::Button(isScanning ? "Detener Escaneo" : "Iniciar Escaneo", ImVec2(-1, 0))) {
            isScanning = !isScanning;
            if (isScanning) {
                protector.hasFinishedScan = false;
                scanThread = std::thread([&]() {
                    while (isScanning) {
                        protector.runAllScans();
                        LogManager::loadLogs();
                        SDL_Delay(1000);
                        if (protector.hasFinishedScan) {
                            LogManager::addLog("Escaneo finalizado");
                            isScanning = false;
                            break;
                        }
                    }
                });
                scanThread.detach();
            }
            LogManager::addLog(isScanning ? "Escaneo iniciado" : "Escaneo detenido");
        }

        ImGui::Separator();

        ImGui::Text("Registro de Eventos:");
        ImGui::BeginChild("Logs", ImVec2(0, io.DisplaySize.y * 0.33f), true, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& log : LogManager::logs) {
            ImGui::TextWrapped("%s", log.c_str());
        }
        if (!LogManager::logs.empty()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::End();


        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.04f, 0.04f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
