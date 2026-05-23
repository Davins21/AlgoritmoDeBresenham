#include "VariablesGlobales.h"

std::vector<VertexSquare> gridVerts;
std::vector<GLuint> gridIndices;
unsigned int VAO, VBO, EBO;

int xi = 0, yi = 0, cxi = 0, cyi = 0;
bool primerClickLinea = true;
bool primerClickCirculo = true;
int tx0 = 0, ty0 = 0, tx1 = 0, ty1 = 0;
int clicTriangulo = 0;

GLfloat baseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat colorSeleccionado[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
Herramienta herramientaActual = LINEA;