#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>
#include <ctime>

using namespace std;

class Equipment {
protected:
    string name;

public:
    Equipment(string name) : name(name) {}
    virtual ~Equipment() {}

    string getName() const { return name; }
    virtual void display() const = 0;
};

class Weapon : public Equipment {
private:
    int damageBonus;

public:
    Weapon(string name, int damageBonus)
        : Equipment(name), damageBonus(damageBonus) {}

    int getDamageBonus() const { return damageBonus; }

    void display() const override {
        cout << "Weapon: " << name << ", Damage Bonus: " << damageBonus << endl;
    }

    static const vector<pair<string, int>>& getWeaponList() {
        static const vector<pair<string, int>> weaponList = {
            {"Iron Sword", 5}, {"Steel Axe", 7}, {"Long Bow", 6}, {"Magic Staff", 8}, {"Dagger", 4}
        };
        return weaponList;
    }
};

class Armor : public Equipment {
private:
    int defenseBonus;

public:
    Armor(string name, int defenseBonus)
        : Equipment(name), defenseBonus(defenseBonus) {}

    int getDefenseBonus() const { return defenseBonus; }

    int reduceDamage(int incomingDamage) const {
        double reductionFactor = 1 - exp(-0.01 * defenseBonus);
        int reducedDamage = incomingDamage * (1.0 - reductionFactor);
        if (reducedDamage < 1) reducedDamage = 1;
        return reducedDamage;
    }

    void display() const override {
        cout << "Armor: " << name << ", Defense Bonus: " << defenseBonus << endl;
    }

    static const vector<pair<string, int>>& getArmorList() {
        static const vector<pair<string, int>> armorList = {
            {"Leather Armor", 3}, {"Chainmail", 5}, {"Plate Armor", 7}, {"Mage Robes", 2}, {"Cloak", 1}
        };
        return armorList;
    }
};

class Spell {
private:
    string name;
    int damage;
    int manaCost;

public:
    Spell(string name, int damage, int manaCost)
        : name(name), damage(damage), manaCost(manaCost) {}

    string getName() const { return name; }
    int getDamage() const { return damage; }
    int getManaCost() const { return manaCost; }
};

class Character {
protected:
    string name;
    int health;
    int MaxHealth;
    int level;
    int strength;
    int dexterity;
    int intelligence;
    int mana;
    int maxMana;
    Weapon* weapon;
    Armor* armor;
    vector<Spell> spells;

public:
    Character(string name, int level, Weapon* weapon = nullptr, Armor* armor = nullptr)
        : name(name), level(level), weapon(weapon), armor(armor), health(0), mana(0), maxMana(0) {}

    virtual ~Character() {}

    string getName() const { return name; }
    int getHealth() const { return health; }
    int getLevel() const { return level; }
    int getMana() const { return mana; }

    void initializeStats() {
        setStatsByClass();
    }

    virtual void setStatsByClass() = 0;

    string takeDamage(int damage) {
        int actualDamage = damage;
        if (armor) {
            actualDamage = armor->reduceDamage(damage);
        }
        health -= actualDamage;
        if (health < 0) health = 0;

        string log = name + " takes " + to_string(actualDamage) + " damage. Health is now " + to_string(health) + ".";
        if (health == 0) {
            log += " " + name + " dies!";
        }
        return log;
    }

    bool isAlive() const {
        return health > 0;
    }

    virtual int getDamagePotential() const = 0;

    virtual string attack(Character& target) = 0;

    virtual string castSpell(Spell& spell, Character& target) = 0;

    const vector<Spell>& getAvailableSpells() const {
        return spells;
    }

    void resetHealth() {
        health = MaxHealth;
    }

    virtual void display() const {
        cout << "Character: " << name
                  << ", Health: " << health
                  << ", Level: " << level
                  << ", Strength: " << strength
                  << ", Dexterity: " << dexterity
                  << ", Intelligence: " << intelligence
                  << ", Mana: " << mana << "/" << maxMana << endl;
        if (weapon) {
            weapon->display();
        }
        if (armor) {
            armor->display();
        }
    }
};

