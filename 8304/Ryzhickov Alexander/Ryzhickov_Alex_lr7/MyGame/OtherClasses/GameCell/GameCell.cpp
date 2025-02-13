//
// Created by Alex on 21.03.2020.
//

#include "GameCell.h"

namespace MyGame {
    GameCell::GameCell() {
        GameCell::unit = nullptr;
        GameCell::neutralObject = nullptr;
        GameCell::base = nullptr;
        numer++;
    }

    void GameCell::setUnit(Unit *unit) {
        GameCell::unit = unit;
        /*if(neutralObject!= nullptr){
            neutralObject[unit->getArmorForGenerator()];
        }*/
    }

    Unit *GameCell::getUnit() const {
        return unit;
    }

    void GameCell::deleteUnit() {
        GameCell::unit = nullptr;
    }

    void GameCell::setLanscape(Landscape *lanscape) {
        GameCell::lanscape = lanscape;
    }

    void GameCell::setNeutralObject(NeutralObject *neutralObject) {
        GameCell::neutralObject = neutralObject;
    }

    void GameCell::setBase(Base *base) {
        GameCell::base = base;
    }

    Landscape *GameCell::getLanscape() const {
        return lanscape;
    }

    NeutralObject *GameCell::getNeutralObject() const {
        return neutralObject;
    }
}
