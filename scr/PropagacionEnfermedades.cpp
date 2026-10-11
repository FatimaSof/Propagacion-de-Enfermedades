#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

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
    sf::CircleShape forma; //figura

public:

	Persona(float x, float y) { //constructor que recibe la posicion de la persona
		forma.setRadius(10.f); //radio de la figura
		forma.setFillColor(sf::Color::Green); //color de la figura
		forma.setOrigin({ 10.f, 10.f }); //origen de la figura
		forma.setPosition({ x + 25.f, y + 25.f }); //posicion de la figura
    }

	void dibujar(sf::RenderWindow& window) { // para que se dibuje la figura en la ventana
		window.draw(forma);
	}

	void moverAleatoriamente() { // se genera un numero aleatorio para elegir la direccion en la que se movera la figura
		
		sf::Vector2f posicionAnterior = forma.getPosition(); //guarda la posicion anterior de la figura	
		int direccion = direccionElegida();

		switch (direccion) {
		case 0: // Arriba
			forma.move({ 0.f, -50.f });
			break;
		case 1: // Abajo
			forma.move({ 0.f, 50.f });
			break;
		case 2: // Izquierda
			forma.move({ -50.f, 0.f });
			break;
		case 3: // Derecha
			forma.move({ 50.f, 0.f });
			break;
		}

		sf::Vector2f posicionNueva = forma.getPosition();

		if(posicionNueva.x < 25  || posicionNueva.x > 775 || posicionNueva.y < 25 || posicionNueva.y > 575)
		{
			forma.setPosition(posicionAnterior);
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

		
		void dibujar(sf::RenderWindow & window) {

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

	personas.emplace_back(100.f, 100.f);
    personas.emplace_back(150.f, 100.f);
	personas.emplace_back(200.f, 100.f);



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