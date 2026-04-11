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
GLfloat lineColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f };    // Color Cyan
GLfloat circleColor[4] = { 1.0f, 0.5f, 0.0f, 1.0f }; //Color naranja

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

// Función para cambiar el color de una celda específica
void paintCell(int x, int y, GLfloat color[4]) {
	if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) return;
	int base = (y * GRID_SIZE + x) * 4;
	for (int i = 0; i < 4; i++) { // Aplicar color a los 4 vértices de la celda
		gridVerts[base + i].color[0] = color[0];
		gridVerts[base + i].color[1] = color[1];
		gridVerts[base + i].color[2] = color[2];
		gridVerts[base + i].color[3] = color[3];
	}
}

// Algoritmo de Bresenham para la línea
void bresenhamLinea(int x0, int y0, int x1, int y1) {
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	while (true) {
		paintCell(x0, y0, lineColor); // Pinta la celda actual
		if (x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx) {
			err += dx;
			y0 += sy;
		}
	}
}

// Algoritmo de Bresenham para Círculos
void bresenhamCirculo(int cx, int cy, int r) {
	int x = 0;
	int y = r;
	int p = 3 - 2 * r; // Factor de decisión inicial

	// Función para pintar los 8 puntos simétricos del círculo
	auto plot = [&](int x, int y) { paintCell(cx + x, cy + y, circleColor); };

	while (y >= x) {
		plot(x, y); plot(-x, y); plot(x, -y); plot(-x, -y);
		plot(y, x); plot(-y, x); plot(y, -x); plot(-y, -x);
		x++;
		if (p > 0) {
			y--;
			p = p + 4 * (x - y) + 10;
		}
		else {
			p = p + 4 * x + 6;
		}
	}
}

//Actualiza los datos de los vértices (colores) en la GPU
void actualizarGPU() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gridVerts.size() * sizeof(VertexSquare), gridVerts.data());
}

// Procesa los eventos del mouse en la pantalla
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		// Convierte la posición del mouse en coordenadas de la rejilla
		int x = (int)(xpos / (w / (float)GRID_SIZE));
		int y = (int)((h - ypos) / (h / (float)GRID_SIZE));

		// CLICK IZQUIERDO: Línea
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (primerClickLinea) { xi = x; yi = y; primerClickLinea = false; }
			else { bresenhamLinea(xi, yi, x, y); actualizarGPU(); primerClickLinea = true; }
		}
		// CLICK DERECHO: Círculo
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (primerClickCirculo) { cxi = x; cyi = y; primerClickCirculo = false; }
			else {
				// Calcula el radio basado en la distancia entre clics
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
		unsigned int vS = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vS, 1, &vertexShaderSource, NULL); glCompileShader(vS);
		unsigned int fS = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fS, 1, &fragmentShaderSource, NULL); glCompileShader(fS);
		unsigned int prog = glCreateProgram(); glAttachShader(prog, vS); glAttachShader(prog, fS); glLinkProgram(prog);

		// Configutación de Buffers (VAO, VBO, EBO)
		glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO); glGenBuffers(1, &EBO);
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
