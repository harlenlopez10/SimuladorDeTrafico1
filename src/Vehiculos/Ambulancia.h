#ifndef AMBULANCIA_H
#define AMBULANCIA_H

#include "Carro.h"

class Ambulancia : public Carro {
public:
    Ambulancia(const std::string& nombre, const sf::Vector2f& posicion, float velocidad, Grafo& grafo, const std::vector<sf::Vector2f>& ruta, const std::map<std::string, Nodo>& nodosSemaforos);

    void mover(float deltaTime, const sf::Font& font);
};

#endif // AMBULANCIA_H

