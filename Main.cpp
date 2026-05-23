#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "VariablesGlobales.h"
#include "Lienzo.h"
#include "EventoMouse.h"
#include "Shaders.h"
#include "Buffers.h"

int main()
{
    glfwInit();

    // OpenGL 3.3 para máxima compatibilidad con ImGui Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Algoritmo de Bresenham", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, eventoMouse);

    gladLoadGL();

    // Inicialización de ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Ajuste estético de bordes redondeados para la interfaz
    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    ConfRejilla(); // Crea la geometría inicial

    // Compilación de Shaders
    unsigned int prog = compilarShaders();

    // Configuración de Buffers (VAO, VBO, EBO)
    inicializarBuffersGPU();

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Ocupar ancho d ela ventana
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)width, (float)PANEL_ALTO_PX));

        //Mantener Interfaz Estática
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Panel de Control");
        ImGui::SetWindowSize(ImVec2(300, 180), ImGuiCond_FirstUseEver);

        // Botón Línea
        bool PopLinea = (herramientaActual == LINEA);
        if (PopLinea) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.45f, 0.9f, 1.0f));
        if (ImGui::Button("Linea", ImVec2(110, 26))) {
            herramientaActual = LINEA;
            primerClickCirculo = true;
        }
        if (PopLinea) ImGui::PopStyleColor();

        ImGui::SameLine();

        // Botón Círculo
        bool PopCirculo = (herramientaActual == CIRCULO);
        if (PopCirculo) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9f, 0.45f, 0.0f, 1.0f));
        if (ImGui::Button("Circulo", ImVec2(110, 26))) {
            herramientaActual = CIRCULO;
            primerClickLinea = true;
        }
        if (PopCirculo) ImGui::PopStyleColor();

        ImGui::SameLine();

        bool PopTriangulo = (herramientaActual == TRIANGULO);
        if (PopTriangulo) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.65f, 0.3f, 1.0f)); // Color verde
        if (ImGui::Button("Triangulo", ImVec2(110, 26))) {
            herramientaActual = TRIANGULO;
            primerClickLinea = true;
            primerClickCirculo = true;
            clicTriangulo = 0;
        }
        if (PopTriangulo) ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::TextDisabled("|"); 
        ImGui::SameLine();

        // Selector de color compacto
        ImGui::Text("COLOR:"); ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::ColorEdit4("##ColorPicker", colorSeleccionado, ImGuiColorEditFlags_NoInputs);

        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        // Botón Limpiar
        if (ImGui::Button("Limpiar Lienzo", ImVec2(120, 26))) {
            ConfRejilla();
            actualizarGPU();
            primerClickLinea = true;
            primerClickCirculo = true;
            clicTriangulo = 0;
        }

        ImGui::End();

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)gridIndices.size(), GL_UNSIGNED_INT, 0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}