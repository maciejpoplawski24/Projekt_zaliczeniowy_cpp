#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>

// --- STRUKTURY (Backend) ---
struct Karta {
    int wartosc;
    bool odkryta = false;
    std::string tekst() const {
        if (!odkryta) return "";
        const char* nazwy[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
        return nazwy[wartosc];
    }
};

struct StanGry {
    std::vector<Karta> talia;
    std::vector<Karta> stosy[10];
    int zebrane_krolestwa;
    float czas_gry;
};

enum StanRozgrywki { W_TRAKCIE, WYGRANA, PRZEGRANA };

// --- LOGIKA GRY (Backend) ---
class GraPajak {
public:
    std::vector<Karta> talia;
    std::vector<Karta> stosy[10];
    int zebrane_krolestwa = 0;
    StanRozgrywki stan = W_TRAKCIE;

    // Czas i Punktacja
    sf::Clock zegar;
    float czas_gry = 0.f;
    const int BAZA_PUNKTOW = 100000;

    // Historia do cofania ruchów
    std::vector<StanGry> historia;

    // Zmienne przygotowane dla interfejsu (Frontend)
    bool isDragging = false;
    int dragCol = -1;
    int dragRow = -1;
    sf::Vector2f dragOffset;
    sf::Vector2f mousePos;

    GraPajak() {
        for (int zestaw = 0; zestaw < 8; ++zestaw) {
            for (int w = 1; w <= 13; ++w) {
                talia.push_back({w, false});
            }
        }
        tasuj();
        rozdaj_poczatkowe();
        sprawdz_stan_gry();
        zegar.restart();
    }

    void tasuj() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(talia.begin(), talia.end(), g);
    }

    void rozdaj_poczatkowe() {
        for (int i = 0; i < 10; ++i) {
            int ile_kart = (i < 4) ? 6 : 5;
            for (int j = 0; j < ile_kart; ++j) {
                if (talia.empty()) break;
                Karta k = talia.back();
                talia.pop_back();
                if (j == ile_kart - 1) k.odkryta = true;
                stosy[i].push_back(k);
            }
        }
    }

    void aktualizujCzas() {
        if (stan == W_TRAKCIE) {
            czas_gry += zegar.restart().asSeconds();
        } else {
            zegar.restart();
        }
    }

    int obliczWynik() {
        int wynik = BAZA_PUNKTOW - (static_cast<int>(czas_gry) * 50);
        return (wynik < 0) ? 0 : wynik;
    }

    void zapisz_stan() {
        StanGry s;
        s.talia = talia;
        for (int i = 0; i < 10; ++i) s.stosy[i] = stosy[i];
        s.zebrane_krolestwa = zebrane_krolestwa;
        s.czas_gry = czas_gry;
        historia.push_back(s);
    }

    void cofnij_ruch() {
        if (historia.empty()) return;
        StanGry s = historia.back();
        historia.pop_back();

        talia = s.talia;
        for (int i = 0; i < 10; ++i) stosy[i] = s.stosy[i];
        zebrane_krolestwa = s.zebrane_krolestwa;
        czas_gry = s.czas_gry;

        isDragging = false;
        stan = W_TRAKCIE;
        zegar.restart();
        sprawdz_stan_gry();
    }

    void dobierz_z_talii() {
        if (talia.empty() || stan != W_TRAKCIE) return;
        zapisz_stan();
        for (int i = 0; i < 10; ++i) {
            if (talia.empty()) break;
            Karta k = talia.back();
            talia.pop_back();
            k.odkryta = true;
            stosy[i].push_back(k);
        }
        sprawdz_sekwencje();
        sprawdz_stan_gry();
    }

    void sprawdz_sekwencje() {
        for (int i = 0; i < 10; ++i) {
            if (stosy[i].size() >= 13) {
                bool pelna = true;
                int n = stosy[i].size();
                for (int j = 0; j < 13; ++j) {
                    if (!stosy[i][n - 1 - j].odkryta || stosy[i][n - 1 - j].wartosc != j + 1) {
                        pelna = false; break;
                    }
                }
                if (pelna) {
                    stosy[i].erase(stosy[i].end() - 13, stosy[i].end());
                    zebrane_krolestwa++;
                    if (!stosy[i].empty()) stosy[i].back().odkryta = true;
                }
            }
        }
    }

    bool czy_sa_ruchy() {
        if (!talia.empty()) return true;
        for (int i = 0; i < 10; ++i) {
            if (stosy[i].empty()) continue;
            int j = stosy[i].size() - 1;
            while (j > 0 && stosy[i][j - 1].odkryta && stosy[i][j - 1].wartosc == stosy[i][j].wartosc + 1) j--;
            for (size_t k = (size_t)j; k < stosy[i].size(); ++k) {
                int val = stosy[i][k].wartosc;
                for (int c = 0; c < 10; ++c) {
                    if (c == i) continue;
                    if (stosy[c].empty() || stosy[c].back().wartosc == val + 1) return true;
                }
            }
        }
        return false;
    }

    void sprawdz_stan_gry() {
        if (zebrane_krolestwa == 😎 stan = WYGRANA;
        else if (!czy_sa_ruchy()) stan = PRZEGRANA;
        else stan = W_TRAKCIE;
    }

}; // Koniec klasy GraPajak (tymczasowy)
