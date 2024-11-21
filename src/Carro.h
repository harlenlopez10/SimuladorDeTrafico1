#ifndef CARRO_H
#define CARRO_H

#include "Vehiculo.h"
#include "Grafo.h"
#include <vector>
#include "Interfaz.h"
#include "Semaforo.h"

class Carro : public Vehiculo {
public:
    Carro(const std::string& nombre, const sf::Vector2f& posicion, float velocidad, Grafo& grafo, const std::string& rutaImagen, const std::vector<sf::Vector2f>& ruta, const std::map<std::string, Nodo>& nodosSemaforos, float tamanoCuadro = 50.0f);

    sf::Vector2f obtenerDireccionDesdeArista(const Grafo& grafo, const std::string& nodoDesde, const std::string& nodoHacia);

    void dibujar(sf::RenderWindow& window);
    void calcularRuta(const std::string& nodoInicio, const std::string& nodoDestino);
    void mover(float deltaTime, const sf::Font& font); // Asegúrate de que mover acepte font
    void detener(float tiempo); // Declarar la función detener correctamente
    void verificarColisiones(const std::map<std::string, Nodo>& nodosSemaforos, const sf::Font& font); // Ajuste aquí
    const std::map<std::string, Nodo>& getNodosSemaforos() const; // Getter para nodosSemaforos

private:
    Grafo& grafo; 
    float tamanoCuadro;
    std::vector<sf::Vector2f> ruta;
    std::map<std::string, Nodo> nodosSemaforos;
    bool colisionado = false; // Variable para determinar si el carro está en colisión
    float tiempoDetenido = 0; // Tiempo que el carro se detiene tras una colisión
};

#endif // CARRO_H
