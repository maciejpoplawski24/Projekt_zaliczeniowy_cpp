#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

struct Karta {
    int wartosc;
    bool odkryta = false;
    std::string tekst() const;
};

struct StanGry {
    std::vector<Karta> talia;
    std::vector<Karta> stosy[10];
    int zebrane_krolestwa;
    float czas_gry; 
};

enum StanRozgrywki { W_TRAKCIE, WYGRANA, PRZEGRANA };

class GraPajak {
    std::vector<Karta> talia;
    std::vector<Karta> stosy[10];
    int zebrane_krolestwa = 0;
    StanRozgrywki stan = W_TRAKCIE;

    sf::Clock zegar;
    float czas_gry = 0.f;
    const int BAZA_PUNKTOW = 100000; 

    std::vector<StanGry> historia;

    bool isDragging = false;
    int dragCol = -1;
    int dragRow = -1;
    sf::Vector2f dragOffset;
    sf::Vector2f mousePos;

public:
    GraPajak();
    void tasuj();
    void rozdaj_poczatkowe();
    void aktualizujCzas();
    int obliczWynik();
    void zapisz_stan();
    void cofnij_ruch();
    void dobierz_z_talii();
    void sprawdz_sekwencje();
    bool czy_sa_ruchy();
    void sprawdz_stan_gry();
    void obsluzKlikniecie(sf::Vector2f klik);
    void obsluzPuszczenie();
    void aktualizujMysz(sf::Vector2f pos);
    void rysuj(sf::RenderWindow& window, sf::Font& font);
};
