#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>  // Incluir cmath para std::sqrt
#include "Carro.h"
#include "Grafo.h"
#include "Interfaz.h"
#include "Ruta.h"

void dibujarTodo(sf::RenderWindow& window, Grafo& grafo, Interfaz& interfaz, const std::vector<Carro*>& vehiculos, bool mostrarEtiquetas) {
    window.clear(sf::Color(255, 255, 255)); 
    interfaz.crearPanelSuperior(window);
    interfaz.crearPanelDerecho(window);
    grafo.dibujar(window, mostrarEtiquetas);
    
    for (const auto& carro : vehiculos) {
        carro->dibujar(window);
    }

    window.display();
}

void moverCarros(std::vector<Carro*>& vehiculos, float deltaTime, const sf::Font& font) {
    for (auto& carro : vehiculos) {
        carro->mover(deltaTime, font); // Pasar ambos argumentos a mover
    }
}

void verificarColisiones(std::vector<Carro*>& vehiculos, sf::RenderWindow& window, const sf::Font& font) {
    static bool colisionDetectada = false;

    for (size_t i = 0; i < vehiculos.size(); ++i) {
        for (size_t j = i + 1; j < vehiculos.size(); ++j) {
            if (std::sqrt(std::pow(vehiculos[i]->obtenerPosicion().x - vehiculos[j]->obtenerPosicion().x, 2) + 
                          std::pow(vehiculos[i]->obtenerPosicion().y - vehiculos[j]->obtenerPosicion().y, 2)) < 10) { // Ajuste de margen de colisión
                
                if (!colisionDetectada) {
                    vehiculos[i]->detener(10.0f);
                    vehiculos[j]->detener(10.0f);

                    std::cout << "Colisión detectada entre (" << vehiculos[i]->obtenerPosicion().x << ", " << vehiculos[i]->obtenerPosicion().y << ") y (" 
                              << vehiculos[j]->obtenerPosicion().x << ", " << vehiculos[j]->obtenerPosicion().y << ")" << std::endl;

                    colisionDetectada = true;

                    // Crear una ventana emergente
                    sf::RenderWindow alertWindow(sf::VideoMode(300, 200), "Alerta de Colisión");
                    while (alertWindow.isOpen()) {
                        sf::Event event;
                        while (alertWindow.pollEvent(event)) {
                            if (event.type == sf::Event::Closed)
                                alertWindow.close();
                        }

                        alertWindow.clear();
                        sf::Text text;
                        text.setFont(font);
                        text.setString("Colisión detectada entre carros");
                        text.setCharacterSize(24);
                        text.setFillColor(sf::Color::Red);
                        text.setPosition(20, 80);
                        alertWindow.draw(text);
                        alertWindow.display();
                    }
                }
                return;  // Salir después de manejar la colisión
            }
        }
    }
    colisionDetectada = false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Simulador de Trafico");
    Grafo grafo; 
    std::vector<Carro*> vehiculos; 

    sf::Font font;
    if (!font.loadFromFile("../src/Roboto-BoldCondensed.ttf")) {
        std::cerr << "Error al cargar la fuente." << std::endl;
        return -1; // Termina si la fuente no se carga
    }

    Interfaz interfaz(font, grafo);
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            interfaz.manejarEventos(event, window);
        }

        float deltaTime = clock.restart().asSeconds();
        grafo.actualizarSemaforos(deltaTime);
        moverCarros(interfaz.obtenerVehiculos(), deltaTime, font); // Pasar la fuente a moverCarros
        verificarColisiones(interfaz.obtenerVehiculos(), window, font);
        dibujarTodo(window, grafo, interfaz, interfaz.obtenerVehiculos(), interfaz.isMostrarEtiquetas());
    }

    for (auto& carro : interfaz.obtenerVehiculos()) {
        delete carro;
    }

    return 0;
}

/*
./SimuladorTrafico
g++ -o SimuladorTrafico main.cpp Interfaz.cpp Grafo.cpp Vehiculo.cpp BotonManager.cpp Carro.cpp Semaforo.cpp Nodo.cpp Ruta.cpp -I/opt/homebrew/opt/sfml/include -L/opt/homebrew/opt/sfml/lib -std=c++11 -lsfml-graphics -lsfml-window -lsfml-system

chmod +x SimuladorTrafico

g++ -o SimuladorTrafico main.cpp Interfaz.cpp Grafo.cpp Vehiculo.cpp BotonManager.cpp Carro.cpp Semaforo.cpp Nodo.cpp Ruta.cpp -I/opt/homebrew/opt/sfml/include -L/opt/homebrew/opt/sfml/lib -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
*/