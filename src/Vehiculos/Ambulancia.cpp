// Ambulancia.cpp
#include "Ambulancia.h"

Ambulancia::Ambulancia(const std::string& nombre, const sf::Vector2f& posicion, float velocidad, Grafo& grafo, const std::vector<sf::Vector2f>& ruta, const std::map<std::string, Nodo>& nodosSemaforos)
    : Carro(nombre, posicion, velocidad, grafo, "../Resources/CarroEspecial.png", ruta, nodosSemaforos) {
}

void Ambulancia::mover(float deltaTime, const sf::Font& font) {
    Carro::mover(deltaTime, font);
}
