#include "weapon.h"

GetWeapon::GetWeapon(Weapon * wep):weapon(wep)
{

}

void GetWeapon::operator +=(Unit &unit)
{
    weapon->use(unit);
}

string GetWeapon::getType()const{
    return "neutral";
}

NeutralType GetWeapon::getTypeEnum()
{
    return weapon->type();
}

GetWeapon::~GetWeapon()
{
    delete weapon;
}

void Sword::use(Unit &unit)
{
    unit.getAttributes()->setAttack(unit.getAttributes()->getAttack()+50);
}

NeutralType Sword::type()
{
    return SWORD;
}

void Sickle::use(Unit &unit)
{
    unit.getAttributes()->setAttack(unit.getAttributes()->getAttack()+100);
}

NeutralType Sickle::type()
{
    return SICKLE;
}


string GetWeapon::characteristic()
{
    return "GetWeapon";
}