class Warrior : public Character {
private:
    Spell* heavySlash;
    Spell* smite;

public:
    Warrior(string name, int level, Weapon* weapon = nullptr, Armor* armor = nullptr)
        : Character(name, level, weapon, armor) {
        initializeStats();
        heavySlash = new Spell("Heavy Slash", level * 3 + 10, 5); 
        smite = new Spell("Smite", level * 4 + 10, 8);
        spells.push_back(*heavySlash);
        spells.push_back(*smite);
    }

    void setStatsByClass() override {
        strength = level * 5;
        dexterity = 5 + level;
        intelligence = 2 + level * 2;
        health = 40 + strength * 4;
        MaxHealth = health;
        maxMana = intelligence * 2;
        mana = maxMana;
    }

    string attack(Character& target) override {
        int damage = strength + (level * 2);
        if (weapon) {
            damage += weapon->getDamageBonus();
        }
        target.takeDamage(damage);
        return name + " swings a sword at " + target.getName() + ", dealing " + to_string(damage) + " damage!";
    }

    string castSpell(Spell& spell, Character& target) override {
        if (mana >= spell.getManaCost()) {
            mana -= spell.getManaCost();
            target.takeDamage(spell.getDamage());
            return name + " casts " + spell.getName() + " on " + target.getName() + ", dealing " + to_string(spell.getDamage()) + " damage!";
        } else {
            return name + " doesn't have enough mana to cast " + spell.getName() + "!";
        }
    }

    int getDamagePotential() const override {
        return strength + (level * 2);
    }

    ~Warrior() {
        delete heavySlash;
        delete smite;
    }
};

class Archer : public Character {
private:
    Spell* powerShot;
    Spell* bearTrap;

public:
    Archer(string name, int level, Weapon* weapon = nullptr, Armor* armor = nullptr)
        : Character(name, level, weapon, armor) {
        initializeStats();
        powerShot = new Spell("Power Shot", level * 3 + 10, 5);
        bearTrap = new Spell("Bear Trap", level * 4 + 10, 8);
        spells.push_back(*powerShot);
        spells.push_back(*bearTrap);
    }

    void setStatsByClass() override {
        strength = 5 + level * 2;        
        dexterity = 10 + level * 5; 
        intelligence = 2 + level;    
        health = 40 + (strength * 4);
        MaxHealth = health;
        maxMana = intelligence * 2;
        mana = maxMana;
    }

    string attack(Character& target) override {
        int damage = dexterity + (level * 1);
        if (weapon) {
            damage += weapon->getDamageBonus();
        }
        target.takeDamage(damage);
        return name + " shoots an arrow at " + target.getName() + ", dealing " + to_string(damage) + " damage!";
    }

    string castSpell(Spell& spell, Character& target) override {
        if (mana >= spell.getManaCost()) {
            mana -= spell.getManaCost();
            target.takeDamage(spell.getDamage());
        return name + " casts " + spell.getName() + " on " + target.getName() + ", dealing " + to_string(spell.getDamage()) + " damage!";
        } else {
            return name + " doesn't have enough mana to cast " + spell.getName() + "!";
        }
    }

    void display() const override {
        cout << "Archer: " << name << endl;
        Character::display();
    }

    int getDamagePotential() const override {
        return dexterity + (level * 1);
    }

    ~Archer() {
        delete powerShot;
        delete bearTrap;
    }
};

class Mage : public Character {
private:
    Spell* iceShard;
    Spell* fireBlast;

public:
    Mage(string name, int level, Weapon* weapon = nullptr, Armor* armor = nullptr)
        : Character(name, level, weapon, armor) {
        initializeStats();
        iceShard = new Spell("Ice Shard", level * 3 + 10, 5);
        fireBlast = new Spell("Fire Blast", level * 4 + 10, 8);
        spells.push_back(*iceShard);
        spells.push_back(*fireBlast);
    }

