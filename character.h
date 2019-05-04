#ifndef PROJECT_CHARACTER_H
#define PROJECT_CHARACTER_H

#include <iostream>
#include "config.h"
#include "Field.h"

using namespace std;

class Action {
protected:
    Unit *unit;
    int rate = 1;
public:
    Action(Unit *unit);

    virtual string name();

    virtual void invoke(Field *field, int player, int position);

    virtual void setRate(int rate);

    virtual int getRate();

    virtual int getType();
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

    virtual string getInfo();

    virtual string getSkillName();

    virtual int getBeTargetRate();

    bool canBeTarget();

    bool canAttack(Unit *target);

    void attack(Unit *target);

    bool isAlive();

    bool isAvailableToUseSkill();

    bool isAvailableToAttackl();

    virtual void useSkill(Field *field);

    virtual void whenTurnStart(Field *field);

    virtual void whenTurnEnd(Field *field);

    virtual bool isStillCooldown();
};


#endif //PROJECT_CHARACTER_H
