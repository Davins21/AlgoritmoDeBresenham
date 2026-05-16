#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<cmath>

// Vertex Shader. Recibe posición (Vec3) y color (vec4)
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec4 aColor;\n" // Recibe el color del VBO
"out vec4 ourColor;\n"
"void main() {\n"
"   gl_Position = vec4(aPos, 1.0);\n"    // Mantiene la posición en coordenadas normalizadas
"   ourColor = aColor;\n"                // Pasa el color al Fragment Shader
"}\0";

// Fragment Shader. Determina el color de cada pixel en pantalla
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec4 ourColor;\n" // Recibe el color del Vertex Shader
"void main() { FragColor = ourColor; }\0";

// Estructura para organizar los datos de cada vértice (Posición + Color)
struct VertexSquare {
	GLfloat pos[3];
	GLfloat color[4];
};

// Configuración de la rejilla
const int GRID_SIZE = 150;				// Número de celdas por lado
std::vector<VertexSquare> gridVerts;	// Almacena los vértices en CPU
std::vector<GLuint> gridIndices;		// Almacena el orden de dibujo (índices)
unsigned int VAO, VBO, EBO;				// Objetos de la GPU

// Estado del dibujo
int xi, yi, cxi, cyi;					// Coordenadas de inicio para líneas y círculos
bool primerClickLinea = true;			// Control para el estado de la línea
bool primerClickCirculo = true;			// Control para el estado del círculo

