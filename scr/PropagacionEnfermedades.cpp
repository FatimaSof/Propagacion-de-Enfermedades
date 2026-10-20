#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <cstdlib>

using namespace std;

/*Avance 19/10/2026 hecho por Fatima:
Agregue que las bolitas no se puedan encimar en la misma celda
El estado de salud de las bolitas
Las bolitas aparecen en celdas aleatorias y ya no las tenemos que crear manualmente


*/

enum class EstadoSalud {
    Suceptible, Infectado, Recuperado
};

int direccionElegida(); //declaro la funcion para que no marque error
float numeroProb();



class Persona { //clase persona
private:
    sf::CircleShape forma;
    sf::Time momentoContagio;
    int celdaX;          // columna actual en la malla
    int celdaY;          // fila actual en la malla
    int ultimaDireccion; // 0=arriba,1=abajo,2=izq,3=der, -1=ninguna
    std::vector<pair<int, int>> historial; // últimas celdas visitadas

    EstadoSalud estadoSalud;

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
    Persona(int col, int fila) : celdaX(col), celdaY(fila), ultimaDireccion(-1), estadoSalud(EstadoSalud::Suceptible) {
        forma.setRadius(15.f);
        forma.setFillColor(sf::Color::Green);
        forma.setOrigin({ 15.f, 15.f });
        forma.setPosition(celdaAPixeles(col, fila));
        historial.push_back({ col, fila });
    }

	int getCeldaX() const { 
        return celdaX; 
    }

	int getCeldaY() const {
		return celdaY;
	}


    void dibujar(sf::RenderWindow& window) {
        window.draw(forma);
    }

    void infectar(sf::Time tiempoAct) {
        if (estadoSalud != EstadoSalud::Suceptible) {
            return;
        }

		estadoSalud = EstadoSalud::Infectado;
		momentoContagio = tiempoAct;
		actualizarColor();
    }

    void actSalud(sf::Time tiempoAct) {
        if (estadoSalud == EstadoSalud::Infectado) {
            if (tiempoAct - momentoContagio >= sf::seconds(10.f)) {
                estadoSalud = EstadoSalud::Recuperado;
                actualizarColor();
            }
        }
    }

	bool estaInfectado() const {
		return estadoSalud == EstadoSalud::Infectado;
	}



    void moverAleatoriamente(const std::vector<Persona>& personas, int indiceActual) {
        // Direcciones: 0=arriba,1=abajo,2=izq,3=der
        const int dx[4] = { 0,  0, -1,  1 };
        const int dy[4] = { -1,  1,  0,  0 };

        std::vector<int> opciones;

        auto ocupada = [&](int col, int fila) {
            for (int i = 0; i < personas.size(); i++) {
                if (i == indiceActual) {
                    continue;
                }
                if (personas[i].getCeldaX() == col && personas[i].getCeldaY() == fila) {
                    return true;
                }
            }
            return false;
        };



        for (int d = 0; d < 4; d++) {
            // No deshacer el último paso (evita ir y volver)
            if (ultimaDireccion != -1 && d == (ultimaDireccion ^ 1)) // 0<->1, 2<->3
                continue;

            int nc = celdaX + dx[d];
            int nf = celdaY + dy[d];

            if (!celdaValida(nc, nf)) continue;
			if (ocupada(nc, nf)) continue;
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
                if (ocupada(nc, nf)) continue;
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
                if (!celdaValida(nc, nf)) continue;
                if (ocupada(nc, nf)) continue;

                opciones.push_back(d);
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

    void actualizarColor() {
        if (estadoSalud == EstadoSalud::Suceptible) {
			forma.setFillColor(sf::Color::Green);

		}
		else if (estadoSalud == EstadoSalud::Infectado) {
			forma.setFillColor(sf::Color::Red);
		}
		else if (estadoSalud == EstadoSalud::Recuperado) {
			forma.setFillColor(sf::Color(150,150,150));
		}
    }


    void intentoContagio(std::vector<Persona>& personas, int indiceActual, float probabilidad, sf::Time tiempoActual) {
        if (estadoSalud != EstadoSalud::Infectado) {
            return;
        }
        for (int i = 0; i < personas.size(); i++) {
			if (i == indiceActual) {
				continue;
            }

			int diferenciaX = abs(celdaX - personas[i].getCeldaX());
			int diferenciaY = abs(celdaY - personas[i].getCeldaY());

            if (diferenciaX + diferenciaY != 1) {
				continue; 
            }

            if (personas[i].estadoSalud != EstadoSalud::Suceptible) {
                continue;
            }

			float numeroRandom = numeroProb();

            if (numeroRandom < probabilidad) {
				personas[i].infectar(tiempoActual);
            }
        }
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


float numeroProb() {
    std::random_device rd;
    std::mt19937 generador(rd());
    std::uniform_real_distribution<float> sorteo(0.0f, 100.0f);

    return sorteo(generador);

}
int direccionElegida() { //funcion que genera un numero aleatorio entre 0 y 3, que seran las direcciones
    std::random_device rd; //prepara el generador de numeros aleatorios
    std::mt19937 generador(rd()); //crea un generador de numeros aleatorios
    std::uniform_int_distribution<int> direccion(0, 3); //quiero numeros del 0 al 3, que seran las direcciones


    return direccion(generador);
}

void crearPersonasAleatorias(std::vector<Persona>& personas, int cantidadPersonas) {
    std::random_device rd;
    std::mt19937 generador(rd());

    std::uniform_int_distribution<int> columna(0, 15);
    std::uniform_int_distribution<int> fila(0, 11);

    while (personas.size() < cantidadPersonas) {
        int x = columna(generador);
        int y = fila(generador);

        bool ocupada = false;

        for (int i = 0; i < personas.size(); i++) {
            if (personas[i].getCeldaX() == x &&
                personas[i].getCeldaY() == y) {
                ocupada = true;
                break;
            }
        }

        if (!ocupada) {
            personas.emplace_back(x, y);
        }
    }
}




int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Propagacion de Enfermedades");

	std::vector<Persona> personas;
    sf::Clock reloj, relojSalud;
	Malla malla(800.f, 600.f, 50.f);
	int cantidadPersonas = 20; // Cambia este valor para crear más o menos personas
    float probContagio = 50.f;

	crearPersonasAleatorias(personas, cantidadPersonas);
	personas[0].infectar(relojSalud.getElapsedTime()); // Infectar la primera persona al inicio

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

		sf::Time tiempoActual = relojSalud.getElapsedTime();

		if (reloj.getElapsedTime().asMilliseconds() >= 500) {
			for (int i = 0; i < personas.size(); i++) {
				personas[i].actSalud(tiempoActual);
			}
            for (int i = 0; i < personas.size(); i++) {
				personas[i].intentoContagio(personas, i, probContagio, tiempoActual);
            }
            for (int i = 0; i < personas.size(); i++) {
				personas[i].moverAleatoriamente(personas, i);
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