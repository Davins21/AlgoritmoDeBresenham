#include "VariablesGlobales.h"
#include "EventoMouse.h"
#include "Lienzo.h"
#include "AlgoritmoBresenhamm.h"
#include "imgui.h"
#include <cmath>

void eventoMouse(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (ypos <= PANEL_ALTO_PX) return;

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        int x = (int)(xpos / (w / (float)GRID_SIZE));
        int y = (int)((h - ypos) / (h / (float)GRID_SIZE));

        if (herramientaActual == LINEA) {
            if (primerClickLinea) { xi = x; yi = y; primerClickLinea = false; }
            else { bresenhamLinea(xi, yi, x, y); actualizarGPU(); primerClickLinea = true; }
        }
        else if (herramientaActual == CIRCULO) {
            if (primerClickCirculo) { cxi = x; cyi = y; primerClickCirculo = false; }
            else {
                int r = (int)std::sqrt(std::pow(x - cxi, 2) + std::pow(y - cyi, 2));
                bresenhamCirculo(cxi, cyi, r);
                actualizarGPU();
                primerClickCirculo = true;
            }
        }
        else if (herramientaActual == TRIANGULO) {
            if (clicTriangulo == 0) { tx0 = x; ty0 = y; clicTriangulo = 1; }
            else if (clicTriangulo == 1) {
                tx1 = x; ty1 = y;
                bresenhamLinea(tx0, ty0, tx1, ty1);
                actualizarGPU();
                clicTriangulo = 2;
            }
            else if (clicTriangulo == 2) {
                bresenhamLinea(tx1, ty1, x, y);
                bresenhamLinea(x, y, tx0, ty0);
                actualizarGPU();
                clicTriangulo = 0;
            }
        }
    }
}