#include <iostream>
#include <vector>
#include "config.h"
#include "character.h"

using namespace std;


//=== Field ========================================================


void Field::setUnit(int player, int position, Unit *character) {
    units[player][position] = character;
}

Unit *Field::getUnit(int player, int position) {
    return this->units[player][position];
}

int Field::getRound() {
    return round;
}

void Field::endRound() {
    round++;
}

int Field::getPlayer() {
    //return (round+1)%2;
    return round % 2 == 0 ? PLAYER_B : PLAYER_A;
}

string Field::getInfo() {
    string info = "";
    info += "Player A\n";
    for (int i = 0; i < CHARACTER_PER_LINE; i++) {
        info += getUnit(PLAYER_A, i)->getInfo() + "\n";
    }
    info += "\n";

    info += "Player B\n";
    for (int i = 0; i < CHARACTER_PER_LINE; i++) {
        info += getUnit(PLAYER_B, i)->getInfo() + "\n";
    }
    return info;
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


class ActionAttack : public Action {
protected:
    Unit *target;
public:
    ActionAttack(Unit *unit) : Action(unit) {}

    void setTargetUnit(Unit *unit) {
        this->target = unit;
    }

    string name() override {
        return "action : " + unit->getName() + " attack to " + target->getName();
    }

    void invoke(Field *field, int player, int position) override {
        setTargetUnit(field->getUnit(player, position));
        unit->attack(target);
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
    this->hp = hp;
}

int Unit::getHp() {
    return this->hp;
}

string Unit::getInfo() {
    return "Unit[ " + getName() + ", Attack: " + to_string(getAtk()) + ", Hitpoints: " + to_string(getHp()) +
           " ]";
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

void Unit::whenTurnStart(Field *field) {

}

void Unit::whenTurnEnd(Field *field) {

}


//=== Controller ========================================================

class Controller {
private:
    Field *field;
public:
    void initialCharacters() {
        char name = 'A';
        for (int player = 0; player < NUMBER_OF_PLAYERS; player++) {
            for (int i = 0; i < CHARACTER_PER_LINE; i++) {
                string names(1, name++);
                Unit *character = new Unit(names);
                field->setUnit(player, i, character);
            }
        }
    }

    string getInfo() {
        string info = "";
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

    void invokeAction(Action *action, int position) {
        int player = field->getPlayer();
        action->invoke(field, player, position);
    }
};

int main() {

    int i;

    Controller controller;
    controller.initialCharacters();
    cout << controller.getInfo();

    vector<Action *> actions = controller.selectUnit(PLAYER_A, 0);
    i = 1;
    for (auto &action : actions) {
        cout << i++ << ". " << action->name() << endl;
    }

    controller.invokeAction(actions[0], 0);
    cout << controller.getInfo();




    vector<Action *> actions2 = controller.selectUnit(PLAYER_A, 0);
    i = 1;
    for (auto &action : actions2) {
        cout << i++ << ". " << action->name() << endl;
    }

    controller.invokeAction(actions2[0], 0);
    cout << controller.getInfo();






    vector<Action *> actions3 = controller.selectUnit(PLAYER_A, 0);
    i = 1;
    for (auto &action : actions3) {
        cout << i++ << ". " << action->name() << endl;
    }


    return 0;
}