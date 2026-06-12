#include "GraPajak.h"
#include <algorithm>
#include <random>
#include <fstream>

std::string Karta::tekst() const {
    if (!odkryta) return "";
    const char* nazwy[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    return nazwy[wartosc];
}

GraPajak::GraPajak() {
    zaladujTekstury(); 
    // Domyślny start przy włączeniu
    for (int zestaw = 0; zestaw < 8; ++zestaw) {
        for (int w = 1; w <= 13; ++w) talia.push_back({w, 0, false});
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
    float offX = 90.f; float offY = 20.f; float startY = 200.f;
    float drawSzer = (obecnyStyl == 1) ? 86.f : 70.f;
    float drawWys  = (obecnyStyl == 1) ? 120.f : 100.f;
    float shiftX   = (obecnyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (obecnyStyl == 1) ? -10.f : 0.f;
    sf::Vector2f startPos(850.f + shiftX, 50.f + shiftY);

    for (int i = 0; i < 10; ++i) {
        int ile_kart = (i < 4) ? 6 : 5;
        for (int j = 0; j < ile_kart; ++j) {
            if (talia.empty()) break;
            Karta k = talia.back();
            talia.pop_back();
            if (j == ile_kart - 1) k.odkryta = true;
            
            sf::Sprite spr;
            if (k.odkryta) {
                if (obecnyStyl == 1) spr.setTexture(texAwers[k.kolor][k.wartosc]);
                else spr.setTexture(texAwers2[k.kolor][k.wartosc]);
            } else {
                if (obecnyStyl == 1) spr.setTexture(texRewers);
                else spr.setTexture(texRewers2);
            }
            spr.setScale(drawSzer / spr.getLocalBounds().width, drawWys / spr.getLocalBounds().height);
            
            sf::Vector2f cel(50.f + i * offX + shiftX, startY + j * offY + shiftY);
            float opoznienie = (i * ile_kart + j) * 0.02f; // Kaskadowe opoznienie dla lepszego efektu
            obiektyGry.push_back(std::make_unique<AnimowanaKartaRozdania>(k, spr, startPos, cel, i, opoznienie));
        }
    }
    stan = ANIMACJA_ROZDAWANIA;
}

void GraPajak::aktualizujCzas() {
    if (stan == W_TRAKCIE) czas_gry += zegar.restart().asSeconds();
    else zegar.restart();
}

int GraPajak::obliczWynik() {
    int wynik = BAZA_PUNKTOW - (static_cast<int>(czas_gry) * 50);
    return (wynik < 0) ? 0 : wynik;
}

void GraPajak::zapiszGreDoPliku(const std::string& sciezka) {
    std::ofstream out(sciezka);
    if (!out) return;
    out << aktualnyGracz << "\n" << aktualnyTryb << "\n" << czas_gry << "\n" << zebrane_krolestwa << "\n";
    out << talia.size() << "\n";
    for (const auto& k : talia) out << k.wartosc << " " << k.kolor << " " << k.odkryta << "\n";
    for (int i = 0; i < 10; ++i) {
        out << stosy[i].size() << "\n";
        for (const auto& k : stosy[i]) out << k.wartosc << " " << k.kolor << " " << k.odkryta << "\n";
    }
}

void GraPajak::wczytajGreZPliku(const std::string& sciezka) {
    std::ifstream in(sciezka);
    if (!in) return;
    in >> aktualnyGracz >> aktualnyTryb >> czas_gry >> zebrane_krolestwa;
    size_t s; in >> s;
    talia.clear();
    for (size_t i = 0; i < s; ++i) { Karta k; in >> k.wartosc >> k.kolor >> k.odkryta; talia.push_back(k); }
    for (int i = 0; i < 10; ++i) {
        in >> s;
        stosy[i].clear();
        for (size_t j = 0; j < s; ++j) { Karta k; in >> k.wartosc >> k.kolor >> k.odkryta; stosy[i].push_back(k); }
    }
    stan = W_TRAKCIE;
    historia.clear();
    obiektyGry.clear();
    ekranPodsumowania = false;
    zegar.restart();
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
    wyczyscWskazowke();
    zegar.restart();
    sprawdz_stan_gry();
}

void GraPajak::dobierz_z_talii() {
    if (talia.empty() || stan == ANIMACJA_ROZDAWANIA || stan != W_TRAKCIE) return;
    zapisz_stan();

    float offX = 90.f; float offY = 20.f; float startY = 200.f;
    float drawSzer = (obecnyStyl == 1) ? 86.f : 70.f;
    float drawWys  = (obecnyStyl == 1) ? 120.f : 100.f;
    float shiftX   = (obecnyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (obecnyStyl == 1) ? -10.f : 0.f;
    sf::Vector2f startPos(850.f + shiftX, 50.f + shiftY);

    for (int i = 0; i < 10; ++i) {
        if (talia.empty()) break;
        Karta k = talia.back();
        talia.pop_back();
        k.odkryta = true;
        
        sf::Sprite spr;
        if (obecnyStyl == 1) spr.setTexture(texAwers[k.kolor][k.wartosc]);
        else spr.setTexture(texAwers2[k.kolor][k.wartosc]);
        spr.setScale(drawSzer / spr.getLocalBounds().width, drawWys / spr.getLocalBounds().height);
        
        int j = stosy[i].size();
        sf::Vector2f cel(50.f + i * offX + shiftX, startY + j * offY + shiftY);
        float opoznienie = i * 0.05f; 
        
        obiektyGry.push_back(std::make_unique<AnimowanaKartaRozdania>(k, spr, startPos, cel, i, opoznienie));
    }
    stan = ANIMACJA_ROZDAWANIA;
}

void GraPajak::sprawdz_sekwencje() {
    for (int i = 0; i < 10; ++i) {
        if (stosy[i].size() >= 13) {
            int rozmiar = stosy[i].size();
            int kolorSekwencji = stosy[i].back().kolor;
            bool pelna = true;
            
            for (int j = 0; j < 13; ++j) {
                if (!stosy[i][rozmiar - 1 - j].odkryta || 
                    stosy[i][rozmiar - 1 - j].wartosc != j + 1 ||
                    stosy[i][rozmiar - 1 - j].kolor != kolorSekwencji) { 
                    pelna = false; 
                    break; 
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

bool GraPajak::czy_sa_ruchy() {
    if (!talia.empty()) return true;
    for (int i = 0; i < 10; ++i) {
        if (stosy[i].empty()) continue;
        int j = stosy[i].size() - 1;
        while (j > 0 && stosy[i][j - 1].odkryta && stosy[i][j - 1].wartosc == stosy[i][j].wartosc + 1 && stosy[i][j - 1].kolor == stosy[i][j].kolor) j--;
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
    if (stan == ANIMACJA_ROZDAWANIA) return;
    if (zebrane_krolestwa == 8) {
        stan = WYGRANA;
        if (!wynikZapisany) {
            zapiszWynikRanking(obliczWynik());
            wynikZapisany = true;
        }
        if (obiektyGry.empty()) {
            inicjujAnimacjeWygranej(obecnyStyl);
        }
    }
    else if (!czy_sa_ruchy()) stan = PRZEGRANA;
    else stan = W_TRAKCIE;
}

void GraPajak::obsluzKlikniecie(sf::Vector2f klik, int wybranyStyl) {
    wyczyscWskazowke();
    
    sf::FloatRect boundsCofnij(750.f, 50.f, 80.f, 40.f);
    if (boundsCofnij.contains(klik)) { cofnij_ruch(); return; }

    sf::FloatRect boundsDev(850.f, 700.f, 120.f, 40.f);
    if (boundsDev.contains(klik)) {
        zapisz_stan();
        stan = WYGRANA;
        zapiszWynikRanking(obliczWynik());
        wynikZapisany = true;
        if (obiektyGry.empty()) {
            inicjujAnimacjeWygranej(wybranyStyl);
        }
        return;
    }

    if (stan != W_TRAKCIE && stan != ANIMACJA_ROZDAWANIA) return;
    if (stan == ANIMACJA_ROZDAWANIA) {
        przyspieszRozdawanie();
        return;
    }

    float szer = 70.f; float wys = 100.f;
    float offX = 90.f; float offY = 20.f;
    float drawSzer = (wybranyStyl == 1) ? 86.f : szer;
    float drawWys  = (wybranyStyl == 1) ? 120.f : wys;
    float shiftX   = (wybranyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (wybranyStyl == 1) ? -10.f : 0.f;

    sf::FloatRect boundsTalia(850.f + shiftX, 50.f + shiftY, drawSzer, drawWys);
    if (boundsTalia.contains(klik)) { dobierz_z_talii(); return; }

    float startY = 200.f;
    for (int i = 0; i < 10 && !isDragging; ++i) {
        for (int j = (int)stosy[i].size() - 1; j >= 0; --j) {
            sf::FloatRect bounds(50.f + i * offX + shiftX, startY + j * offY + shiftY, drawSzer, drawWys);
            if (bounds.contains(klik) && stosy[i][j].odkryta) {
                bool moznaPodniesc = true;
                for(size_t k = (size_t)j; k < stosy[i].size() - 1; ++k) {
                    if (stosy[i][k].wartosc != stosy[i][k+1].wartosc + 1 || 
                        stosy[i][k].kolor != stosy[i][k+1].kolor) { 
                        moznaPodniesc = false; 
                        break; 
                    }
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

void GraPajak::obsluzPuszczenie(int wybranyStyl) {
    if (!isDragging) return;
    
    float offX = 90.f;
    float drawSzer = (wybranyStyl == 1) ? 86.f : 70.f;
    float shiftX   = (wybranyStyl == 1) ? -8.f : 0.f;

    int targetCol = -1;
    for (int i = 0; i < 10; ++i) {
        float colX = 50.f + i * offX + shiftX;
        if (mousePos.x >= colX && mousePos.x <= colX + drawSzer) { targetCol = i; break; }
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

void GraPajak::zaladujTekstury() {
    if (bufWygrana.loadFromFile("assets/566203__colorscrimsontears__fanfare-rpg.wav")) {
        dzwiekWygrana.setBuffer(bufWygrana);
    }

    texTlo1.loadFromFile("assets/tlo1.jpg"); 
    texTlo2.loadFromFile("assets/tlo2.jpg"); 
    texTlo3.loadFromFile("assets/tlo3.jpg"); 

    texRewers.loadFromFile("assets/card_back.png");
    texRewers2.loadFromFile("assets/cardBack_blue2.png");

    std::string nazwyStyl1[] = {"clubs", "diamonds", "hearts", "spades"};
    std::string nazwyStyl2[] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    std::string wartosci[]   = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

    for (int k = 0; k < 4; ++k) {
        for (int w = 1; w <= 13; ++w) {
            std::string strW = wartosci[w];
            std::string strW1 = (w >= 2 && w <= 9) ? "0" + strW : strW;
            texAwers[k][w].loadFromFile("assets/card_" + nazwyStyl1[k] + "_" + strW1 + ".png");
            texAwers2[k][w].loadFromFile("assets/card" + nazwyStyl2[k] + strW + ".png");
        }
    }
}

void GraPajak::resetuj() {
    talia.clear();
    for (int i = 0; i < 10; ++i) stosy[i].clear();
    historia.clear();
    zebrane_krolestwa = 0;
    stan = W_TRAKCIE;
    czas_gry = 0.f;
    isDragging = false;
    wynikZapisany = false;
    wyczyscWskazowke();
    obiektyGry.clear();
    ekranPodsumowania = false;

    for (int zestaw = 0; zestaw < 8; ++zestaw) {
        int obecnyKolor = 0; 
        if (aktualnyTryb == 2) obecnyKolor = zestaw % 2; 
        else if (aktualnyTryb == 4) obecnyKolor = zestaw % 4; 
        
        for (int w = 1; w <= 13; ++w) {
            talia.push_back({w, obecnyKolor, false});
        }
    }
    tasuj();
    rozdaj_poczatkowe();
    zegar.restart();
}

void GraPajak::inicjujAnimacjeWygranej(int wybranyStyl) {
    dzwiekWygrana.play();
    obiektyGry.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(-10.f, 10.f);
    std::uniform_real_distribution<float> disY(-15.f, -2.f);
    
    float szer = 70.f; float wys = 100.f;
    float offX = 90.f; float offY = 20.f;
    float startY = 200.f;
    float drawSzer = (wybranyStyl == 1) ? 86.f : szer;
    float drawWys  = (wybranyStyl == 1) ? 120.f : wys;
    float shiftX   = (wybranyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (wybranyStyl == 1) ? -10.f : 0.f;

    for (int i = 0; i < 10; ++i) {
        for (size_t j = 0; j < stosy[i].size(); ++j) {
            sf::Sprite spr;
            if (stosy[i][j].odkryta) {
                if (wybranyStyl == 1) spr.setTexture(texAwers[stosy[i][j].kolor][stosy[i][j].wartosc]);
                else spr.setTexture(texAwers2[stosy[i][j].kolor][stosy[i][j].wartosc]);
            } else {
                if (wybranyStyl == 1) spr.setTexture(texRewers);
                else spr.setTexture(texRewers2);
            }
            sf::Vector2f pos(50.f + i * offX + shiftX, startY + j * offY + shiftY);
            spr.setScale(drawSzer / spr.getLocalBounds().width, drawWys / spr.getLocalBounds().height);
            
            sf::Vector2f vel(disX(gen) * 60.f, disY(gen) * 60.f); // px/s
            float rotVel = (disX(gen) > 0 ? 1 : -1) * (100.f + std::abs(disY(gen)*10.f)); // stopnie/s
            
            obiektyGry.push_back(std::make_unique<AnimowanaKartaWygrana>(spr, pos, vel, rotVel));
        }
        stosy[i].clear();
    }
}

void GraPajak::aktualizujObiekty(float dt) {
    if (stan == ANIMACJA_ROZDAWANIA) {
        bool animacjeRozdaniaAktywne = false;
        
        for (auto it = obiektyGry.begin(); it != obiektyGry.end(); ) {
            auto animRozd = dynamic_cast<AnimowanaKartaRozdania*>(it->get());
            if (animRozd) {
                animacjeRozdaniaAktywne = true;
                bool done = animRozd->aktualizuj(dt);
                if (done) {
                    stosy[animRozd->celKolumna].push_back(animRozd->karta);
                    it = obiektyGry.erase(it);
                } else {
                    ++it;
                }
            } else {
                (*it)->aktualizuj(dt);
                ++it;
            }
        }
        
        if (!animacjeRozdaniaAktywne && obiektyGry.empty()) {
            sprawdz_sekwencje();
            stan = W_TRAKCIE;
            sprawdz_stan_gry();
        }
    } else {
        // Zwykla aktualizacja np. dla wygranej
        for (auto& obj : obiektyGry) {
            obj->aktualizuj(dt);
        }
    }
}

void GraPajak::przyspieszRozdawanie() {
    if (stan != ANIMACJA_ROZDAWANIA) return;
    
    // Wypycha wszystkie resztki z kontenera na plansze od razu, używając dynamic_cast
    for (auto it = obiektyGry.begin(); it != obiektyGry.end(); ) {
        if (auto animRozd = dynamic_cast<AnimowanaKartaRozdania*>(it->get())) {
            stosy[animRozd->celKolumna].push_back(animRozd->karta);
            it = obiektyGry.erase(it);
        } else {
            ++it;
        }
    }
    
    sprawdz_sekwencje();
    stan = W_TRAKCIE;
    sprawdz_stan_gry();
}

void GraPajak::znajdzWskazowke() {
    wyczyscWskazowke();
    if (stan != W_TRAKCIE) return;

    for (int i = 0; i < 10; ++i) {
        if (stosy[i].empty()) continue;
        for (int j = (int)stosy[i].size() - 1; j >= 0; --j) {
            if (!stosy[i][j].odkryta) continue;

            bool legalnyStos = true;
            for (size_t k = j; k < stosy[i].size() - 1; ++k) {
                if (stosy[i][k].wartosc != stosy[i][k+1].wartosc + 1 ||
                    stosy[i][k].kolor != stosy[i][k+1].kolor) {
                    legalnyStos = false; 
                    break;
                }
            }
            if (!legalnyStos) continue;

            for (int d = 0; d < 10; ++d) {
                if (i == d) continue;
                bool moznaPolozyc = false;
                if (stosy[d].empty()) moznaPolozyc = true;
                else if (stosy[d].back().wartosc == stosy[i][j].wartosc + 1) moznaPolozyc = true;

                if (moznaPolozyc) {
                    hintZ_Kol = i; 
                    hintZ_Rzad = j; 
                    hintDo_Kol = d;
                    pokazWskazowke = true;
                    return; 
                }
            }
        }
    }
}

// --- OBSŁUGA PLIKÓW I RANKINGU ---
std::vector<std::string> GraPajak::wczytajProfile() {
    std::vector<std::string> profile;
    std::ifstream plik("profile.txt");
    std::string nick;
    while (plik >> nick) { profile.push_back(nick); }
    return profile;
}

void GraPajak::dodajProfil(const std::string& nick) {
    auto profile = wczytajProfile();
    if (std::find(profile.begin(), profile.end(), nick) == profile.end()) {
        if (profile.size() < 20) { 
            std::ofstream plik("profile.txt", std::ios::app);
            plik << nick << "\n";
        }
    }
}

void GraPajak::zapiszWynikRanking(int punkty) {
    if (punkty > 95000) return; // Zablokowanie wyników "DEV: WYGRAJ"
    std::string nazwaPliku = "ranking_" + std::to_string(aktualnyTryb) + ".txt";
    std::ofstream plik(nazwaPliku, std::ios::app);
    plik << aktualnyGracz << " " << punkty << "\n";
}

void GraPajak::wyczyscRanking(int tryb) {
    std::string nazwaPliku = "ranking_" + std::to_string(tryb) + ".txt";
    std::ofstream plik(nazwaPliku, std::ios::trunc);
    plik.close();
}

std::vector<WynikGracza> GraPajak::pobierzRanking(int tryb) {
    std::string nazwaPliku = "ranking_" + std::to_string(tryb) + ".txt";
    std::ifstream plik(nazwaPliku);
    std::vector<WynikGracza> tabela;
    std::string n; int p;

    while (plik >> n >> p) tabela.push_back({n, p});

    std::sort(tabela.begin(), tabela.end(), [](const WynikGracza& a, const WynikGracza& b) {
        return a.punkty > b.punkty;
    });
    return tabela;
}
