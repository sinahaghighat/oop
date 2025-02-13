//
// Created by Artem Butko on 20/05/2020.
//

#include "CreateUnitCommand.h"

void CreateUnitCommand::Command()
{
    int number;
    if (player == "B") number = 0;
    else if (player == "D") number = 1;
    else number = 2;
    if (this->field->basesOnField[number]!= nullptr)
    {
        std::cout << "------ CREATING UNIT ------" << std::endl;
        std::cout << "'C' - Crossbowman" << std::endl;
        std::cout << "'A' - Archer" << std::endl;
        std::cout << "'W' - Wizard" << std::endl;
        std::cout << "'M' - Mage" << std::endl;
        std::cout << "'K' - Knight" << std::endl;
        std::cout << "'S' - Spearman" << std::endl;
        std::cout << "---------------------------" << std::endl;
        std::cout << "id = ";
        std::cin >> this->id;
        std::cout << "---------------------------" << std::endl;
        field->basesOnField[number]->createUnit(this->id);
        field->logging->loggingCreateUnit(this->id);
    }
    else std::cout << "ERROR: Base doesn't exist!" << std::endl;

}
