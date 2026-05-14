#include <iostream>
#include <SFML/Graphics.hpp>
#include "GraPajak.h"

// Maciej implementuje metodę rysuj zdefiniowaną w GraPajak.h
void GraPajak::rysuj(sf::RenderWindow& window, sf::Font& font) {
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

// --- GLOWNA PETLA GRY ---
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
