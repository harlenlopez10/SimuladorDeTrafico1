// Carro.cpp
#include "Carro.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <queue>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <unordered_set>

Carro::Carro(const std::string& nombre, const sf::Vector2f& posicion, float velocidad, Grafo& grafo, const std::string& rutaImagen, const std::vector<sf::Vector2f>& ruta, const std::map<std::string, Nodo>& nodosSemaforos, float tamanoCuadro)
    : Vehiculo(nombre, posicion, velocidad, rutaImagen), grafo(grafo), ruta(ruta), nodosSemaforos(nodosSemaforos), velocidad(10.0f), tamanoCuadro(tamanoCuadro) {
    sprite.setTexture(textura);
    sprite.setScale(0.1f, 0.1f); 
    this->posicion = !ruta.empty() ? ruta.front() : posicion;
    sprite.setPosition(this->posicion); 
}

// Carro.cpp
void Carro::mover(float deltaTime, const sf::Font& font) {
    if (colisionado) {
        tiempoDetenido -= deltaTime;
        if (tiempoDetenido <= 0) {
            colisionado = false;
        }
        return;
    }

    if (ruta.size() < 2) {
        return;
    }

    sf::Vector2f posicionActual = ruta.front();
    sf::Vector2f posicionSiguiente = ruta[1];

    sf::Vector2f direccion = posicionSiguiente - posicionActual;
    float distanciaTotal = std::sqrt(direccion.x * direccion.x + direccion.y * direccion.y);

    if (distanciaTotal > 0) {
        direccion /= distanciaTotal;
    }

    sf::Vector2f nuevaPosicion = posicion + direccion * velocidad * deltaTime;

    if (std::sqrt((nuevaPosicion.x - posicionActual.x) * (nuevaPosicion.x - posicionActual.x) + (nuevaPosicion.y - posicionActual.y) * (nuevaPosicion.y - posicionActual.y)) >= distanciaTotal) {
        ruta.erase(ruta.begin());

        if (!ruta.empty()) {
            posicion = ruta.front();
        }
    } else {
        posicion = nuevaPosicion;
    }

    sprite.setPosition(posicion);
}


std::vector<sf::Vector2f> Carro::generarRutaCiclica(Grafo& grafo, const std::string& nodoInicio, int cantidad) {
    std::vector<sf::Vector2f> ruta;
    std::unordered_set<std::string> nodosVisitados;
    std::string nodoActual = nodoInicio;

    ruta.push_back(grafo.obtenerPosicionNodo(nodoActual));
    nodosVisitados.insert(nodoActual);

    for (int i = 1; i < cantidad; ++i) {
        std::vector<std::string> nodosConectados = grafo.obtenerNodosConectados(nodoActual);

        if (nodosConectados.empty()) {
            break;
        }

        std::vector<std::string> nodosNoVisitados;
        std::copy_if(nodosConectados.begin(), nodosConectados.end(), std::back_inserter(nodosNoVisitados),
                     [&](const std::string& nodo) { return nodosVisitados.count(nodo) == 0; });

        if (!nodosNoVisitados.empty()) {
            int indiceAleatorio = rand() % nodosNoVisitados.size();
            nodoActual = nodosNoVisitados[indiceAleatorio];
            nodosVisitados.insert(nodoActual);
        } else {
            break;
        }

        sf::Vector2f posicionNodo = grafo.obtenerPosicionNodo(nodoActual);
        ruta.push_back(posicionNodo);
    }

    ruta.push_back(grafo.obtenerPosicionNodo(nodoInicio));

    return ruta;
}

void Carro::detener(float tiempo) {
    colisionado = true;
    tiempoDetenido = tiempo;
    velocidad = 0.0f;
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

void Carro::actualizarVelocidad(float nuevaVelocidad) {
    velocidad = nuevaVelocidad;
}

float Carro::getVelocidad() const {
    return velocidad;
}

bool Carro::verificarColisiones(const std::vector<Carro*>& listaDeCarros, std::vector<std::pair<Carro*, Carro*>>& colisiones) {
    const float radioColision = 20.0f;

    for (size_t i = 0; i < listaDeCarros.size(); ++i) {
        for (size_t j = i + 1; j < listaDeCarros.size(); ++j) {
            Carro* carro1 = listaDeCarros[i];
            Carro* carro2 = listaDeCarros[j];

            float distancia = std::sqrt(
                std::pow(carro1->obtenerPosicion().x - carro2->obtenerPosicion().x, 2) +
                std::pow(carro1->obtenerPosicion().y - carro2->obtenerPosicion().y, 2)
            );

            if (distancia < radioColision) {
                colisiones.push_back(std::make_pair(carro1, carro2));
                carro1->detener(10.0f);
                carro2->detener(10.0f);
                carro1->colisionado = true;
                carro2->colisionado = true;
                return true;
            }
        }
    }

    return false;
}

void Carro::mostrarColision(sf::RenderWindow& window, const std::vector<Carro*>& listaDeCarros) {
    std::vector<std::pair<Carro*, Carro*>> colisiones;
    bool hayColision = verificarColisiones(listaDeCarros, colisiones);

    if (hayColision) {
        colisionador.mostrarColision(window, true);
    }
}

bool Carro::colisionDetectada() const {
    return colisionado;
}

// Carro.cpp
const std::map<std::string, Nodo>& Carro::getNodosSemaforos() const {
    return nodosSemaforos;
}
