#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

struct WynikGracza {
    std::string nick;
    int punkty;
};

struct Karta { 
    int wartosc; 
    int kolor; // 0=Trefl, 1=Karo, 2=Kier, 3=Pik
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
    
    sf::Clock zegar;
    const int BAZA_PUNKTOW = 100000; 

    std::vector<StanGry> historia;

    bool isDragging = false;
    int dragCol = -1;
    int dragRow = -1;
    sf::Vector2f dragOffset;
    sf::Vector2f mousePos;

public:
    float czas_gry = 0.f;
    StanRozgrywki stan = W_TRAKCIE;

    // --- ZMIENNE GRACZA I RANKINGU ---
    std::string aktualnyGracz = "Nieznajomy";
    int aktualnyTryb = 1; 
    bool wynikZapisany = false;

    // --- SYSTEM WSKAZÓWEK ---
    bool pokazWskazowke = false;
    int hintZ_Kol = -1;
    int hintZ_Rzad = -1;
    int hintDo_Kol = -1;

    // --- TEKSTURY ---
    sf::Texture texTlo1; 
    sf::Texture texTlo2; 
    sf::Texture texTlo3; 
    sf::Texture texRewers;
    sf::Texture texRewers2;
    // Tablice 2D dla 4 kolorów i 14 wartości
    sf::Texture texAwers[4][14]; 
    sf::Texture texAwers2[4][14];
    sf::Sprite spriteTlo;

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
    void rysuj(sf::RenderWindow& window, sf::Font& font, int wybranyStyl, int wybraneTlo);
    void resetuj();
    bool czyKoniecGry() const { return stan != W_TRAKCIE; }
    void zaladujTekstury();
    
    void znajdzWskazowke();
    void wyczyscWskazowke() { pokazWskazowke = false; }

    // --- METODY PLIKOWE ---
    std::vector<std::string> wczytajProfile();
    void dodajProfil(const std::string& nick);
    void zapiszWynikRanking(int punkty);
    std::vector<WynikGracza> pobierzRanking(int tryb);
};
