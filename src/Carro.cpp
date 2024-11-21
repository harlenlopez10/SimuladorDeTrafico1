#include "Carro.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>
#include <algorithm>
#include <SFML/Graphics.hpp>

// Constructor del Carro
Carro::Carro(const std::string& nombre, const sf::Vector2f& posicion, float velocidad, Grafo& grafo, const std::string& rutaImagen, const std::vector<sf::Vector2f>& ruta, const std::map<std::string, Nodo>& nodosSemaforos, float tamanoCuadro)
    : Vehiculo(nombre, posicion, velocidad, rutaImagen), grafo(grafo), ruta(ruta), nodosSemaforos(nodosSemaforos), tamanoCuadro(tamanoCuadro) {
    if (!textura.loadFromFile(rutaImagen)) {
        std::cerr << "Error al cargar la imagen del carro." << std::endl;
    }
    sprite.setTexture(textura);
    sprite.setScale(0.1f, 0.1f); 
    this->posicion = !ruta.empty() ? ruta.front() : posicion;
    sprite.setPosition(this->posicion); 
}

void Carro::mover(float deltaTime, const sf::Font& font) {
    if (colisionado) {
        tiempoDetenido -= deltaTime;
        if (tiempoDetenido <= 0) {
            colisionado = false;
        }
        return;  // No mover si está en colisión
    }

    if (ruta.size() < 2) {
        std::cout << "Ruta insuficiente para mover." << std::endl;
        return;
    }

    sf::Vector2f posicionActual = ruta.front();
    sf::Vector2f posicionSiguiente = ruta[1];

    sf::Vector2f direccion = posicionSiguiente - posicionActual;
    float distanciaTotal = std::sqrt(direccion.x * direccion.x + direccion.y * direccion.y);

    if (distanciaTotal > 0) {
        direccion /= distanciaTotal;  // Normaliza la dirección.
    }

    sf::Vector2f nuevaPosicion = posicion + direccion * velocidad * deltaTime;

    if (std::sqrt((nuevaPosicion.x - posicionActual.x) * (nuevaPosicion.x - posicionActual.x) + (nuevaPosicion.y - posicionActual.y) * (nuevaPosicion.y - posicionActual.y)) >= distanciaTotal) {
        std::cout << "Nodo alcanzado: (" << posicionSiguiente.x << ", " << posicionSiguiente.y << ")" << std::endl;

        ruta.erase(ruta.begin());  // Elimina el nodo alcanzado.

        if (!ruta.empty()) {
            posicion = ruta.front();  // Mueve a la siguiente posición.
        }
    } else {
        posicion = nuevaPosicion;  // Sigue avanzando hacia el siguiente nodo.
    }

    sprite.setPosition(posicion);  // Actualiza la posición del sprite.
}

void Carro::detener(float tiempo) {
    colisionado = true;
    tiempoDetenido = tiempo;
}

void Carro::dibujar(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2f Carro::obtenerDireccionDesdeArista(const Grafo& grafo, const std::string& nodoDesde, const std::string& nodoHacia) {
    return grafo.obtenerPosicionNodo(nodoHacia) - grafo.obtenerPosicionNodo(nodoDesde);
}

void Carro::calcularRuta(const std::string& nodoInicio, const std::string& nodoDestino) {
    std::map<std::string, float> distancias;
    std::map<std::string, std::string> predecesores;

    auto comparador = [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
        return a.second > b.second; 
    };

    std::priority_queue<std::pair<std::string, float>, std::vector<std::pair<std::string, float>>, decltype(comparador)> cola(comparador);

    for (const auto& nodo : grafo.getNodos()) {
        distancias[nodo.first] = std::numeric_limits<float>::infinity();
    }

    distancias[nodoInicio] = 0;
    cola.emplace(nodoInicio, 0);

    while (!cola.empty()) {
        auto [nodoActual, distanciaActual] = cola.top();
        cola.pop();

        if (nodoActual == nodoDestino) {
            ruta.clear();
            for (auto at = nodoDestino; !at.empty(); at = predecesores[at]) {
                ruta.push_back(grafo.obtenerPosicionNodo(at));
            }
            std::reverse(ruta.begin(), ruta.end());
            return;
        }

        for (const auto& nodoConectado : grafo.obtenerNodosConectados(nodoActual)) {
            float nuevaDistancia = distanciaActual + grafo.getPeso(nodoActual, nodoConectado);
            if (nuevaDistancia < distancias[nodoConectado]) {
                distancias[nodoConectado] = nuevaDistancia;
                predecesores[nodoConectado] = nodoActual;
                cola.emplace(nodoConectado, nuevaDistancia);
            }
        }
    }
}
