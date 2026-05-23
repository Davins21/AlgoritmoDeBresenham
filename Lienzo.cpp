#include "VariablesGlobales.h"
#include "Lienzo.h"

void paintCell(int x, int y, GLfloat color[4]) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
    int base = (y * GRID_SIZE + x) * 4;
    for (int i = 0; i < 4; i++) {
        gridVerts[base + i].color[0] = color[0];
        gridVerts[base + i].color[1] = color[1];
        gridVerts[base + i].color[2] = color[2];
        gridVerts[base + i].color[3] = color[3];
    }
}

void ConfRejilla() {
    gridVerts.clear();
    gridIndices.clear();
    float cellW = 2.0f / GRID_SIZE;
    float cellH = 2.0f / GRID_SIZE;

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            float xPos = -1.0f + (x * cellW);
            float yPos = -1.0f + (y * cellH);

            gridVerts.push_back({ {xPos, yPos, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
            gridVerts.push_back({ {xPos + cellW, yPos, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
            gridVerts.push_back({ {xPos + cellW, yPos + cellH, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
            gridVerts.push_back({ {xPos, yPos + cellH, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });

            GLuint base = (y * GRID_SIZE + x) * 4;
            gridIndices.push_back(base + 0); gridIndices.push_back(base + 1); gridIndices.push_back(base + 2);
            gridIndices.push_back(base + 0); gridIndices.push_back(base + 2); gridIndices.push_back(base + 3);
        }
    }
}

void actualizarGPU() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gridVerts.size() * sizeof(VertexSquare), gridVerts.data());
}