#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>

// --- STRUKTURY ---
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

// --- LOGIKA GRY ---
class GraPajak {
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

    // Przeciąganie myszką
    bool isDragging = false;
    int dragCol = -1;
    int dragRow = -1;
    sf::Vector2f dragOffset;
    sf::Vector2f mousePos;

public:
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
        // Każda sekunda zabiera 50 punktów z puli 100 000
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
        if (zebrane_krolestwa == 8) stan = WYGRANA;
        else if (!czy_sa_ruchy()) stan = PRZEGRANA;
        else stan = W_TRAKCIE;
    }

    void obsluzKlikniecie(sf::Vector2f klik) {
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

    void obsluzPuszczenie() {
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

    void aktualizujMysz(sf::Vector2f pos) { mousePos = pos; }

    void rysuj(sf::RenderWindow& window, sf::Font& font) {
        float szer = 70.f; float wys = 100.f;
        float offX = 90.f; float offY = 20.f;
        float startY = 200.f;

        int minuty = (int)czas_gry / 60;
        int sekundy = (int)czas_gry % 60;
        std::string infoStr = "Czas: " + std::to_string(minuty) + ":" + (sekundy < 10 ? "0" : "") + std::to_string(sekundy) +
                              " | Wynik: " + std::to_string(obliczWynik());
        sf::Text txtInfo(infoStr, font, 24);
        txtInfo.setPosition(50.f, 10.f);
        txtInfo.setFillColor(sf::Color::White);
        window.draw(txtInfo);

        if (!talia.empty()) {
            sf::RectangleShape stock(sf::Vector2f(szer, wys));
            stock.setPosition(850.f, 50.f);
            stock.setFillColor(sf::Color(100, 149, 237));
            stock.setOutlineThickness(2.f);
            stock.setOutlineColor(sf::Color::Black);
            window.draw(stock);
            sf::Text txt("TALIA", font, 14);
            txt.setPosition(860.f, 90.f);
            txt.setFillColor(sf::Color::White);
            window.draw(txt);
        }

        sf::RectangleShape cofnijRect(sf::Vector2f(80.f, 40.f));
        cofnijRect.setPosition(750.f, 50.f);
        cofnijRect.setFillColor(historia.empty() ? sf::Color(150, 150, 150) : sf::Color(200, 50, 50));
        cofnijRect.setOutlineThickness(2.f);
        cofnijRect.setOutlineColor(sf::Color::Black);
        window.draw(cofnijRect);
        sf::Text txtCofnij("COFNIJ", font, 16);
        txtCofnij.setPosition(760.f, 60.f);
        txtCofnij.setFillColor(sf::Color::White);
        window.draw(txtCofnij);

        sf::RectangleShape devRect(sf::Vector2f(120.f, 40.f));
        devRect.setPosition(850.f, 700.f);
        devRect.setFillColor(sf::Color(128, 0, 128));
        devRect.setOutlineThickness(2.f);
        devRect.setOutlineColor(sf::Color::Black);
        window.draw(devRect);
        sf::Text txtDev("DEV: WYGRAJ", font, 14);
        txtDev.setPosition(855.f, 710.f);
        txtDev.setFillColor(sf::Color::White);
        window.draw(txtDev);

        for (int i = 0; i < zebrane_krolestwa; ++i) {
            sf::RectangleShape found(sf::Vector2f(szer, wys));
            found.setPosition(50.f + i * 25.f, 50.f);
            found.setFillColor(sf::Color::White);
            found.setOutlineThickness(2.f);
            found.setOutlineColor(sf::Color(255, 215, 0));
            window.draw(found);
        }

        for (int i = 0; i < 10; ++i) {
            if (stosy[i].empty()) {
                sf::RectangleShape emptyCol(sf::Vector2f(szer, wys));
                emptyCol.setPosition(50.f + i * offX, startY);
                emptyCol.setFillColor(sf::Color(30, 100, 30));
                window.draw(emptyCol);
            }
            for (size_t j = 0; j < stosy[i].size(); ++j) {
                if (isDragging && i == dragCol && j >= (size_t)dragRow) continue;
                sf::RectangleShape rect(sf::Vector2f(szer, wys));
                rect.setPosition(50.f + i * offX, startY + j * offY);
                rect.setOutlineThickness(1.f);
                rect.setOutlineColor(sf::Color::Black);
                rect.setFillColor(stosy[i][j].odkryta ? sf::Color::White : sf::Color(100, 149, 237));
                window.draw(rect);
                if (stosy[i][j].odkryta) {
                    sf::Text val(stosy[i][j].tekst(), font, 20);
                    val.setPosition(55.f + i * offX, startY + j * offY + 5.f);
                    val.setFillColor(sf::Color::Black);
                    window.draw(val);
                }
            }
        }

        if (isDragging) {
            for (size_t k = (size_t)dragRow; k < stosy[dragCol].size(); ++k) {
                float x = mousePos.x - dragOffset.x;
                float y = mousePos.y - dragOffset.y + ((k - dragRow) * offY);
                sf::RectangleShape dRect(sf::Vector2f(szer, wys));
                dRect.setPosition(x, y);
                dRect.setFillColor(sf::Color::White);
                dRect.setOutlineThickness(2.f);
                dRect.setOutlineColor(sf::Color::Blue);
                window.draw(dRect);
                sf::Text dVal(stosy[dragCol][k].tekst(), font, 20);
                dVal.setPosition(x + 5.f, y + 5.f);
                dVal.setFillColor(sf::Color::Black);
                window.draw(dVal);
            }
        }

        if (stan != W_TRAKCIE) {
            sf::RectangleShape overlay(sf::Vector2f(1000.f, 800.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            sf::Text msg(stan == WYGRANA ? "ZWYCIESTWO!" : "BRAK RUCHOW!", font, 70);
            msg.setFillColor(stan == WYGRANA ? sf::Color::Yellow : sf::Color::Red);
            sf::FloatRect textRect = msg.getLocalBounds();
            msg.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
            msg.setPosition(500.f, 350.f);
            window.draw(msg);

            std::string statsStr = "Czas: " + std::to_string(minuty) + ":" + (sekundy < 10 ? "0" : "") + std::to_string(sekundy) +
                                   "\nWYNIK KONCOWY: " + std::to_string(obliczWynik());
            sf::Text statsMsg(statsStr, font, 35);
            statsMsg.setFillColor(sf::Color::White);
            statsMsg.setOutlineColor(sf::Color::Black);
            statsMsg.setOutlineThickness(2.f);
            sf::FloatRect sRect = statsMsg.getLocalBounds();
            statsMsg.setOrigin(sRect.left + sRect.width/2.0f, sRect.top + sRect.height/2.0f);
            statsMsg.setPosition(500.f, 500.f);
            window.draw(statsMsg);
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Pasjans Pajak SFML");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie udalo sie zaladowac czcionki arial.ttf" << std::endl;
        return -1;
    }
    GraPajak gra;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                gra.obsluzKlikniecie(sf::Vector2f((float)event.mouseButton.x, (float)event.mouseButton.y));
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                gra.obsluzPuszczenie();
            if (event.type == sf::Event::MouseMoved)
                gra.aktualizujMysz(sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y));
        }
        gra.aktualizujCzas();
        window.clear(sf::Color(34, 139, 34));
        gra.rysuj(window, font);
        window.display();
    }
    return 0;
}
