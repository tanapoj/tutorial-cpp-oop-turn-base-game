#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <iostream>
#include "config.h"
#include "Field.h"

using namespace std;

class Action {
protected:
    Unit *unit;
public:
    Action(Unit *unit);

    virtual string name();

    virtual void invoke(Field* field, int player, int position);
};

class Unit {
protected:
    string name;
    int atk;
    int hp;
public:
    Unit(string name, int atk = 5, int hp = 10);

    void setName(string name);

    string getName();

    void setAtk(int atk);

    int getAtk();

    void setHp(int hp);

    int getHp();

    string getInfo();

    bool canBeTarget();

    bool canAttack(Unit *target);

    void attack(Unit *target);

    bool isAlive();

    void whenTurnStart(Field *field);

    void whenTurnEnd(Field *field);
};


#endif //PROJECT_CHARACTER_H
