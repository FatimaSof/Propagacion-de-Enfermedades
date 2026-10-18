#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

using namespace std;

/*Explicacion de que hace cada cosa :
primero cree la clase persona la cual podras modificar para agregarle mas variables,
como lo de si esta infectado o no. Tambien esta la clase malla, que es la cuadricula que se dibuja en la ventana.
En la clase persona para hacer el movimiento genero un numero aleatorio del 0  al 4,
y luego por medio del switch hice que la figura mueva su coordenada. La clase malla no la toques
por que solo yo y chatgpt sabiamos lo que haciamos en ese momento con todos esos vectores :v
La funcion direccion elegida es la que me genera el numero aleatorio.
Luego esta el main y ahi cree por mientras 3 personas (bolitas) ya que lo tengamos mas avanzado
a recomendacion de mi patron chatsito metamos lo de la interfaz. luego se crea el reloj para hacer
el movimiento por que si no es demasiado rapido y no se veria como se mueven. Y por ultimo se dibuja
intente meter a todas las personas dentro de un vector para que sea mas sencillo al final y no estar
dibujando uno por uno, al final solo se recorre el vector personas. y listo uwu
*/



int direccionElegida(); //declaro la funcion para que no marque error

class Persona { //clase persona
private:
    sf::CircleShape forma;
    int celdaX;          // columna actual en la malla
    int celdaY;          // fila actual en la malla
    int ultimaDireccion; // 0=arriba,1=abajo,2=izq,3=der, -1=ninguna
    std::vector<pair<int, int>> historial; // últimas celdas visitadas

    static constexpr int TAMANO_CELDA = 50;
    static constexpr int MAX_HISTORIAL = 5;

    // Convierte una celda (col,fila) a posición en píxeles (centro de la celda)
    sf::Vector2f celdaAPixeles(int col, int fila) {
        return sf::Vector2f(col * TAMANO_CELDA + TAMANO_CELDA / 2.f,
            fila * TAMANO_CELDA + TAMANO_CELDA / 2.f);
    }

    bool celdaValida(int col, int fila) {
        // malla de 800x600 con celdas de 50 => 16 columnas x 12 filas
        return col >= 0 && col < 16 && fila >= 0 && fila < 12;
    }

    bool estaEnHistorial(int col, int fila) {
        for (auto& c : historial)
            if (c.first == col && c.second == fila) return true;
        return false;
    }

public:
    Persona(int col, int fila) : celdaX(col), celdaY(fila), ultimaDireccion(-1) {
        forma.setRadius(10.f);
        forma.setFillColor(sf::Color::Green);
        forma.setOrigin({ 10.f, 10.f });
        forma.setPosition(celdaAPixeles(col, fila));
        historial.push_back({ col, fila });
    }

    void dibujar(sf::RenderWindow& window) {
        window.draw(forma);
    }

    void moverAleatoriamente() {
        // Direcciones: 0=arriba,1=abajo,2=izq,3=der
        const int dx[4] = { 0,  0, -1,  1 };
        const int dy[4] = { -1,  1,  0,  0 };

        std::vector<int> opciones;

        for (int d = 0; d < 4; d++) {
            // No deshacer el último paso (evita ir y volver)
            if (ultimaDireccion != -1 && d == (ultimaDireccion ^ 1)) // 0<->1, 2<->3
                continue;

            int nc = celdaX + dx[d];
            int nf = celdaY + dy[d];

            if (!celdaValida(nc, nf)) continue;

            // Evitar celdas visitadas recientemente
            if (estaEnHistorial(nc, nf)) continue;

            opciones.push_back(d);
        }

        // Si no hay opciones "buenas", permitimos deshacer el último paso
        // pero sin repetir celdas del historial si es posible.
        if (opciones.empty()) {
            for (int d = 0; d < 4; d++) {
                int nc = celdaX + dx[d];
                int nf = celdaY + dy[d];
                if (!celdaValida(nc, nf)) continue;
                if (estaEnHistorial(nc, nf)) continue;
                opciones.push_back(d);
            }
        }

        // Si aún no hay opciones (raro), permitimos cualquier movimiento válido
        // excepto el opuesto si se puede evitar.
        if (opciones.empty()) {
            for (int d = 0; d < 4; d++) {
                int nc = celdaX + dx[d];
                int nf = celdaY + dy[d];
                if (celdaValida(nc, nf)) opciones.push_back(d);
            }
        }

        if (opciones.empty()) return; // completamente atrapado, no se mueve

        // Elegir una dirección al azar entre las opciones
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(0, (int)opciones.size() - 1);
        int d = opciones[dist(gen)];

        // Actualizar estado
        celdaX += dx[d];
        celdaY += dy[d];
        ultimaDireccion = d;
        forma.setPosition(celdaAPixeles(celdaX, celdaY));

        // Actualizar historial
        historial.push_back({ celdaX, celdaY });
        if (historial.size() > MAX_HISTORIAL)
            historial.erase(historial.begin());
    }
};

class Malla {
private:
    float ancho;
    float alto;
    float tamanoCelda;
public:
    Malla(float ancho, float alto, float tamanoCelda) : ancho(ancho), alto(alto), tamanoCelda(tamanoCelda) {}


    void dibujar(sf::RenderWindow& window) {

        // Lineas verticales
        for (float x = 0; x <= ancho; x += tamanoCelda) {
            sf::Vertex linea[2];

            linea[0].position = sf::Vector2f(x, 0.f);
            linea[0].color = sf::Color(70, 70, 70);

            linea[1].position = sf::Vector2f(x, alto);
            linea[1].color = sf::Color(70, 70, 70);

            window.draw(linea, 2, sf::PrimitiveType::Lines);
        }

        // Lineas horizontales
        for (float y = 0; y <= alto; y += tamanoCelda) {
            sf::Vertex linea[2];

            linea[0].position = sf::Vector2f(0.f, y);
            linea[0].color = sf::Color(70, 70, 70);

            linea[1].position = sf::Vector2f(ancho, y);
            linea[1].color = sf::Color(70, 70, 70);

            window.draw(linea, 2, sf::PrimitiveType::Lines);
        }
    }

};
int direccionElegida() { //funcion que genera un numero aleatorio entre 0 y 3, que seran las direcciones
    std::random_device rd; //prepara el generador de numeros aleatorios
    std::mt19937 generador(rd()); //crea un generador de numeros aleatorios
    std::uniform_int_distribution<int> direccion(0, 3); //quiero numeros del 0 al 3, que seran las direcciones


    return direccion(generador);
}






int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Propagacion de Enfermedades");

	std::vector<Persona> personas;
    sf::Clock reloj;
	Malla malla(800.f, 600.f, 50.f);

    personas.emplace_back(2, 2); // columna 2, fila 2 (celda del centro)
    personas.emplace_back(3, 2);
    personas.emplace_back(4, 2); 



    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        
		if (reloj.getElapsedTime().asMilliseconds() >= 500) {
			for (int i = 0; i < personas.size(); i++) {
				personas[i].moverAleatoriamente();
			}
			reloj.restart();

		}

		window.clear();
		malla.dibujar(window);
		for (int i = 0; i < personas.size(); i++) {
			personas[i].dibujar(window);
		}
        
		

        window.display();
    }

    return 0;
}