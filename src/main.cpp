// main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vehiculos/Carro.h"
#include "Vehiculos/Ambulancia.h"
#include "Grafo.h"
#include <cmath>
#include "Interfaz.h"
#include "Ruta.h"

// Declaración de funciones antes de su uso
void moverCarros(std::vector<Carro*>& vehiculos, float deltaTime, const sf::Font& font);
void dibujarTodo(sf::RenderWindow& window, Grafo& grafo, Interfaz& interfaz, const std::vector<Carro*>& vehiculos, bool mostrarEtiquetas);
void eliminarColisiones(std::vector<Carro*>& vehiculos);

const int MAX_AMBULANCIAS = 5; // Limitar el número de ambulancias activas

void verificarColisiones(std::vector<Carro*>& vehiculos, Grafo& grafo) {
    std::vector<std::pair<Carro*, Carro*>> colisiones;
    int numAmbulancias = 0;

    for (auto& carro : vehiculos) {
        if (dynamic_cast<Ambulancia*>(carro) != nullptr) {
            numAmbulancias++;
        }
    }

    for (auto& carro : vehiculos) {
        carro->verificarColisiones(vehiculos, colisiones);
    }

    for (auto& colision : colisiones) {
        if (numAmbulancias < MAX_AMBULANCIAS) {
            sf::Vector2f posicionColision = colision.first->obtenerPosicion();

            // Añadir una ambulancia para la colisión
            sf::Vector2f posicionInicialAmbulancia(100, 100); // Determina una posición inicial para la ambulancia
            std::string nodoInicial = grafo.obtenerNodoDesdePosicion(posicionInicialAmbulancia, 35.0f);
            std::string nodoColision = grafo.obtenerNodoDesdePosicion(posicionColision, 35.0f);
            std::vector<sf::Vector2f> rutaAmbulancia = grafo.generarRuta(nodoInicial, nodoColision);

            // Crear la instancia de la ambulancia con mayor velocidad
            float velocidadAmbulancia = 80.0f; // Cambia este valor para ajustar la velocidad de las ambulancias
            Ambulancia* ambulancia = new Ambulancia("Ambulancia", posicionInicialAmbulancia, velocidadAmbulancia, grafo, rutaAmbulancia, colision.first->getNodosSemaforos());

            vehiculos.push_back(ambulancia);
            numAmbulancias++;
        }
    }
}

void eliminarColisiones(std::vector<Carro*>& vehiculos) {
    for (auto it = vehiculos.begin(); it != vehiculos.end();) {
        if ((*it)->colisionDetectada() || dynamic_cast<Ambulancia*>(*it) != nullptr) {
            delete *it;
            it = vehiculos.erase(it);
        } else {
            ++it;
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Simulador de Trafico");

    std::vector<Carro*> vehiculos; 

    sf::Font font;
    if (!font.loadFromFile("../Resources/Roboto-BoldCondensed.ttf")) {
        // Manejar el error
    }

    Grafo grafo;
    Interfaz interfaz(font, grafo); 

    grafo.setInterfaz(&interfaz); 
    
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) { // Supón que 'E' es el botón para eliminar colisiones
                eliminarColisiones(interfaz.obtenerVehiculos());
            }

            interfaz.manejarEventos(event, window);
        }

        float deltaTime = clock.restart().asSeconds(); 
        grafo.actualizarSemaforos(deltaTime);
        moverCarros(interfaz.obtenerVehiculos(), deltaTime, font);
        verificarColisiones(interfaz.obtenerVehiculos(), grafo);
        dibujarTodo(window, grafo, interfaz, interfaz.obtenerVehiculos(), interfaz.isMostrarEtiquetas());
    }

    for (auto& carro : interfaz.obtenerVehiculos()) {
        delete carro;
    }

    return 0;
}

void moverCarros(std::vector<Carro*>& vehiculos, float deltaTime, const sf::Font& font) {
    for (auto& carro : vehiculos) {
        carro->mover(deltaTime, font);
    }
}

void dibujarTodo(sf::RenderWindow& window, Grafo& grafo, Interfaz& interfaz, const std::vector<Carro*>& vehiculos, bool mostrarEtiquetas) {
    window.clear(sf::Color(255, 229, 217));  

    sf::RectangleShape fondoGrafo(sf::Vector2f(window.getSize().x, window.getSize().y));
    fondoGrafo.setFillColor(sf::Color(255, 229, 217));
    window.draw(fondoGrafo);

    interfaz.crearPanelDerecho(window);
    interfaz.crearPanelSuperior(window);

    grafo.dibujar(window, mostrarEtiquetas);  

    for (const auto& par : grafo.obtenerNodos()) {
        par.second.dibujar(window);
    }

    for (const auto& carro : vehiculos) {
        carro->dibujar(window);
        carro->mostrarColision(window, vehiculos); 
    }

    window.display();  
}


/*

cd build
make
./TrafficSimulation


*/