    void setStatsByClass() override {
        strength = 5 + level; 
        dexterity = 2 + level * 2;    
        intelligence = 10 + level * 5; 
        health = 50 + strength * 4; 
        MaxHealth = health;
        maxMana = intelligence * 2;
        mana = maxMana;
    }

    string attack(Character& target) override {
        int damage = intelligence + (level * 3);
        if (weapon) {
            damage += weapon->getDamageBonus(); 
        }
        target.takeDamage(damage);
        return name + " shoots a firebolt at " + target.getName() + ", dealing " + to_string(damage) + " damage!";
    }

    string castSpell(Spell& spell, Character& target) override {
        if (mana >= spell.getManaCost()) {
            mana -= spell.getManaCost();
            target.takeDamage(spell.getDamage());
        return name + " casts " + spell.getName() + " on " + target.getName() + ", dealing " + to_string(spell.getDamage()) + " damage!";
        } else {
            return name + " doesn't have enough mana to cast " + spell.getName() + "!";
        }
    }

    void display() const override {
        cout << "Mage: " << name << endl;
        Character::display();
    }

    
    int getDamagePotential() const override {
        return intelligence + (level * 3);
    }

    ~Mage() {
        delete iceShard;
        delete fireBlast;
    }
};

int getValidatedInput(const string& prompt, int minRange, int maxRange) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail() || value < minRange || value > maxRange) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid number between " << minRange << " and " << maxRange << ".\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

Character* createCharacter() {
    string name;
    int classChoice, level;

    cout << "Enter character name: ";
    cin >> name;

    classChoice = getValidatedInput("Choose character class (1: Warrior, 2: Mage, 3: Archer): ", 1, 3);
    level = getValidatedInput("Enter character level: ", 1, 100);

    Weapon* weapon = nullptr;
    int addWeapon = getValidatedInput("Do you want to add a weapon? (1 for Yes, 0 for No): ", 0, 1);
    if (addWeapon == 1) {
        const auto& weaponList = Weapon::getWeaponList();
        int randomIndex = rand() % weaponList.size();
        weapon = new Weapon(weaponList[randomIndex].first, weaponList[randomIndex].second);
        cout << "Assigned weapon: " << weaponList[randomIndex].first << " (Damage Bonus: " << weaponList[randomIndex].second << ")\n";
    }

    Armor* armor = nullptr;
    int addArmor = getValidatedInput("Do you want to add armor? (1 for Yes, 0 for No): ", 0, 1);
    if (addArmor == 1) {
        const auto& armorList = Armor::getArmorList();
        int randomIndex = rand() % armorList.size();
        armor = new Armor(armorList[randomIndex].first, armorList[randomIndex].second);
        cout << "Assigned armor: " << armorList[randomIndex].first << " (Defense Bonus: " << armorList[randomIndex].second << ")\n";
    }

    Character* character = nullptr;
    switch (classChoice) {
    case 1:
        character = new Warrior(name, level, weapon, armor);
        break;
    case 2:
        character = new Mage(name, level, weapon, armor);
        break;
    case 3:
        character = new Archer(name, level, weapon, armor);
        break;
    }

    return character;
}

enum class FocusStrategy {
    LowestHP,
    HighestHP,
    LowestDamage,
    HighestDamage
};

Character* findTarget(Character* attacker, const vector<Character*>& enemies, FocusStrategy strategy) {
    Character* target = nullptr;
    for (Character* enemy : enemies) {
        if (!enemy->isAlive()) continue;

        if (!target) {
            target = enemy;
            continue;
        }

        switch (strategy) {
            case FocusStrategy::LowestHP:
                if (enemy->getHealth() < target->getHealth()) {
                    target = enemy;
                }
                break;
            case FocusStrategy::HighestHP:
                if (enemy->getHealth() > target->getHealth()) {
                    target = enemy;
                }
                break;
            case FocusStrategy::LowestDamage:
                if (enemy->getDamagePotential() < target->getDamagePotential()) {
                    target = enemy;
                }
                break;
            case FocusStrategy::HighestDamage:
                if (enemy->getDamagePotential() > target->getDamagePotential()) {
                    target = enemy;
                }
                break;
        }
    }
    return target;
}

