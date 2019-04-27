#include <iostream>
#include <stdio.h>
#include <vector>
#include <random>
#include <sstream>
#include <string>
#include <ctime>
#include "config.h"
#include "character.h"

using namespace std;

int randomInt(int min, int max) {
    return min + (rand() % (int) (max - min + 1));
}

string spacePadNumber(int num, int digit = 2) {
    stringstream ss;
    ss << num;
    string ret;
    ss >> ret;

    // Append zero chars
    int str_length = ret.length();
    for (int i = 0; i < digit - str_length; i++)
        ret = " " + ret;
    return ret;
}

//=== Field ========================================================

void Field::setUnit(int player, int position, Unit *character) {
    units[player][position] = character;
}

Unit *Field::getUnit(int player, int position) {
    return this->units[player][position];
}

int Field::getRound() {
    return round + 1;
}

void Field::enterRound() {
    for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
        for (int position = 0; position < CHARACTER_PER_LINE; position++) {
            units[player][position]->whenTurnStart(this);
        }
    }
}

void Field::endRound() {
    for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
        for (int position = 0; position < CHARACTER_PER_LINE; position++) {
            units[player][position]->whenTurnEnd(this);
        }
    }
    round++;
}

int Field::getPlayer() {
    //return (round+1)%2;
    return round % 2 == 1 ? PLAYER_B : PLAYER_A;
}

string Field::getInfo() {
    string info = "";
    info += "Player A\t\t\t\t\t\t\t\t\tPlayer B\n";
    for (int i = 0; i < CHARACTER_PER_LINE; i++) {
        info += getUnit(PLAYER_A, i)->getInfo() + "\t\t" + getUnit(PLAYER_B, i)->getInfo();
        info += "\n";
    }
    info += "\n";
    return info;
}

int Field::getOwnerOf(Unit *unit) {
    for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
        for (int position = 0; position < CHARACTER_PER_LINE; position++) {
            if (units[player][position] == unit) {
                return player;
            }
        }
    }
    return -1;
}

//=== Action ========================================================
Action::Action(Unit *unit) {
    this->unit = unit;
};

string Action::name() {
    return "-";
}

void Action::invoke(Field *field, int player, int position) {

}

void Action::setRate(int rate) {
    this->rate = rate;
}

int Action::getRate() {
    return this->rate;
}

int Action::getType() {
    return 0;
}


class ActionAttack : public Action {
protected:
    Unit *target;
public:
    ActionAttack(Unit *unit) : Action(unit) {}

    void setTargetUnit(Unit *unit) {
        this->target = unit;
        this->setRate(unit->getBeTargetRate());
    }

    string name() override {
        return "action : " + unit->getName() + " attack to " + target->getName() + " (" + to_string(this->getRate()) +
               ")";
    }

    void invoke(Field *field, int player, int position) override {
//        setTargetUnit(field->getUnit(player, position));
        setTargetUnit(this->target);
        unit->attack(target);
    }

    int getType() override {
        return ACTION_TYPE_ATTACK;
    }
};

class ActionSkill : public Action {
protected:
    Unit *target;
public:
    ActionSkill(Unit *unit) : Action(unit) {}

    void setTargetUnit(Unit *unit) {
        this->target = unit;
    }

    string name() override {
        return "action : skill!";
    }

    void invoke(Field *field, int player, int position) override {

    }

    int getType() override {
        return ACTION_TYPE_SKILL;
    }
};

//=== Unit ========================================================

Unit::Unit(string name, int atk, int hp) {
    this->setName(name);
    this->setAtk(atk);
    this->setHp(hp);
}

void Unit::setName(string name) {
    this->name = name;
}


string Unit::getName() {
    return this->name;
}

void Unit::setAtk(int atk) {
    this->atk = atk;
}

int Unit::getAtk() {
    return this->atk;
}

void Unit::setHp(int hp) {
    this->hp = hp < 0 ? 0 : hp;
}

int Unit::getHp() {
    return this->hp;
}

