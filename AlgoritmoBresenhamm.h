#ifndef ALGORITMOS_HPP
#define ALGORITMOS_HPP

#include "VariablesGlobales.h"
#include "Lienzo.h"
#include <cmath>

void bresenhammHorizontal(int x0, int y0, int x1, int y1);
void bresenhammVertical(int x0, int y0, int x1, int y1);
void bresenhamLinea(int x0, int y0, int x1, int y1);
void bresenhamCirculo(int cx, int cy, int r);

#endif