class BattleGraph {
private:
    map<Character*, vector<Character*>> adjList;

public:
    void addCharacter(Character* character) {
        adjList[character] = vector<Character*>();
    }

    void addEdge(Character* c1, Character* c2) {
        adjList[c1].push_back(c2);
    }

    void createEdgesBasedOnCriteria(vector<Character*>& group1, vector<Character*>& group2, FocusStrategy strategy) {
        adjList.clear();

        for (Character* c1 : group1) {
            Character* target = findTarget(c1, group2, strategy);
            if (target) {
                addEdge(c1, target);
            }
        }

        for (Character* c2 : group2) {
            Character* target = findTarget(c2, group1, strategy);
            if (target) {
                addEdge(c2, target);
            }
        }
    }

    void displayGraph() const {
        cout << "Focus targets for this round:" << endl;
        for (const auto& pair : adjList) {
            cout << pair.first->getName() << " focuses on: ";
            if (pair.second.empty()) {
                cout << "No target";
            } else {
                for (Character* target : pair.second) {
                    cout << target->getName() << " ";
                }
            }
            cout << endl;
        }
    }
};

void battleSimulation(BattleGraph& graph, vector<Character*>& group1, vector<Character*>& group2, FocusStrategy strategy, int rounds, bool showLog) {
    int group1Wins = 0;
    int group2Wins = 0;

    srand(static_cast<unsigned>(time(0)));

    for (int i = 0; i < rounds; ++i) {
        if (showLog) {
            cout << "\nRound " << i + 1 << " - Target Focus:" << endl;
        }

        graph.createEdgesBasedOnCriteria(group1, group2, strategy);

        if (showLog) {
            graph.displayGraph();
        }

        for (Character* c : group1) c->resetHealth();
        for (Character* c : group2) c->resetHealth();

        bool roundOver = false;

        while (!roundOver) {
            for (Character* attacker : group1) {
                if (!attacker || !attacker->isAlive()) continue;

                Character* defender = findTarget(attacker, group2, strategy);
                if (!defender) {
                    group1Wins++;
                    roundOver = true;
                    break;
                }

                string attackLog = attacker->attack(*defender);
                if (showLog) cout << attackLog << endl;

                string damageLog = defender->takeDamage(attacker->getDamagePotential());
                if (showLog) cout << damageLog << endl;

                if (!defender->isAlive()) continue;

                if (!attacker->getAvailableSpells().empty() && rand() % 2 == 0) {
                    Spell spell = Spell(attacker->getAvailableSpells().front());
                    if (attacker->getMana() >= spell.getManaCost()) {
                        string spellLog = attacker->castSpell(spell, *defender);
                        if (showLog) cout << spellLog << endl;

                        damageLog = defender->takeDamage(spell.getDamage());
                        if (showLog) cout << damageLog << endl;
                    }
                }
            }

            if (roundOver) break;

            for (Character* attacker : group2) {
                if (!attacker || !attacker->isAlive()) continue;

                Character* defender = findTarget(attacker, group1, strategy);
                if (!defender) {
                    group2Wins++;
                    roundOver = true;
                    break;
                }

                string attackLog = attacker->attack(*defender);
                if (showLog) cout << attackLog << endl;

                string damageLog = defender->takeDamage(attacker->getDamagePotential());
                if (showLog) cout << damageLog << endl;

                if (!defender->isAlive()) continue;

                if (!attacker->getAvailableSpells().empty() && rand() % 2 == 0) {
                    Spell spell = Spell(attacker->getAvailableSpells().front());
                    if (attacker->getMana() >= spell.getManaCost()) {
                        string spellLog = attacker->castSpell(spell, *defender);
                        if (showLog) cout << spellLog << endl;

                        damageLog = defender->takeDamage(spell.getDamage());
                        if (showLog) cout << damageLog << endl;
                    }
                }
            }
        }
    }

    cout << "\nResults after " << rounds << " rounds:\n";
    cout << "Group 1 won: " << group1Wins << " time(s).\n";
    cout << "Group 2 won: " << group2Wins << " time(s).\n";
    double probGroup1Win = static_cast<double>(group1Wins) / rounds * 100;
    double probGroup2Win = static_cast<double>(group2Wins) / rounds * 100;
    cout << "Group 1 win probability: " << probGroup1Win << "%\n";
    cout << "Group 2 win probability: " << probGroup2Win << "%\n";
}

