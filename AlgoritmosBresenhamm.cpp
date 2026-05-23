#include "VariablesGlobales.h"
#include "AlgoritmoBresenhamm.h"

// Algoritmo de Bresenham para la línea
// Versión para líneas con pendiente suave (Horizontal: dx > dy)
void bresenhammHorizontal(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int P = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        paintCell(x, y, colorSeleccionado);
        if (P > 0) {
            y = y + yi;
            P = P + (2 * (dy - dx));
        }
        else {
            P = P + 2 * dy;
        }
    }
}

// Versión para líneas con pendiente pronunciada (Vertical: dy >= dx)
void bresenhammVertical(int x0, int y0, int x1, int y1) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int P = (2 * dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) {
        paintCell(x, y, colorSeleccionado);
        if (P > 0) {
            x = x + xi;
            P = P + (2 * (dx - dy));
        }
        else {
            P = P + 2 * dx;
        }
    }
}

// Función principal que decide cuál usar
void bresenhamLinea(int x0, int y0, int x1, int y1) {
    if (std::abs(y1 - y0) < std::abs(x1 - x0)) {
        if (x0 > x1)
            bresenhammHorizontal(x1, y1, x0, y0);
        else
            bresenhammHorizontal(x0, y0, x1, y1);
    }
    else {
        if (y0 > y1)
            bresenhammVertical(x1, y1, x0, y0);
        else
            bresenhammVertical(x0, y0, x1, y1);
    }
}
// Algoritmo de Bresenham para Círculos optimizado
void bresenhamCirculo(int cx, int cy, int r) {
    int x = 0;
    int y = r;           // Se comienza en el borde del radio
    int p = -r;          // Parámetro de decisión inicial

    while (x <= y) {

        paintCell(cx + x, cy + y, colorSeleccionado);
        paintCell(cx + y, cy + x, colorSeleccionado);
        paintCell(cx - y, cy + x, colorSeleccionado);
        paintCell(cx - x, cy + y, colorSeleccionado);
        paintCell(cx - x, cy - y, colorSeleccionado);
        paintCell(cx - y, cy - x, colorSeleccionado);
        paintCell(cx + y, cy - x, colorSeleccionado);
        paintCell(cx + x, cy - y, colorSeleccionado);

        // Parámetro de decisión y actualización de coordenadas
        if (p > 0) {
            y = y - 1; 
            p = p + 2 * (x - y) + 1;
        }
        else {
            p = p + 2 * x + 1;
        }
        x = x + 1;
    }
}