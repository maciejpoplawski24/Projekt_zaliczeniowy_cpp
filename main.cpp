#include <iostream>
#include <SFML/Graphics.hpp>
#include "GraPajak.h"

bool czyKliknieto(sf::FloatRect bounds, sf::Vector2f klik) {
    return bounds.contains(klik);
}

void GraPajak::rysuj(sf::RenderWindow& window, sf::Font& font, int wybranyStyl, int wybraneTlo) {
    float szer = 70.f; float wys = 100.f;
    float offX = 90.f; float offY = 20.f;
    float startY = 200.f;

    float drawSzer = (wybranyStyl == 1) ? 86.f : szer;
    float drawWys  = (wybranyStyl == 1) ? 120.f : wys;
    float shiftX   = (wybranyStyl == 1) ? -8.f : 0.f;
    float shiftY   = (wybranyStyl == 1) ? -10.f : 0.f;

    if (wybraneTlo == 1) spriteTlo.setTexture(texTlo1);
    else if (wybraneTlo == 2) spriteTlo.setTexture(texTlo2);
    else spriteTlo.setTexture(texTlo3);

    float scaleX = 1000.f / spriteTlo.getLocalBounds().width;
    float scaleY = 800.f / spriteTlo.getLocalBounds().height;
    spriteTlo.setScale(scaleX, scaleY);
    window.draw(spriteTlo);

    int minuty = (int)czas_gry / 60;
    int sekundy = (int)czas_gry % 60;
    std::string infoStr = "Gracz: " + aktualnyGracz + " | Tryb: " + std::to_string(aktualnyTryb) + " Kolor(y)" + 
                          " | Czas: " + std::to_string(minuty) + ":" + (sekundy < 10 ? "0" : "") + std::to_string(sekundy) +
                          " | Wynik: " + std::to_string(obliczWynik());
    sf::Text txtInfo(infoStr, font, 24);
    txtInfo.setPosition(50.f, 10.f);
    txtInfo.setFillColor(sf::Color::White);
    window.draw(txtInfo);

    if (!talia.empty()) {
        sf::Sprite stockSprite;
        if (wybranyStyl == 1) stockSprite.setTexture(texRewers);
        else stockSprite.setTexture(texRewers2);
        stockSprite.setPosition(850.f + shiftX, 50.f + shiftY);
        stockSprite.setScale(drawSzer / stockSprite.getLocalBounds().width, drawWys / stockSprite.getLocalBounds().height);
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
    window.draw(txtCofnij);

    // Przycisk PODPOWIEDŹ
    sf::RectangleShape btnHint(sf::Vector2f(100.f, 30.f));
    btnHint.setPosition(740.f, 100.f);
    btnHint.setFillColor(sf::Color(0, 100, 200)); 
    btnHint.setOutlineThickness(2.f); 
    btnHint.setOutlineColor(sf::Color::Black);
    window.draw(btnHint);
    sf::Text txtHint("PODPOWIEDZ", font, 12);
    txtHint.setPosition(748.f, 108.f); 
    txtHint.setFillColor(sf::Color::White);
    window.draw(txtHint);

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
            emptyCol.setFillColor(sf::Color(30, 100, 30, 150));
            window.draw(emptyCol);
        }
        for (size_t j = 0; j < stosy[i].size(); ++j) {
            if (isDragging && i == dragCol && j >= (size_t)dragRow) continue;
            sf::Sprite kartaSprite;
            if (stosy[i][j].odkryta) {
                if (wybranyStyl == 1) kartaSprite.setTexture(texAwers[stosy[i][j].kolor][stosy[i][j].wartosc]);
                else kartaSprite.setTexture(texAwers2[stosy[i][j].kolor][stosy[i][j].wartosc]);
            } else { 
                if (wybranyStyl == 1) kartaSprite.setTexture(texRewers);
                else kartaSprite.setTexture(texRewers2);
            }
            kartaSprite.setPosition(50.f + i * offX + shiftX, startY + j * offY + shiftY);
            kartaSprite.setScale(drawSzer / kartaSprite.getLocalBounds().width, drawWys / kartaSprite.getLocalBounds().height);
            window.draw(kartaSprite);
        }
    }

    if (isDragging) {
        for (size_t k = (size_t)dragRow; k < stosy[dragCol].size(); ++k) {
            float x = mousePos.x - dragOffset.x;
            float y = mousePos.y - dragOffset.y + ((k - dragRow) * offY);
            sf::Sprite dSprite;
            if (wybranyStyl == 1) dSprite.setTexture(texAwers[stosy[dragCol][k].kolor][stosy[dragCol][k].wartosc]);
            else dSprite.setTexture(texAwers2[stosy[dragCol][k].kolor][stosy[dragCol][k].wartosc]);
            dSprite.setPosition(x + shiftX, y + shiftY);
            dSprite.setScale(drawSzer / dSprite.getLocalBounds().width, drawWys / dSprite.getLocalBounds().height);
            window.draw(dSprite);
        }
    }

    if (pokazWskazowke) {
        sf::RectangleShape rZ(sf::Vector2f(drawSzer, drawWys));
        rZ.setPosition(50.f + hintZ_Kol * offX + shiftX, startY + hintZ_Rzad * offY + shiftY);
        rZ.setFillColor(sf::Color::Transparent);
        rZ.setOutlineThickness(5.f);
        rZ.setOutlineColor(sf::Color::Yellow);
        window.draw(rZ);

        sf::RectangleShape rD(sf::Vector2f(drawSzer, drawWys));
        float celY = startY;
        if (!stosy[hintDo_Kol].empty()) celY = startY + (stosy[hintDo_Kol].size() - 1) * offY;
        rD.setPosition(50.f + hintDo_Kol * offX + shiftX, celY + shiftY);
        rD.setFillColor(sf::Color::Transparent);
        rD.setOutlineThickness(5.f);
        rD.setOutlineColor(sf::Color::Green);
        window.draw(rD);
    }

    // Animacje sa obslugiwane poza funkcja rysuj() przez aktualizujObiekty(dt) i osobne rysowanie

    if (stan == PRZEGRANA || stan == WYGRANA) {
        if (stan == WYGRANA && !ekranPodsumowania) {
            // Animacje sa obslugiwane poza funkcja rysuj()
            sf::Text txtPomin("Kliknij LPM, aby wyswietlic podsumowanie", font, 22);
            txtPomin.setFillColor(sf::Color(255, 255, 255, 200));
            sf::FloatRect pRect = txtPomin.getLocalBounds();
            txtPomin.setOrigin(pRect.left + pRect.width/2.0f, pRect.top + pRect.height/2.0f);
            txtPomin.setPosition(500.f, 750.f);
            window.draw(txtPomin);
        }
        else {
            sf::RectangleShape overlay(sf::Vector2f(1000.f, 800.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            if (stan == WYGRANA) {
                // Animacje sa obslugiwane poza funkcja rysuj()
            }

            sf::Text msg(stan == WYGRANA ? "ZWYCIESTWO!" : "BRAK RUCHOW!", font, 70);
            msg.setFillColor(stan == WYGRANA ? sf::Color::Yellow : sf::Color::Red);
            sf::FloatRect textRect = msg.getLocalBounds();
            msg.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
            msg.setPosition(500.f, 320.f);
            window.draw(msg);

            std::string statsStr = "Gracz: " + aktualnyGracz + " | Czas: " + std::to_string(minuty) + ":" + (sekundy < 10 ? "0" : "") + std::to_string(sekundy) +
                                  "\nWYNIK KONCOWY: " + std::to_string(obliczWynik());
            sf::Text statsMsg(statsStr, font, 35);
            statsMsg.setFillColor(sf::Color::White);
            sf::FloatRect sRect = statsMsg.getLocalBounds();
            statsMsg.setOrigin(sRect.left + sRect.width/2.0f, sRect.top + sRect.height/2.0f);
            statsMsg.setPosition(500.f, 460.f);
            window.draw(statsMsg);

            sf::Text txtPowrot("Kliknij LPM, aby powrocic do Menu Glownego", font, 22);
            txtPowrot.setFillColor(sf::Color(180, 180, 180));
            sf::FloatRect pRect2 = txtPowrot.getLocalBounds();
            txtPowrot.setOrigin(pRect2.left + pRect2.width/2.0f, pRect2.top + pRect2.height/2.0f);
            txtPowrot.setPosition(500.f, 580.f);
            window.draw(txtPowrot);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Pasjans Pajak SFML");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie udalo sie zaladowac czcionki arial.ttf\n";
        return -1;
    }
    
    GraPajak gra;
    enum StanAplikacji { MENU_GLOWNE, MENU_PERSONALIZACJI, WYBOR_GRACZA, RANKING, ROZGRYWKA };
    StanAplikacji obecnyStan = MENU_GLOWNE;
    
    int wybranyStyl = 1; 
    int wybraneTlo = 1;  
    std::string wpisywanyNick = "";
    int docelowyTrybGry = 1;
    int zakladkaRankingu = 1;
    std::vector<std::string> listaZapisanych = gra.wczytajProfile();

    sf::FloatRect btn1Kolor(350.f, 250.f, 300.f, 50.f);
    sf::FloatRect btn2Kolory(350.f, 330.f, 300.f, 50.f);
    sf::FloatRect btn4Kolory(350.f, 410.f, 300.f, 50.f);
    sf::FloatRect btnRanking(350.f, 490.f, 300.f, 50.f);
    sf::FloatRect btnPersonalizacja(350.f, 570.f, 300.f, 50.f);
    sf::FloatRect btnWroc(100.f, 700.f, 200.f, 45.f);
    sf::FloatRect btnWczytaj(700.f, 250.f, 200.f, 50.f);
    sf::FloatRect btnZapisz(850.f, 650.f, 120.f, 40.f);
    sf::Clock dtClock;

    sf::Color ciemnyZielony(0, 130, 0);
    sf::Color ciemnyCzerwony(140, 40, 40);
    sf::Color szary(45, 45, 45);

    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::TextEntered && obecnyStan == WYBOR_GRACZA) {
                if (event.text.unicode == '\b' && !wpisywanyNick.empty()) wpisywanyNick.pop_back();
                else if (event.text.unicode > 32 && event.text.unicode < 127 && wpisywanyNick.size() < 12) wpisywanyNick += static_cast<char>(event.text.unicode);
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f myszPos((float)event.mouseButton.x, (float)event.mouseButton.y);

                if (obecnyStan == MENU_GLOWNE) {
                    if (czyKliknieto(btn1Kolor, myszPos)) { docelowyTrybGry = 1; obecnyStan = WYBOR_GRACZA; wpisywanyNick = ""; }
                    else if (czyKliknieto(btn2Kolory, myszPos)) { docelowyTrybGry = 2; obecnyStan = WYBOR_GRACZA; wpisywanyNick = ""; }
                    else if (czyKliknieto(btn4Kolory, myszPos)) { docelowyTrybGry = 4; obecnyStan = WYBOR_GRACZA; wpisywanyNick = ""; }
                    else if (czyKliknieto(btnRanking, myszPos)) { obecnyStan = RANKING; zakladkaRankingu = 1; }
                    else if (czyKliknieto(btnPersonalizacja, myszPos)) obecnyStan = MENU_PERSONALIZACJI;
                    else if (czyKliknieto(btnWczytaj, myszPos)) { gra.wczytajGreZPliku("save.txt"); obecnyStan = ROZGRYWKA; }
                }
                else if (obecnyStan == WYBOR_GRACZA) {
                    if (czyKliknieto(btnWroc, myszPos)) obecnyStan = MENU_GLOWNE;
                    else if (czyKliknieto(sf::FloatRect(650.f, 150.f, 200.f, 50.f), myszPos) && !wpisywanyNick.empty()) {
                        gra.dodajProfil(wpisywanyNick); gra.aktualnyGracz = wpisywanyNick; gra.aktualnyTryb = docelowyTrybGry;
                        listaZapisanych = gra.wczytajProfile(); gra.resetuj(); obecnyStan = ROZGRYWKA;
                    }
                    for (size_t i = 0; i < listaZapisanych.size(); ++i) {
                        if (sf::FloatRect(100.f + (i % 4) * 200.f, 300.f + (i / 4) * 60.f, 180.f, 40.f).contains(myszPos)) {
                            gra.aktualnyGracz = listaZapisanych[i]; gra.aktualnyTryb = docelowyTrybGry;
                            gra.resetuj(); obecnyStan = ROZGRYWKA;
                        }
                    }
                }
                else if (obecnyStan == RANKING) {
                    if (czyKliknieto(btnWroc, myszPos)) obecnyStan = MENU_GLOWNE;
                    if (czyKliknieto(sf::FloatRect(200.f, 150.f, 180.f, 40.f), myszPos)) zakladkaRankingu = 1;
                    if (czyKliknieto(sf::FloatRect(410.f, 150.f, 180.f, 40.f), myszPos)) zakladkaRankingu = 2;
                    if (czyKliknieto(sf::FloatRect(620.f, 150.f, 180.f, 40.f), myszPos)) zakladkaRankingu = 4;
                    if (czyKliknieto(sf::FloatRect(700.f, 700.f, 200.f, 45.f), myszPos)) gra.wyczyscRanking(zakladkaRankingu);
                }
                else if (obecnyStan == MENU_PERSONALIZACJI) {
                    if (czyKliknieto(btnWroc, myszPos)) obecnyStan = MENU_GLOWNE;
                    if (czyKliknieto(sf::FloatRect(100.f, 160.f, 250.f, 45.f), myszPos)) wybranyStyl = 1;
                    if (czyKliknieto(sf::FloatRect(100.f, 215.f, 250.f, 45.f), myszPos)) wybranyStyl = 2;
                    if (czyKliknieto(sf::FloatRect(100.f, 340.f, 250.f, 45.f), myszPos)) wybraneTlo = 1;
                    if (czyKliknieto(sf::FloatRect(100.f, 395.f, 250.f, 45.f), myszPos)) wybraneTlo = 2;
                    if (czyKliknieto(sf::FloatRect(100.f, 450.f, 250.f, 45.f), myszPos)) wybraneTlo = 3;
                }
                else if (obecnyStan == ROZGRYWKA) {
                    if (czyKliknieto(sf::FloatRect(740.f, 100.f, 100.f, 30.f), myszPos)) {
                        gra.znajdzWskazowke();
                    }
                    else if (czyKliknieto(sf::FloatRect(850.f, 700.f, 120.f, 40.f), myszPos)) {
                        gra.zapisz_stan();
                        gra.stan = WYGRANA; 
                        gra.zapiszWynikRanking(gra.obliczWynik());
                        gra.wynikZapisany = true;
                        if (gra.obiektyGry.empty()) {
                            gra.inicjujAnimacjeWygranej(wybranyStyl);
                        }
                    }
                    else if (czyKliknieto(btnZapisz, myszPos)) {
                        gra.zapiszGreDoPliku("save.txt");
                    }
                    else if (gra.czyKoniecGry()) { 
                        if (gra.stan == WYGRANA && !gra.ekranPodsumowania) {
                            gra.ekranPodsumowania = true;
                        } else {
                            gra.resetuj(); 
                            obecnyStan = MENU_GLOWNE; 
                        }
                    }
                    else {
                        gra.obsluzKlikniecie(myszPos, wybranyStyl);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && obecnyStan == ROZGRYWKA) gra.obsluzPuszczenie(wybranyStyl);
            if (event.type == sf::Event::MouseMoved && obecnyStan == ROZGRYWKA) gra.aktualizujMysz(sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y));
        }

        window.clear(sf::Color(20, 50, 30));

        if (obecnyStan == MENU_GLOWNE) {
            sf::Text tTytul("PASJANS PAJAK", font, 60); 
            tTytul.setFillColor(sf::Color::Yellow); 
            tTytul.setPosition(500.f - tTytul.getLocalBounds().width/2.f, 80.f); 
            window.draw(tTytul);

            std::pair<sf::FloatRect, std::string> btns[] = {
                {btn1Kolor, "Graj: 1 Kolor"}, {btn2Kolory, "Graj: 2 Kolory"}, 
                {btn4Kolory, "Graj: 4 Kolory"}, {btnRanking, "Ranking"}, {btnPersonalizacja, "Personalizacja"}
            };
            for(auto& p : btns) {
                sf::RectangleShape r(sf::Vector2f(p.first.width, p.first.height)); 
                r.setPosition(p.first.left, p.first.top); 
                r.setFillColor(szary); 
                r.setOutlineThickness(2); 
                window.draw(r);

                sf::Text t(p.second, font, 20); 
                t.setPosition(p.first.left + (p.first.width - t.getLocalBounds().width)/2.f, p.first.top + 12.f); 
                window.draw(t);
            }

            sf::RectangleShape rW(sf::Vector2f(btnWczytaj.width, btnWczytaj.height));
            rW.setPosition(btnWczytaj.left, btnWczytaj.top);
            rW.setFillColor(ciemnyZielony);
            rW.setOutlineThickness(2.f);
            window.draw(rW);
            sf::Text tW("WCZYTAJ GRE", font, 18);
            tW.setPosition(btnWczytaj.left + (btnWczytaj.width - tW.getLocalBounds().width)/2.f, btnWczytaj.top + 15.f);
            window.draw(tW);
        }
        else if (obecnyStan == WYBOR_GRACZA) {
            sf::Text tQ("KTO GRA?", font, 40); 
            tQ.setPosition(100.f, 50.f); 
            tQ.setFillColor(sf::Color::Yellow);
            window.draw(tQ);

            sf::RectangleShape pole(sf::Vector2f(300.f, 50.f)); 
            pole.setPosition(300.f, 150.f); 
            pole.setFillColor(sf::Color(10, 10, 10)); 
            pole.setOutlineThickness(2.f);
            window.draw(pole);

            sf::Text tW(wpisywanyNick + (((int)(gra.czas_gry*2)%2==0)?"_":""), font, 24); 
            tW.setPosition(310.f, 160.f); 
            window.draw(tW);

            sf::RectangleShape bZ(sf::Vector2f(200.f, 50.f)); 
            bZ.setPosition(650.f, 150.f); 
            bZ.setFillColor(ciemnyZielony); 
            bZ.setOutlineThickness(2.f);
            window.draw(bZ);

            sf::Text tZ("ZAPISZ I GRAJ", font, 18); 
            tZ.setPosition(650.f + (200.f - tZ.getLocalBounds().width)/2.f, 165.f); 
            window.draw(tZ);

            sf::Text txtLub("Zapisane profile:", font, 20);
            txtLub.setPosition(100.f, 250.f); 
            window.draw(txtLub);

            for(size_t i=0; i<listaZapisanych.size(); ++i) {
                float x = 100.f + (i % 4) * 200.f;
                float y = 300.f + (i / 4) * 60.f;
                sf::RectangleShape rP(sf::Vector2f(180.f, 40.f)); 
                rP.setPosition(x, y); 
                rP.setFillColor(szary); 
                rP.setOutlineThickness(1.f);
                window.draw(rP);

                sf::Text tP(listaZapisanych[i], font, 18); 
                tP.setPosition(x + (180.f - tP.getLocalBounds().width)/2.f, y + 10.f); 
                window.draw(tP);
            }

            sf::RectangleShape rB(sf::Vector2f(btnWroc.width, btnWroc.height)); 
            rB.setPosition(btnWroc.left, btnWroc.top); 
            rB.setFillColor(ciemnyCzerwony); 
            rB.setOutlineThickness(2.f);
            window.draw(rB);

            sf::Text tB("Wstecz", font, 18); 
            tB.setPosition(btnWroc.left + (btnWroc.width - tB.getLocalBounds().width)/2.f, btnWroc.top + 12.f); 
            window.draw(tB);
        }
        else if (obecnyStan == RANKING) {
            sf::Text tR("TABELA WYNIKOW", font, 40); 
            tR.setPosition(500.f - tR.getLocalBounds().width/2.f, 50.f); 
            tR.setFillColor(sf::Color::Yellow);
            window.draw(tR);

            int tryby[] = {1, 2, 4}; 
            sf::FloatRect tabs[] = {sf::FloatRect(200,150,180,40), sf::FloatRect(410,150,180,40), sf::FloatRect(620,150,180,40)};
            for(int i=0; i<3; ++i) {
                sf::RectangleShape rT(sf::Vector2f(180,40)); 
                rT.setPosition(tabs[i].left, tabs[i].top); 
                rT.setFillColor(zakladkaRankingu == tryby[i] ? ciemnyZielony : szary); 
                rT.setOutlineThickness(2.f);
                window.draw(rT);

                sf::Text tT(std::to_string(tryby[i]) + " Kolor", font, 18); 
                tT.setPosition(tabs[i].left + (180.f - tT.getLocalBounds().width)/2.f, tabs[i].top + 10.f); 
                window.draw(tT);
            }

            sf::RectangleShape tloRankingu(sf::Vector2f(600.f, 400.f));
            tloRankingu.setPosition(200.f, 220.f); 
            tloRankingu.setFillColor(sf::Color(15, 15, 15, 200));
            tloRankingu.setOutlineThickness(2.f);
            tloRankingu.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(tloRankingu);

            auto rnk = gra.pobierzRanking(zakladkaRankingu);
            if (rnk.empty()) {
                sf::Text txtPuste("Brak wynikow. Badz pierwszy!", font, 24);
                txtPuste.setPosition(500.f - txtPuste.getLocalBounds().width/2.f, 400.f); 
                txtPuste.setFillColor(sf::Color(150, 150, 150));
                window.draw(txtPuste);
            } else {
                for(size_t i=0; i<rnk.size() && i<10; ++i) {
                    sf::Text tNum(std::to_string(i+1) + ".", font, 22); 
                    tNum.setPosition(230.f, 240.f + i*35.f); 
                    window.draw(tNum);

                    sf::Text tL(rnk[i].nick, font, 22); 
                    tL.setPosition(280.f, 240.f + i*35.f); 
                    tL.setFillColor(sf::Color::Cyan);
                    window.draw(tL);

                    sf::Text tS(std::to_string(rnk[i].punkty) + " pkt", font, 22); 
                    tS.setPosition(650.f, 240.f + i*35.f); 
                    tS.setFillColor(sf::Color::Yellow);
                    window.draw(tS);
                }
            }

            sf::RectangleShape rB(sf::Vector2f(btnWroc.width, btnWroc.height)); 
            rB.setPosition(btnWroc.left, btnWroc.top); 
            rB.setFillColor(ciemnyCzerwony); 
            rB.setOutlineThickness(2.f);
            window.draw(rB);

            sf::Text tB("Wstecz", font, 18); 
            tB.setPosition(btnWroc.left + (btnWroc.width - tB.getLocalBounds().width)/2.f, btnWroc.top + 12.f); 
            window.draw(tB);

            sf::FloatRect btnCzysc(700.f, btnWroc.top, 200.f, 45.f);
            sf::RectangleShape rC(sf::Vector2f(btnCzysc.width, btnCzysc.height));
            rC.setPosition(btnCzysc.left, btnCzysc.top);
            rC.setFillColor(ciemnyCzerwony);
            rC.setOutlineThickness(2.f);
            window.draw(rC);

            sf::Text tC("Wyczysc tabele", font, 18);
            tC.setPosition(btnCzysc.left + (btnCzysc.width - tC.getLocalBounds().width)/2.f, btnCzysc.top + 12.f);
            window.draw(tC);
        }
        else if (obecnyStan == MENU_PERSONALIZACJI) {
            sf::Text tP("PERSONALIZACJA", font, 40); 
            tP.setPosition(100.f, 40.f); 
            window.draw(tP);

            sf::Text tT("WYBOR TALII:", font, 20); 
            tT.setPosition(100.f, 120.f); 
            tT.setFillColor(sf::Color::Yellow); 
            window.draw(tT);

            sf::RectangleShape rS1(sf::Vector2f(250.f, 45.f)); 
            rS1.setPosition(100.f, 160.f); 
            rS1.setFillColor(wybranyStyl == 1 ? ciemnyZielony : szary); 
            rS1.setOutlineThickness(2.f);
            window.draw(rS1);
            sf::Text tS1("Styl: Pikselowy", font, 18); 
            tS1.setPosition(100.f + (250.f - tS1.getLocalBounds().width)/2.f, 172.f); 
            window.draw(tS1);

            sf::RectangleShape rS2(sf::Vector2f(250.f, 45.f)); 
            rS2.setPosition(100.f, 215.f); 
            rS2.setFillColor(wybranyStyl == 2 ? ciemnyZielony : szary); 
            rS2.setOutlineThickness(2.f);
            window.draw(rS2);
            sf::Text tS2("Styl: Wektorowy", font, 18); 
            tS2.setPosition(100.f + (250.f - tS2.getLocalBounds().width)/2.f, 227.f); 
            window.draw(tS2);

            sf::Text tSt("WYBOR STOLU:", font, 20); 
            tSt.setPosition(100.f, 300.f); 
            tSt.setFillColor(sf::Color::Yellow); 
            window.draw(tSt);

            sf::RectangleShape rT1(sf::Vector2f(250.f, 45.f)); 
            rT1.setPosition(100.f, 340.f); 
            rT1.setFillColor(wybraneTlo == 1 ? ciemnyZielony : szary); 
            rT1.setOutlineThickness(2.f);
            window.draw(rT1);
            sf::Text tT1("Stol: Drewno", font, 18); 
            tT1.setPosition(100.f + (250.f - tT1.getLocalBounds().width)/2.f, 352.f); 
            window.draw(tT1);

            sf::RectangleShape rT2(sf::Vector2f(250.f, 45.f)); 
            rT2.setPosition(100.f, 395.f); 
            rT2.setFillColor(wybraneTlo == 2 ? ciemnyZielony : szary); 
            rT2.setOutlineThickness(2.f);
            window.draw(rT2);
            sf::Text tT2("Stol: Sukno", font, 18); 
            tT2.setPosition(100.f + (250.f - tT2.getLocalBounds().width)/2.f, 407.f); 
            window.draw(tT2);

            sf::RectangleShape rT3(sf::Vector2f(250.f, 45.f)); 
            rT3.setPosition(100.f, 450.f); 
            rT3.setFillColor(wybraneTlo == 3 ? ciemnyZielony : szary); 
            rT3.setOutlineThickness(2.f);
            window.draw(rT3);
            sf::Text tT3("Stol: Zakard", font, 18); 
            tT3.setPosition(100.f + (250.f - tT3.getLocalBounds().width)/2.f, 462.f); 
            window.draw(tT3);

            sf::RectangleShape pBox(sf::Vector2f(450.f, 450.f)); 
            pBox.setPosition(450.f, 140.f); 
            pBox.setFillColor(sf::Color::Black); 
            pBox.setOutlineThickness(3.f); 
            pBox.setOutlineColor(sf::Color::Yellow); 
            window.draw(pBox);

            sf::Sprite pTlo; 
            if(wybraneTlo == 1) pTlo.setTexture(gra.texTlo1); 
            else if(wybraneTlo == 2) pTlo.setTexture(gra.texTlo2); 
            else pTlo.setTexture(gra.texTlo3);

            pTlo.setPosition(470.f, 200.f); 
            pTlo.setScale(410.f / pTlo.getLocalBounds().width, 310.f / pTlo.getLocalBounds().height); 
            window.draw(pTlo);
            
            sf::Sprite pA, pR;
            if(wybranyStyl == 1) { 
                pA.setTexture(gra.texAwers[0][13]); // Zawsze wyświetli pika na podglądzie
                pR.setTexture(gra.texRewers); 
                pA.setScale(86.f / pA.getLocalBounds().width, 120.f / pA.getLocalBounds().height); 
                pR.setScale(86.f / pR.getLocalBounds().width, 120.f / pR.getLocalBounds().height); 
                pA.setPosition(510.f, 250.f); 
                pR.setPosition(630.f, 250.f); 
            } else { 
                pA.setTexture(gra.texAwers2[0][13]); 
                pR.setTexture(gra.texRewers2); 
                pA.setScale(70.f / pA.getLocalBounds().width, 100.f / pA.getLocalBounds().height); 
                pR.setScale(70.f / pR.getLocalBounds().width, 100.f / pR.getLocalBounds().height); 
                pA.setPosition(520.f, 260.f); 
                pR.setPosition(630.f, 260.f); 
            }
            window.draw(pA); 
            window.draw(pR);

            sf::RectangleShape rB(sf::Vector2f(btnWroc.width, btnWroc.height)); 
            rB.setPosition(btnWroc.left, btnWroc.top); 
            rB.setFillColor(ciemnyCzerwony); 
            rB.setOutlineThickness(2.f);
            window.draw(rB);

            sf::Text tB("Wstecz", font, 18); 
            tB.setPosition(btnWroc.left + (btnWroc.width - tB.getLocalBounds().width)/2.f, btnWroc.top + 12.f); 
            window.draw(tB);
        }
        else if (obecnyStan == ROZGRYWKA) {
            gra.aktualizujCzas();
            gra.rysuj(window, font, wybranyStyl, wybraneTlo);
            
            // Obsluga polimorficznych animacji (obiekty 2D)
            gra.aktualizujObiekty(dt);
            for (auto& obj : gra.obiektyGry) {
                obj->rysuj(window);
            }

            sf::RectangleShape rZap(sf::Vector2f(btnZapisz.width, btnZapisz.height));
            rZap.setPosition(btnZapisz.left, btnZapisz.top);
            rZap.setFillColor(ciemnyZielony);
            rZap.setOutlineThickness(2.f);
            window.draw(rZap);
            sf::Text tZap("ZAPISZ GRE", font, 16);
            tZap.setPosition(btnZapisz.left + 15.f, btnZapisz.top + 10.f);
            window.draw(tZap);
        }

        window.display();
    }
    return 0;
}