string Unit::getInfo() {
    return "Unit[ "
           + getName()
           + ", Attack: " + spacePadNumber(getAtk())
           + ", Hitpoints: " + spacePadNumber(getHp())
           + " ]";
}

int Unit::getBeTargetRate() {
    return 1;
}

bool Unit::canBeTarget() {
    return isAlive();
}

bool Unit::canAttack(Unit *target) {
    return isAlive() && target->isAlive();
}

void Unit::attack(Unit *target) {
    target->setHp(target->getHp() - this->getAtk());
}

bool Unit::isAlive() {
    return this->getHp() > 0;
}

bool Unit::isAvailableToAttackl() {
    return true;
}

bool Unit::isAvailableToUseSkill() {
    return true;
}

void Unit::useSkill(Field *field) {

}

void Unit::whenTurnStart(Field *field) {

}

void Unit::whenTurnEnd(Field *field) {

}

//=== Unit: Cooldownale ===

class UnitCooldownable : public Unit {
protected:
    int cooldown = 0;
public:
    UnitCooldownable(const string &name, int atk, int hp) : Unit(name, atk, hp) {}

    bool isStillCooldown() {
        return cooldown > 0;
    }

    void setCooldown(int turn = 4) {
        this->cooldown = turn;
    }

    void whenTurnEnd(Field *field) override {
        cooldown = cooldown > 0 ? cooldown - 1 : 0;
    }
};

//=== Unit: Healer ===

class UnitHealer : public UnitCooldownable {
public:
    UnitHealer(const string &name, int atk, int hp) : UnitCooldownable(name, atk, hp) {
        cout << "H" << endl;
    }

    void useSkill(Field *field) override {
        int player = field->getOwnerOf(this);
        int minPosition = 0;
        for (int position = 0; position < CHARACTER_PER_LINE; position++) {
            if (field->getUnit(player, position)->getHp() < field->getUnit(player, minPosition)->getHp()) {
                minPosition = position;
            }
        }
        Unit *unit = field->getUnit(player, minPosition);
        unit->setHp(unit->getHp() + 5);
    }

    string getInfo() override {
        return "Healer[ "
               + getName()
               + ", Attack: " + spacePadNumber(getAtk())
               + ", Hitpoints: " + spacePadNumber(getHp())
               + ", CD: " + spacePadNumber(cooldown)
               + " ]";
    }
};



//=== Unit: Tank ===

//=== Unit: DamageDealer ===


//=== Controller ========================================================

class Controller {
public:
    Field *field;

    bool isGameEnd() {
        for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
            int alive = 0;
            for (int position = 0; position < CHARACTER_PER_LINE; position++) {
                if (field->getUnit(player, position)->isAlive()) {
                    alive++;
                }
            }
            if (alive == 0) {
                return true;
            }
        }
        return false;
    }

    int getWinner() {
        for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
            int alive = 0;
            for (int position = 0; position < CHARACTER_PER_LINE; position++) {
                if (field->getUnit(player, position)->isAlive()) {
                    alive++;
                }
            }
            if (alive > 0) {
                return player;
            }
        }
        return 0;
    }

    void startRound() {
    }

    void endRound() {
        field->endRound();
    }

    void initialCharacters() {
        char name = 'A';
        for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
            for (int i = 0; i < CHARACTER_PER_LINE; i++) {
                string names(1, name++);

                Unit *character;
                switch (randomInt(0, 9)) {
                    case 1:
                    case 2:
                        character = new UnitHealer(names, randomInt(5, 7), randomInt(10, 12));
                    default:
                        character = new Unit(names, randomInt(5, 7), randomInt(10, 12));
                }


                field->setUnit(player, i, character);
            }
        }
    }

    int getPlayer() {
        return field->getPlayer();
    }

    string getInfo() {
        string info = "";
        info += "Round " + to_string(field->getRound()) + " | ";
        if (field->getPlayer() == PLAYER_A) {
            info += "Player A\n";
        } else {
            info += "Player B\n";
        }
        info += field->getInfo();
        return info;
    }

    vector<Action *> selectUnit(int player, int position) {

        Unit *myUnit = field->getUnit(player, position);

        vector<Action *> actions;
        for (int i = 0; i < CHARACTER_PER_LINE; i++) {
            Unit *opponentUnit = field->getUnit(!player, i);
            if (opponentUnit->isAlive() && opponentUnit->canBeTarget()) {
                ActionAttack *a = new ActionAttack(myUnit);
                a->setTargetUnit(opponentUnit);
                actions.push_back(a);
            }
        }
//        actions.push_back(new ActionSkill(field->getUnit(player, position)));
        return actions;
    }

    Action *invokeActions(vector<Action *> actions, int actionType) {
        int player = field->getPlayer();
        int sum = 0;
        for (auto &action : actions) {
            if (action->getType() != actionType) {
                continue;
            }
            sum += action->getRate();
        }
        int r = randomInt(1, sum);
        sum = 0;
        for (auto &action : actions) {
            if (action->getType() != actionType) {
                continue;
            }
            sum += action->getRate();
            if (sum >= r) {
                invokeAction(action, -1);
                return action;
            }
        }
    }

    void invokeAction(Action *action, int position) {
        int player = field->getPlayer();
        action->invoke(field, player, position);
    }
};