// Colores
GLfloat baseColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Fondo de celdas
GLfloat colorCyan[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
GLfloat colorNaranja[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
GLfloat colorRojo[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat colorVerde[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat colorGris[4] = { 0.6f, 0.6f, 0.6f, 1.0f }; // Para los botones
GLfloat colorNegro[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

enum Herramienta { LINEA, CIRCULO };
Herramienta herramientaActual = LINEA; // Por defecto empieza en Línea
// Color dinámico seleccionado por el usuario (Inicia en Cyan)
GLfloat colorSeleccionado[4] = { 0.0f, 1.0f, 1.0f, 1.0f };


// Límites de la zona de la interfaz (Filas de arriba)
const int UI_ROWS = 6;

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

// Dibuja los botones fijos en la parte superior
void drawInterface() {
	// 1. Pintar fondo de la barra de herramientas (Gris oscuro/Gris)
	for (int y = GRID_SIZE - UI_ROWS; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			paintCell(x, y, colorGris);
		}
	}

	// 2. Botón LÍNEA (Celdas x: 5 a 25) - Color indicador Negro/Gris
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) {
		for (int x = 5; x <= 25; x++) {
			paintCell(x, y, (herramientaActual == LINEA) ? colorCyan : colorNegro);
		}
	}

	// 3. Botón CÍRCULO (Celdas x: 30 a 50)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) {
		for (int x = 30; x <= 50; x++) {
			paintCell(x, y, (herramientaActual == CIRCULO) ? colorNaranja : colorNegro);
		}
	}
	// DIBUJO DE TEXTO PIXELADO (Letras 'L' y 'C')
	// ========================================================
	// El color del texto será blanco para que contraste bien sobre el botón activo o inactivo
	GLfloat colorTexto[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	int topY = GRID_SIZE - 1;

	// Letra 'L' dentro del botón de Línea (centrado alrededor de x = 15)
	// Línea vertical de la L
	paintCell(14, topY - 1, colorTexto);
	paintCell(14, topY - 2, colorTexto);
	paintCell(14, topY - 3, colorTexto);
	// Base de la L
	paintCell(14, topY - 4, colorTexto);
	paintCell(15, topY - 4, colorTexto);
	paintCell(16, topY - 4, colorTexto);

	// Letra 'C' dentro del botón de Círculo (centrado alrededor de x = 40)
	// Techo y suelo de la C
	paintCell(39, topY - 1, colorTexto); paintCell(40, topY - 1, colorTexto); paintCell(41, topY - 1, colorTexto);
	paintCell(39, topY - 4, colorTexto); paintCell(40, topY - 4, colorTexto); paintCell(41, topY - 4, colorTexto);
	// Pared izquierda de la C
	paintCell(39, topY - 2, colorTexto);
	paintCell(39, topY - 3, colorTexto);

	// 4. Paleta de Colores (Botones pequeños al lado derecho)
	// Cyan (x: 70 a 80)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) { for (int x = 70; x <= 80; x++) paintCell(x, y, colorCyan); }
	// Naranja (x: 85 a 95)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) { for (int x = 85; x <= 95; x++) paintCell(x, y, colorNaranja); }
	// Rojo (x: 100 a 110)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) { for (int x = 100; x <= 110; x++) paintCell(x, y, colorRojo); }
	// Verde (x: 115 a 125)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) { for (int x = 115; x <= 125; x++) paintCell(x, y, colorVerde); }

	// Indicador del color actual seleccionado (Muestra el color activo en la esquina derecha x: 135 a 145)
	for (int y = GRID_SIZE - 4; y < GRID_SIZE - 1; y++) { for (int x = 135; x <= 145; x++) paintCell(x, y, colorSeleccionado); }
}

// Función para inicializar la rejilla 
void setupGrid() {
	gridVerts.clear();
	gridIndices.clear();
	float cellW = 2.0f / GRID_SIZE; // Ancho de cada celda
	float cellH = 2.0f / GRID_SIZE; // Alto de cada celda
	int mitad = GRID_SIZE / 2;

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			//Calcular esquina inferior izquierda de la celda
			float xPos = -1.0f + (x * cellW);
			float yPos = -1.0f + (y * cellH);

			// Crear 4 vértices por cuadrado (celda)
			gridVerts.push_back({ {xPos, yPos, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
			gridVerts.push_back({ {xPos + cellW, yPos, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
			gridVerts.push_back({ {xPos + cellW, yPos + cellH, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });
			gridVerts.push_back({ {xPos, yPos + cellH, 0.0f}, {baseColor[0], baseColor[1], baseColor[2], baseColor[3]} });

			// Definir indices para formar dos tríangulos por celda
			GLuint base = (y * GRID_SIZE + x) * 4;
			gridIndices.push_back(base + 0); gridIndices.push_back(base + 1); gridIndices.push_back(base + 2);
			gridIndices.push_back(base + 0); gridIndices.push_back(base + 2); gridIndices.push_back(base + 3);
		}
	}
}

// Algoritmo de Bresenham para la línea
// Versión para líneas con pendiente suave (Horizontal: dx > dy)
void drawLineLow(int x0, int y0, int x1, int y1) {
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
		if (y < GRID_SIZE - UI_ROWS) paintCell(x, y, colorSeleccionado); // No pintar sobre la UI
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
void drawLineHigh(int x0, int y0, int x1, int y1) {
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
		if (y < GRID_SIZE - UI_ROWS) paintCell(x, y, colorSeleccionado); // No pintar sobre la UI
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
			drawLineLow(x1, y1, x0, y0);
		else
			drawLineLow(x0, y0, x1, y1);
	}
	else {
		if (y0 > y1)
			drawLineHigh(x1, y1, x0, y0);
		else
			drawLineHigh(x0, y0, x1, y1);
	}
}

// Algoritmo de Bresenham para Círculos 
void bresenhamCirculo(int cx, int cy, int r) {
	int x = 0;
	int y = r;           // Se comienza en el borde del radio
	int p = -r;          // Parámetro de decisión inicial

	// 2. Bucle principal para un octante (X hasta la diagonal)
	while (x <= y) {

		if (cy + y < GRID_SIZE - UI_ROWS) paintCell(cx + x, cy + y, colorSeleccionado);
		if (cy + x < GRID_SIZE - UI_ROWS) paintCell(cx + y, cy + x, colorSeleccionado);
		if (cy + x < GRID_SIZE - UI_ROWS) paintCell(cx - y, cy + x, colorSeleccionado);
		if (cy + y < GRID_SIZE - UI_ROWS) paintCell(cx - x, cy + y, colorSeleccionado);
		if (cy - y < GRID_SIZE - UI_ROWS) paintCell(cx - x, cy - y, colorSeleccionado);
		if (cy - x < GRID_SIZE - UI_ROWS) paintCell(cx - y, cy - x, colorSeleccionado);
		if (cy - x < GRID_SIZE - UI_ROWS) paintCell(cx + y, cy - x, colorSeleccionado);
		if (cy - y < GRID_SIZE - UI_ROWS) paintCell(cx + x, cy - y, colorSeleccionado);

		// 4. Parámetro de decisión y actualización de coordenadas
		if (p > 0) {
			y = y - 1; // Se reduce y para mantenerse cerca de la curva
			p = p + 2 * (x - y) + 1;
		}
		else {
			p = p + 2 * x + 1;
		}

		// X se incrementa en cada iteración
		x = x + 1;
	}
}

//Actualiza los datos de los vértices (colores) en la GPU
void actualizarGPU() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gridVerts.size() * sizeof(VertexSquare), gridVerts.data());
}

// Procesa los eventos del mouse en la pantalla
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		int x = (int)(xpos / (w / (float)GRID_SIZE));
		int y = (int)((h - ypos) / (h / (float)GRID_SIZE));

		// ========================================================
		// DETECCIÓN DE CLICS EN LOS BOTONES (ZONA SUPERIOR)
		// ========================================================
		if (y >= GRID_SIZE - UI_ROWS) {
			// Click en Botón LÍNEA (Celdas x 5 a 25)
			if (x >= 5 && x <= 25) {
				herramientaActual = LINEA;
				std::cout << "Herramienta: Linea" << std::endl;
			}
			// Click en Botón CÍRCULO (Celdas x 30 a 50)
			else if (x >= 30 && x <= 50) {
				herramientaActual = CIRCULO;
				std::cout << "Herramienta: Circulo" << std::endl;
			}
			// Click en Paleta: CYAN (x 70 a 80)
			else if (x >= 70 && x <= 80) {
				for (int i = 0; i < 4; i++) colorSeleccionado[i] = colorCyan[i];
			}
			// Click en Paleta: NARANJA (x 85 a 95)
			else if (x >= 85 && x <= 95) {
				for (int i = 0; i < 4; i++) colorSeleccionado[i] = colorNaranja[i];
			}
			// Click en Paleta: ROJO (x 100 a 110)
			else if (x >= 100 && x <= 110) {
				for (int i = 0; i < 4; i++) colorSeleccionado[i] = colorRojo[i];
			}
			// Click en Paleta: VERDE (x 115 a 125)
			else if (x >= 115 && x <= 125) {
				for (int i = 0; i < 4; i++) colorSeleccionado[i] = colorVerde[i];
			}

			// Forzar reinicio de trazos pendientes al tocar la UI para evitar desfases
			primerClickLinea = true; primerClickCirculo = true;

			// Redibujar la barra para actualizar los colores e indicadores visuales
			drawInterface();
			actualizarGPU();
			return; // Salir para no pintar nada en el lienzo
		}

		// ========================================================
		// DIBUJO EN EL LIENZO (ZONA INFERIOR)
		// ========================================================
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
	}
}

	int main()
	{
		// Inicializa glfw y versión de OpenGL
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow* window = glfwCreateWindow(800, 800, "AlgoritmoDeBresenhamm", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Error al crear la ventana GLFW" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetMouseButtonCallback(window, mouse_callback);
		gladLoadGL();

		setupGrid(); // Crea la geometría inicial

		// Compilación de Shaders
		unsigned int vS = glCreateShader(GL_VERTEX_SHADER); 
		glShaderSource(vS, 1, &vertexShaderSource, NULL); 
		glCompileShader(vS);

		unsigned int fS = glCreateShader(GL_FRAGMENT_SHADER); 
		glShaderSource(fS, 1, &fragmentShaderSource, NULL); 
		glCompileShader(fS);

		unsigned int prog = glCreateProgram();
		glAttachShader(prog, vS); glAttachShader(prog, fS);
		glLinkProgram(prog);

		// Configutación de Buffers (VAO, VBO, EBO)
		glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, gridVerts.size() * sizeof(VertexSquare), gridVerts.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices.size() * sizeof(GLuint), gridIndices.data(), GL_STATIC_DRAW);

		// Layout 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexSquare), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexSquare), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Bucle principal de renderizado
		while (!glfwWindowShouldClose(window))
		{
			// Ajusta el viewport si se cambia el tamaño de la ventana
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			// Limpia la pantalla
			glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Dibujo de la rejilla usando los índices (Triángulos)
			glUseProgram(prog);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, (GLsizei)gridIndices.size(), GL_UNSIGNED_INT, 0);

			glfwSwapBuffers(window); //Intercambia los buffers de imagen
			glfwPollEvents(); //Procesa eventos
		}

		//Limpia la memoria al cerrar
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glfwDestroyWindow(window);
		glfwTerminate();
		return 0;
	}