FocusStrategy chooseFocusStrategy() {
    cout << "Choose criteria for characters to foccus their attck on:\n";
    cout << "1. Least HP\n";
    cout << "2. Most HP\n";
    cout << "3. least Damage\n";
    cout << "4. Most Damage\n";
    int choice;
    cin >> choice;
    switch (choice) {
        case 1: return FocusStrategy::LowestHP;
        case 2: return FocusStrategy::HighestHP;
        case 3: return FocusStrategy::LowestDamage;
        case 4: return FocusStrategy::HighestDamage;
        default: 
            cout << "Invalid choice. Retutned to default (lowest HP)\n";
            return FocusStrategy::LowestHP;
    }
}

void mainMenu() {
    BattleGraph graph;
    vector<Character*> group1, group2;

    int choice;
    FocusStrategy strategy = FocusStrategy::LowestHP;

    cout << "Welcome to the Battle Simulation!\n";
    cout << "You can create characters for 2 groups and simulate battles between them!\n";
    
    do {
        cout << "\nMain Menu:\n";
        cout << "1. Create character for Group 1\n";
        cout << "2. Create character for Group 2\n";
        cout << "3. Display all characters\n";
        cout << "4. Set up and run battle simulation\n";
        cout << "5. Choose strategy\n";
        cout << "6. Exit\n";
        choice = getValidatedInput("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1: {
                cout << "\nCreating character for Group 1...\n";
                Character* newCharacter = createCharacter();
                if (newCharacter) {
                    group1.push_back(newCharacter);
                }
                break;
            }
            case 2: {
                cout << "\nCreating character for Group 2...\n";
                Character* newCharacter = createCharacter();
                if (newCharacter) {
                    group2.push_back(newCharacter);
                }
                break;
            }
            case 3: {
                cout << "\nDisplaying all characters:\n";
                cout << "\n--- Group 1 ---\n";
                for (const Character* character : group1) {
                    character->display();
                    cout << endl;
                }
                cout << "\n--- Group 2 ---\n";
                for (const Character* character : group2) {
                    character->display();
                    cout << endl;
                }
                break;
            }
            case 4: {
                if (group1.empty() || group2.empty()) {
                    cout << "Both groups must have at least one character to start the battle!\n";
                } else {
                    int rounds = getValidatedInput("Enter the number of rounds for the battle: ", 1, 100);
                    int logChoice = getValidatedInput("Do you want a detailed battle log? (1 for Yes, 0 for No): ", 0, 1);
                    bool showLog = logChoice == 1;
                    battleSimulation(graph, group1, group2, strategy, rounds, showLog);
                }
                break;
            }
            case 5:
                strategy = chooseFocusStrategy();
                break;
            case 6:
                cout << "Exiting the program. Goodbye!\n";
                break;
        }
    } while (choice != 6);

    for (Character* character : group1) {
        delete character;
    }
    for (Character* character : group2) {
        delete character;
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    mainMenu();
    return 0;
}