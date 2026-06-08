#include <iostream>
#include <SFML/Graphics.hpp>
#include "GraPajak.h"

// Pomocnicza funkcja zwracająca kolor dla stylu minimalistycznego na potrzeby menu/logiki
sf::Color pobierzKolorKarty(int wartosc) {
    switch (wartosc % 4) {
    case 0: return sf::Color(220, 53, 69);    // Elegancki czerwony
    case 1: return sf::Color(40, 167, 69);    // Żywy zielony
    case 2: return sf::Color(111, 66, 193);   // Głęboki fioletowy
    default: return sf::Color(0, 123, 255);   // Jasny niebieski
    }
}

// Pomocnicza funkcja do łatwego sprawdzania kolizji myszy z przyciskami
bool czyKliknieto(sf::FloatRect bounds, sf::Vector2f klik) {
    return bounds.contains(klik);
}

// Maciej implementuje metodę rysuj zdefiniowaną w GraPajak.h z uwzględnieniem stylów personalizacji
void GraPajak::rysuj(sf::RenderWindow& window, sf::Font& font, int wybranyStyl) {
    // ROZMIAR LOGICZNY (do odstępów i klikania) - ZOSTAJE 70x100!
    float szer = 70.f; float wys = 100.f;
    float offX = 90.f; float offY = 20.f;
    float startY = 200.f;

    // --- KOREKTA WIZUALNA DLA STYLU PIKSELOWEGO ---
    // Niweluje przezroczysty margines w plikach PNG od Kenney'a
    float drawSzer = (wybranyStyl == 1) ? 86.f : szer;
    float drawWys  = (wybranyStyl == 1) ? 120.f : wys;
    float shiftX   = (wybranyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (wybranyStyl == 1) ? -10.f : 0.f;

    // Rysowanie tła stołu na samym spodzie
    window.draw(spriteTlo);

    // Sekcja HUD: Czas i Wynik
    int minuty = (int)czas_gry / 60;
    int sekundy = (int)czas_gry % 60;
    std::string infoStr = "Czas: " + std::to_string(minuty) + ":" + (sekundy < 10 ? "0" : "") + std::to_string(sekundy) +
                          " | Wynik: " + std::to_string(obliczWynik());
    sf::Text txtInfo(infoStr, font, 24);
    txtInfo.setPosition(50.f, 10.f);
    txtInfo.setFillColor(sf::Color::White);
    window.draw(txtInfo);

    // TALIA POMOCNICZA (Zależny rewers w prawym górnym rogu)
    if (!talia.empty()) {
        sf::Sprite stockSprite;
        if (wybranyStyl == 1) {
            stockSprite.setTexture(texRewers);
        } else {
            stockSprite.setTexture(texRewers2);
        }

        stockSprite.setPosition(850.f + shiftX, 50.f + shiftY);
        float scaleX = drawSzer / stockSprite.getLocalBounds().width;
        float scaleY = drawWys / stockSprite.getLocalBounds().height;
        stockSprite.setScale(scaleX, scaleY);
        window.draw(stockSprite);



        sf::Text txt("TALIA", font, 14);
        txt.setPosition(860.f, 90.f);
        txt.setFillColor(sf::Color::White);
        window.draw(txt);
    }

    // Przycisk COFNIJ
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

    // Przycisk DEV WYGRAJ
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

    // Obszar ukończonych sekwencji (lewy górny róg)
    for (int i = 0; i < zebrane_krolestwa; ++i) {
        sf::RectangleShape found(sf::Vector2f(szer, wys));
        found.setPosition(50.f + i * 25.f, 50.f);
        found.setFillColor(sf::Color::White);
        found.setOutlineThickness(2.f);
        found.setOutlineColor(sf::Color(255, 215, 0));
        window.draw(found);
    }

    // Pętla rysowania kart w 10 kolumnach roboczych
    for (int i = 0; i < 10; ++i) {
        if (stosy[i].empty()) {
            sf::RectangleShape emptyCol(sf::Vector2f(szer, wys));
            emptyCol.setPosition(50.f + i * offX, startY);
            emptyCol.setFillColor(sf::Color(30, 100, 30));
            window.draw(emptyCol);
        }

        for (size_t j = 0; j < stosy[i].size(); ++j) {
            if (isDragging && i == dragCol && j >= (size_t)dragRow) continue;

            sf::Sprite kartaSprite;
            if (stosy[i][j].odkryta) {
                if (wybranyStyl == 1) {
                    kartaSprite.setTexture(texAwers[stosy[i][j].wartosc]);
                } else {
                    kartaSprite.setTexture(texAwers2[stosy[i][j].wartosc]);
                }
            } else {
                if (wybranyStyl == 1) {
                    kartaSprite.setTexture(texRewers);
                } else {
                    kartaSprite.setTexture(texRewers2);
                }
            }

            // Aplikowanie powiększonego rozmiaru i przesunięcia centrującego
            kartaSprite.setPosition(50.f + i * offX + shiftX, startY + j * offY + shiftY);
            float scaleX = drawSzer / kartaSprite.getLocalBounds().width;
            float scaleY = drawWys / kartaSprite.getLocalBounds().height;
            kartaSprite.setScale(scaleX, scaleY);
            window.draw(kartaSprite);
        }
    }

    // Rysowanie przeciąganego stosu kart (Drag & Drop)
    if (isDragging) {
        for (size_t k = (size_t)dragRow; k < stosy[dragCol].size(); ++k) {
            float x = mousePos.x - dragOffset.x;
            float y = mousePos.y - dragOffset.y + ((k - dragRow) * offY);

            sf::Sprite dSprite;
            if (wybranyStyl == 1) {
                dSprite.setTexture(texAwers[stosy[dragCol][k].wartosc]);
            } else {
                dSprite.setTexture(texAwers2[stosy[dragCol][k].wartosc]);
            }

            dSprite.setPosition(x + shiftX, y + shiftY);
            float scaleX = drawSzer / dSprite.getLocalBounds().width;
            float scaleY = drawWys / dSprite.getLocalBounds().height;
            dSprite.setScale(scaleX, scaleY);

            window.draw(dSprite);
        }
    }

    // Warstwa końca rozgrywki (Zwycięstwo / Przegrana)
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

// --- GŁÓWNA PĘTLA GRY Z MASZYNĄ STANÓW ---
int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Pasjans Pajak SFML");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie udalo sie zaladowac czcionki arial.ttf" << std::endl;
        return -1;
    }

    GraPajak gra;
    enum StanAplikacji { MENU_GLOWNE, MENU_PERSONALIZACJI, ROZGRYWKA };
    StanAplikacji obecnyStan = MENU_GLOWNE;
    int wybranyStyl = 1; // 1 = Pixel, 2 = Karo

    // Punkty kolizji przycisków Menu Głównego
    sf::FloatRect btn1Kolor(350.f, 280.f, 300.f, 50.f);
    sf::FloatRect btn2Kolory(350.f, 360.f, 300.f, 50.f);
    sf::FloatRect btn4Kolory(350.f, 440.f, 300.f, 50.f);
    sf::FloatRect btnPersonalizacja(350.f, 520.f, 300.f, 50.f);

    // Punkty kolizji przycisków Menu Personalizacji
    sf::FloatRect btnStylPixel(100.f, 250.f, 250.f, 50.f);
    sf::FloatRect btnStylKaro(100.f, 330.f, 250.f, 50.f);
    sf::FloatRect btnWroc(100.f, 500.f, 250.f, 50.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f myszPos((float)event.mouseButton.x, (float)event.mouseButton.y);

                if (obecnyStan == MENU_GLOWNE) {
                    if (czyKliknieto(btn1Kolor, myszPos)) {
                        obecnyStan = ROZGRYWKA;
                    }
                    else if (czyKliknieto(btnPersonalizacja, myszPos)) {
                        obecnyStan = MENU_PERSONALIZACJI;
                    }
                }
                else if (obecnyStan == MENU_PERSONALIZACJI) {
                    if (czyKliknieto(btnStylPixel, myszPos)) {
                        wybranyStyl = 1;
                    }
                    else if (czyKliknieto(btnStylKaro, myszPos)) {
                        wybranyStyl = 2;
                    }
                    else if (czyKliknieto(btnWroc, myszPos)) {
                        obecnyStan = MENU_GLOWNE;
                    }
                }
                else if (obecnyStan == ROZGRYWKA) {
                    gra.obsluzKlikniecie(myszPos);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (obecnyStan == ROZGRYWKA) gra.obsluzPuszczenie();
            }

            if (event.type == sf::Event::MouseMoved) {
                if (obecnyStan == ROZGRYWKA)
                    gra.aktualizujMysz(sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y));
            }
        }

        window.clear(sf::Color(24, 76, 40));

        if (obecnyStan == MENU_GLOWNE) {
            sf::Text txtTytul("PASJANS PAJAK", font, 50);
            txtTytul.setStyle(sf::Text::Bold);
            txtTytul.setFillColor(sf::Color::Yellow);
            txtTytul.setPosition(500.f - txtTytul.getLocalBounds().width/2.f, 100.f);
            window.draw(txtTytul);

            std::pair<sf::FloatRect, std::string> przyciskiMenu[] = {
                {btn1Kolor, "Graj: 1 Kolor"}, {btn2Kolory, "Graj: 2 Kolory"},
                {btn4Kolory, "Graj: 4 Kolory"}, {btnPersonalizacja, "Personalizacja"}
            };

            for (const auto& p : przyciskiMenu) {
                sf::RectangleShape rect(sf::Vector2f(p.first.width, p.first.height));
                rect.setPosition(p.first.left, p.first.top);
                rect.setFillColor(sf::Color(45, 45, 45));
                rect.setOutlineThickness(2.f);
                rect.setOutlineColor(sf::Color::White);
                window.draw(rect);

                sf::Text t(p.second, font, 20);
                t.setPosition(p.first.left + (p.first.width/2.f) - t.getLocalBounds().width/2.f, p.first.top + 12.f);
                t.setFillColor(sf::Color::White);
                window.draw(t);
            }
        }
        else if (obecnyStan == MENU_PERSONALIZACJI) {
            sf::Text txtPers("PERSONALIZACJA", font, 40);
            txtPers.setFillColor(sf::Color::White);
            txtPers.setPosition(100.f, 80.f);
            window.draw(txtPers);

            std::pair<sf::FloatRect, std::string> przyciskiPers[] = {
                {btnStylPixel, "Styl: Pikselowy"}, {btnStylKaro, "Styl: Nowe Karty (Karo)"}, {btnWroc, "Wstecz"}
            };

            for (const auto& p : przyciskiPers) {
                sf::RectangleShape rect(sf::Vector2f(p.first.width, p.first.height));
                rect.setPosition(p.first.left, p.first.top);

                if ((p.first == btnStylPixel && wybranyStyl == 1) || (p.first == btnStylKaro && wybranyStyl == 2))
                    rect.setFillColor(sf::Color(0, 150, 0));
                else
                    rect.setFillColor(sf::Color(45, 45, 45));

                rect.setOutlineThickness(2.f);
                rect.setOutlineColor(sf::Color::White);
                window.draw(rect);

                sf::Text t(p.second, font, 18);
                t.setPosition(p.first.left + (p.first.width/2.f) - t.getLocalBounds().width/2.f, p.first.top + 12.f);
                t.setFillColor(sf::Color::White);
                window.draw(t);
            }

            sf::RectangleShape previewBox(sf::Vector2f(450.f, 400.f));
            previewBox.setPosition(450.f, 200.f);
            previewBox.setFillColor(sf::Color(15, 45, 25));
            previewBox.setOutlineThickness(3.f);
            previewBox.setOutlineColor(sf::Color::Yellow);
            window.draw(previewBox);

            sf::Text txtPodglad("PODGLAD KARTY (KROL)", font, 20);
            txtPodglad.setPosition(470.f, 220.f);
            txtPodglad.setFillColor(sf::Color::Yellow);
            window.draw(txtPodglad);

            if (wybranyStyl == 1) {
                sf::RectangleShape cardSample(sf::Vector2f(120.f, 170.f));
                cardSample.setPosition(615.f, 320.f);
                cardSample.setFillColor(sf::Color::White);
                cardSample.setOutlineThickness(2.f);
                cardSample.setOutlineColor(sf::Color::Black);
                window.draw(cardSample);

                sf::Text txtK("K", font, 40);
                txtK.setPosition(630.f, 330.f);
                txtK.setFillColor(sf::Color::Black);
                window.draw(txtK);

                sf::Text txtDesc("(Klasyczny styl pixelart z zielonym tylem)", font, 14);
                txtDesc.setPosition(535.f, 520.f);
                txtDesc.setFillColor(sf::Color::White);
                window.draw(txtDesc);
            }
            else {
                sf::RectangleShape cardSample(sf::Vector2f(120.f, 170.f));
                cardSample.setPosition(615.f, 320.f);
                cardSample.setFillColor(sf::Color::White);
                cardSample.setOutlineThickness(2.f);
                cardSample.setOutlineColor(sf::Color(220, 53, 53));
                window.draw(cardSample);

                sf::Text txtK("K", font, 50);
                txtK.setStyle(sf::Text::Bold);
                txtK.setPosition(660.f - txtK.getLocalBounds().width/2.f, 370.f);
                txtK.setFillColor(sf::Color(220, 53, 53));
                window.draw(txtK);

                sf::Text txtDesc("(Wygladzony styl wektorowy z niebieskim tylem)", font, 14);
                txtDesc.setPosition(525.f, 520.f);
                txtDesc.setFillColor(sf::Color::White);
                window.draw(txtDesc);
            }
        }
        else if (obecnyStan == ROZGRYWKA) {
            gra.aktualizujCzas();
            gra.rysuj(window, font, wybranyStyl);
        }

        window.display();
    }
    return 0;
}