int main() {

    int i = 100;
    srand((int) time(0));

    Controller controller;
    controller.initialCharacters();

    while (i > 0 && !controller.isGameEnd()) {
        controller.startRound();
        cout << controller.getInfo() << endl;
        int player = controller.getPlayer();

        int selectUnit = 0;
        if (player == PLAYER_A) {
            cout << "select your unit?" << endl;
            for (int position = 0; position < CHARACTER_PER_LINE; position++) {
                Unit *unit = controller.field->getUnit(player, position);
                if (unit->isAlive() && unit->isAvailableToAttackl()) {
                    cout << position << ". " << unit->getName() << endl;
                }
            }
            cin >> selectUnit;
        }

        vector<Action *> actions = controller.selectUnit(player, selectUnit);

        cout << "attack or skill?" << endl;
        cout << ACTION_TYPE_ATTACK << ". attack" << endl;
        cout << ACTION_TYPE_SKILL << ". use skill" << endl;

        Action *action = controller.invokeActions(actions, ACTION_TYPE_ATTACK);
        cout << action->name() << endl;

        controller.endRound();
        i--;
        cout << "-------------------------------------------------" << endl << endl;
    }

    if (controller.getWinner() == PLAYER_A) {
        cout << "You win" << endl;
    } else {
        cout << "You lose" << endl;
    }
    cout << controller.getInfo() << endl;

    return 0;
}


//int main_test() {
//
//    int i;
//
//    srand((int) time(0));
//
//    Controller controller;
//    controller.initialCharacters();
//    cout << controller.getInfo();
//    cout << "1 =========================================" << endl;
//
//    vector<Action *> actions = controller.selectUnit(PLAYER_A, 0);
//    i = 1;
//    for (auto &action : actions) {
//        cout << i++ << ". " << action->name() << endl;
//    }
//    controller.invokeAction(actions[0], 0);
//    cout << controller.getInfo();
//
//    cout << "2 =========================================" << endl;
//
//
//    vector<Action *> actions2 = controller.selectUnit(PLAYER_A, 0);
//    i = 1;
//    for (auto &action : actions2) {
//        cout << i++ << ". " << action->name() << endl;
//    }
//
//    controller.invokeActions(actions2, ACTION_TYPE_ATTACK);
//    cout << controller.getInfo();
//
//
//    cout << "3 =========================================" << endl;
//
//
//    vector<Action *> actions3 = controller.selectUnit(PLAYER_A, 0);
//    i = 1;
//    for (auto &action : actions3) {
//        cout << i++ << ". " << action->name() << endl;
//    }
//
//
//    return 0;
//}