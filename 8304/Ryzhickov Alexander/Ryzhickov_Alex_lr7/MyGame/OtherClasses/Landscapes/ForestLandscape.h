//
// Created by Alex on 21.03.2020.
//

#ifndef GAME_FORESTLANDSCAPE_H
#define GAME_FORESTLANDSCAPE_H


#include "Landscape.h"
namespace Landscapes {
    class ForestLandscape : public Landscape {
        bool getInformationAboutLandscape() override;

        void setBonus(GameUnit::Unit *unit) override;
    };
}

#endif //GAME_FORESTLANDSCAPE_H
