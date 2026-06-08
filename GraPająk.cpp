#include "GraPajak.h"
#include <algorithm>
#include <random>

std::string Karta::tekst() const {
    if (!odkryta) return "";
    const char* nazwy[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    return nazwy[wartosc];
}

GraPajak::GraPajak() { 
    zaladujTekstury(); // <--- DODANE
    for (int zestaw = 0; zestaw < 8; ++zestaw) {
        for (int w = 1; w <= 13; ++w) talia.push_back({w, false});
    }
    tasuj();
    rozdaj_poczatkowe();
    sprawdz_stan_gry();
    zegar.restart();
}

void GraPajak::tasuj() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(talia.begin(), talia.end(), g);
}

void GraPajak::rozdaj_poczatkowe() {
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

void GraPajak::aktualizujCzas() {
    if (stan == W_TRAKCIE) czas_gry += zegar.restart().asSeconds();
    else zegar.restart(); 
}

int GraPajak::obliczWynik() {
    int wynik = BAZA_PUNKTOW - (static_cast<int>(czas_gry) * 50);
    return (wynik < 0) ? 0 : wynik;
}

void GraPajak::zapisz_stan() {
    StanGry s;
    s.talia = talia;
    for (int i = 0; i < 10; ++i) s.stosy[i] = stosy[i];
    s.zebrane_krolestwa = zebrane_krolestwa;
    s.czas_gry = czas_gry;
    historia.push_back(s);
}

void GraPajak::cofnij_ruch() {
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

void GraPajak::dobierz_z_talii() {
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

void GraPajak::sprawdz_sekwencje() {
    for (int i = 0; i < 10; ++i) {
        if (stosy[i].size() >= 13) {
            bool pelna = true;
            int n = stosy[i].size();
            for (int j = 0; j < 13; ++j) {
                if (!stosy[i][n - 1 - j].odkryta || stosy[i][n - 1 - j].wartosc != j + 1) { pelna = false; break; }
            }
            if (pelna) {
                stosy[i].erase(stosy[i].end() - 13, stosy[i].end());
                zebrane_krolestwa++;
                if (!stosy[i].empty()) stosy[i].back().odkryta = true;
            }
        }
    }
}

bool GraPajak::czy_sa_ruchy() {
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

void GraPajak::sprawdz_stan_gry() {
    if (zebrane_krolestwa == 😎 stan = WYGRANA;
    else if (!czy_sa_ruchy()) stan = PRZEGRANA;
    else stan = W_TRAKCIE;
}

void GraPajak::obsluzKlikniecie(sf::Vector2f klik) {
    sf::FloatRect boundsCofnij(750.f, 50.f, 80.f, 40.f);
    if (boundsCofnij.contains(klik)) { cofnij_ruch(); return; }

    sf::FloatRect boundsDev(850.f, 700.f, 120.f, 40.f);
    if (boundsDev.contains(klik)) {
        zapisz_stan();
        zebrane_krolestwa = 8;
        talia.clear();
        for (int i = 0; i < 10; ++i) stosy[i].clear();
        sprawdz_stan_gry();
        return;
    }

    if (stan != W_TRAKCIE) return;
    sf::FloatRect boundsTalia(850.f, 50.f, 70.f, 100.f);
    if (boundsTalia.contains(klik)) { dobierz_z_talii(); return; }

    float startY = 200.f;
    for (int i = 0; i < 10 && !isDragging; ++i) {
        for (int j = (int)stosy[i].size() - 1; j >= 0; --j) {
            sf::FloatRect bounds(50.f + i * 90.f, startY + j * 20.f, 70.f, 100.f);
            if (bounds.contains(klik) && stosy[i][j].odkryta) {
                bool moznaPodniesc = true;
                for(size_t k = (size_t)j; k < stosy[i].size() - 1; ++k) {
                    if (stosy[i][k].wartosc != stosy[i][k+1].wartosc + 1) { moznaPodniesc = false; break; }
                }
                if (moznaPodniesc) {
                    isDragging = true;
                    dragCol = i; dragRow = j;
                    dragOffset = klik - sf::Vector2f(bounds.left, bounds.top);
                    return;
                }
            }
        }
    }
}

void GraPajak::obsluzPuszczenie() {
    if (!isDragging) return;
    int targetCol = -1;
    for (int i = 0; i < 10; ++i) {
        float colX = 50.f + i * 90.f;
        if (mousePos.x >= colX && mousePos.x <= colX + 70.f) { targetCol = i; break; }
    }
    if (targetCol != -1 && targetCol != dragCol) {
        if (stosy[targetCol].empty() || stosy[targetCol].back().wartosc == stosy[dragCol][dragRow].wartosc + 1) {
            zapisz_stan();
            for (size_t k = (size_t)dragRow; k < stosy[dragCol].size(); ++k) stosy[targetCol].push_back(stosy[dragCol][k]);
            stosy[dragCol].erase(stosy[dragCol].begin() + dragRow, stosy[dragCol].end());
            if (!stosy[dragCol].empty()) stosy[dragCol].back().odkryta = true;
            sprawdz_sekwencje(); sprawdz_stan_gry();
        }
    }
    isDragging = false;
}

void GraPajak::aktualizujMysz(sf::Vector2f pos) { mousePos = pos; }



// --- RYSOWANIE (Metoda zadeklarowana przez Piotrka, ale napisze ją Maciej w mainie) ---
void GraPajak::rysuj(sf::RenderWindow& window, sf::Font& font) {
    // Implementacja znajduje się w main.cpp (lub zostanie dopisana przez grafika)
}

void GraPajak::zaladujTekstury() {
    if(texTlo.loadFromFile("assets/tlo1.jpg")) {
        spriteTlo.setTexture(texTlo);
        // Opcjonalne skalowanie tła pod okno 1000x800
        float scaleX = 1000.f / spriteTlo.getLocalBounds().width;
        float scaleY = 800.f / spriteTlo.getLocalBounds().height;
        spriteTlo.setScale(scaleX, scaleY);
    }

    texRewers.loadFromFile("assets/card_back.png");

    texAwers[1].loadFromFile("assets/card_clubs_A.png");
    texAwers[2].loadFromFile("assets/card_clubs_02.png");
    texAwers[3].loadFromFile("assets/card_clubs_03.png");
    texAwers[4].loadFromFile("assets/card_clubs_04.png");
    texAwers[5].loadFromFile("assets/card_clubs_05.png");
    texAwers[6].loadFromFile("assets/card_clubs_06.png");
    texAwers[7].loadFromFile("assets/card_clubs_07.png");
    texAwers[8].loadFromFile("assets/card_clubs_08.png");
    texAwers[9].loadFromFile("assets/card_clubs_09.png");
    texAwers[10].loadFromFile("assets/card_clubs_10.png");
    texAwers[11].loadFromFile("assets/card_clubs_J.png");
    texAwers[12].loadFromFile("assets/card_clubs_Q.png");
    texAwers[13].loadFromFile("assets/card_clubs_K.png");
}
