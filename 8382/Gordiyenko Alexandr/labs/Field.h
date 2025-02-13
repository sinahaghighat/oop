//
// Created by alexg on 03.06.2020.
//

#ifndef LABS_FIELD_H
#define LABS_FIELD_H

#include "Units/Unit.h"
#include "Landscape/Landscape.h"
#include <iostream>
#include "extra4/adapter.h"
#include "extra5/snapshotbackup.h"
//#include "Command.h"

struct Cell{
    int x, y;
    Unit* unit;
    Landscape* landscape;
    NeutralObject* object;

    Cell() {}
    Cell(int x,int y, Unit* unit, Landscape* landscape, NeutralObject* object) :
            x(x), y(y),
            unit(unit),
            landscape(landscape),
            object(object)
    {}
};

//class Base;
class SnapshotBackup;
class Field {
    friend std::ostream& operator<< (std::ostream &out, const Field& field);
    friend class SnapshotBackup;
    friend class createBaseCommand;

    int x, y;
    int maxUnit;
    int curUnit;
    std::vector<Base*> bases;
    Cell ***cell; //матрица(двумерный) указателей на юниты. в каждой ячейке матрцы адрес юнита

public:
    Adapter* adapter;

    friend class Iterator;
    Field(int x, int y, int maxUnit, Adapter* adapter);

    //копирование поля. сслыка на другое поле,
    Field(const Field &field);
    Field(Field &&field);

    const Field& operator=(const Field& f) {
        *this = f;
        return *this;
    }

    void addUnit(Unit* unit);
    Base* createBase(int id, int x, int y, int maxCountUnit, int health);

    Base* getBase(int id) { return bases[id]; }

    void removeUnit(Unit* unit);
    void print();

    bool moveUnit(Unit* unit, int x, int y);

    void addLandscape();

    void printLandscape();
    Adapter *getAdapter();

    SnapshotBackup *createSnap(std::string mode);
};

#endif //LABS_FIELD_H
