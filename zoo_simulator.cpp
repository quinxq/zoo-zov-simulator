/**
 * @file zoo_simulation.cpp
 * @brief Симуляция управления зоопарком, где игроки управляют животными, вольерами, работниками и финансами.
 *
 * Этот программный код реализует текстовую игру по управлению зоопарком. Игроки могут покупать и продавать животных,
 * нанимать работников, строить вольеры, управлять кредитами и разводить животных в течение 20 дней. Цель — поддерживать
 * прибыльный зоопарк, не исчерпав денежные средства.
 */
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <clocale>
#include <limits>
#include <numeric>
#include <random>
using namespace std;

/**
 * @enum AnimalType
 * @brief Определяет тип животного по рациону питания.
 */
enum class AnimalType {
    HERBIVORE,  /**< Травоядные животные */
    CARNIVORE   /**< Хищные животные */
};

/**
 * @enum Climate
 * @brief Определяет климатические предпочтения для животных и вольеров.
 */
enum class Climate {
    TROPICAL,   /**< Тропический климат */
    TEMPERATE,  /**< Умеренный климат */
    ARCTIC      /**< Арктический климат */
};

/**
 * @enum Gender
 * @brief Определяет пол животного.
 */
enum class Gender {
    MALE,       /**< Самец */
    FEMALE      /**< Самка */
};

/**
 * @enum WorkerType
 * @brief Определяет типы работников в зоопарке.
 */
enum class WorkerType {
    DIRECTOR,    /**< Директор зоопарка */
    VETERINARIAN,/**< Ветеринар */
    CLEANER,     /**< Уборщик */
    FEEDER       /**< Кормилец */
};

/**
 * @class Loan
 * @brief Представляет финансовый кредит, взятый зоопарком.
 *
 * Управляет деталями кредита, включая основную сумму, срок, процентную ставку и график погашения.
 */
class Loan {
public:
    double principal;           /**< Основная сумма кредита */
    int days;                  /**< Общий срок кредита в днях */
    double dailyInterestRate;   /**< Дневная процентная ставка (по умолчанию 0.5%) */
    double dailyRepayment;     /**< Ежедневная сумма погашения */
    int daysLeft;              /**< Оставшиеся дни для погашения кредита */

    /**
     * @brief Создает объект кредита.
     * @param p Основная сумма кредита.
     * @param d Срок кредита в днях.
     * @param rate Дневная процентная ставка (по умолчанию 0.005).
     * @throws runtime_error Если срок кредита меньше или равен 0.
     */
    Loan(double p, int d, double rate = 0.005) : principal(p), days(d), dailyInterestRate(rate), daysLeft(d) {
        if (d <= 0) throw runtime_error("Срок кредита должен быть больше 0.");
        double totalInterest = principal * dailyInterestRate * days;
        double totalRepayment = principal + totalInterest;
        dailyRepayment = totalRepayment / days;
    }

    /**
     * @brief Вычисляет оставшийся долг.
     * @return Оставшийся долг на основе ежедневного платежа и оставшихся дней.
     */
    double getRemainingDebt() const { return dailyRepayment * daysLeft; }
};

/**
 * @class Animal
 * @brief Представляет животное в зоопарке.
 *
 * Хранит информацию о животном, включая вид, возраст, вес и состояние здоровья.
 * Поддерживает размножение через оператор +.
 */
class Animal {
private:
    string species;             /**< Вид животного (например, "Лев") */
    string displayName;         /**< Отображаемое имя животного */
    int ageDays;               /**< Возраст животного в днях */
    double weight;             /**< Вес животного в килограммах */
    Climate preferredClimate;   /**< Предпочитаемый климат животного */
    int price;                 /**< Стоимость покупки животного */
    AnimalType type;           /**< Тип животного (травоядное или хищник) */
    int enclosureId;           /**< Идентификатор вольера, в котором содержится животное */
    int daysSincePurchase;     /**< Дни с момента покупки животного */
    Gender gender;             /**< Пол животного */
    bool isBornInZoo;          /**< Истина, если животное родилось в зоопарке */
    pair<string, string> parents; /**< Имена родителей животного */
    bool isSick;               /**< Истина, если животное болеет */int uniqueId;              /**< Уникальный идентификатор животного */
    static int nextId;          /**< Статический счетчик для генерации уникальных идентификаторов */

public:
    /**
     * @brief Создает объект животного.
     * @param sp Название вида.
     * @param name Отображаемое имя.
     * @param age Возраст в днях.
     * @param w Вес в килограммах.
     * @param c Предпочитаемый климат.
     * @param p Стоимость покупки.
     * @param t Тип животного (травоядное или хищник).
     * @param g Пол животного.
     * @param born Истина, если родилось в зоопарке (по умолчанию false).
     * @param encId Идентификатор вольера (по умолчанию -1).
     * @param daysPurch Дни с момента покупки (по умолчанию 0).
     * @param par Имена родителей (по умолчанию {"None", "None"}).
     * @param sick Истина, если животное болеет (по умолчанию false).
     */
    Animal(string sp, string name, int age, double w, Climate c, int p, AnimalType t, Gender g, bool born = false,
        int encId = -1, int daysPurch = 0, pair<string, string> par = { "None", "None" }, bool sick = false)
        : species(sp), displayName(name), ageDays(age), weight(w), preferredClimate(c), price(p), type(t),
        enclosureId(encId), daysSincePurchase(daysPurch), gender(g), isBornInZoo(born), parents(par),
        isSick(sick), uniqueId(nextId++) {
    }

    /**
     * @brief Получает название вида.
     * @return Название вида.
     */
    string getSpecies() const { return species; }

    /**
     * @brief Получает отображаемое имя.
     * @return Отображаемое имя.
     */
    string getDisplayName() const { return displayName; }

    /**
     * @brief Получает возраст в днях.
     * @return Возраст в днях.
     */
    int getAgeDays() const { return ageDays; }

    /**
     * @brief Получает вес.
     * @return Вес в килограммах.
     */
    double getWeight() const { return weight; }

    /**
     * @brief Получает предпочитаемый климат.
     * @return Предпочитаемый климат.
     */
    Climate getPreferredClimate() const { return preferredClimate; }

    /**
     * @brief Получает стоимость покупки.
     * @return Стоимость покупки.
     */
    int getPrice() const { return price; }

    /**
     * @brief Получает тип животного.
     * @return Тип животного (травоядное или хищник).
     */
    AnimalType getType() const { return type; }

    /**
     * @brief Получает идентификатор вольера.
     * @return Идентификатор вольера.
     */
    int getEnclosureId() const { return enclosureId; }

    /**
     * @brief Получает дни с момента покупки.
     * @return Дни с момента покупки.
     */
    int getDaysSincePurchase() const { return daysSincePurchase; }

    /**
     * @brief Получает пол животного.
     * @return Пол животного.
     */
    Gender getGender() const { return gender; }

    /**
     * @brief Проверяет, родилось ли животное в зоопарке.
     * @return Истина, если родилось в зоопарке.
     */
    bool getIsBornInZoo() const { return isBornInZoo; }

    /**
     * @brief Получает имена родителей.
     * @return Пара имен родителей.
     */
    pair<string, string> getParents() const { return parents; }

    /**
     * @brief Проверяет, болеет ли животное.
     * @return Истина, если животное болеет.
     */
    bool getIsSick() const { return isSick; }

    /**
     * @brief Получает уникальный идентификатор.
     * @return Уникальный идентификатор.
     */
    int getUniqueId() const { return uniqueId; }

    /**
     * @brief Устанавливает идентификатор вольера.
     * @param id Новый идентификатор вольера.
     */
    void setEnclosureId(int id) { enclosureId = id; }

    /**
     * @brief Увеличивает дни с момента покупки.
     */
    void incrementDaysSincePurchase() { daysSincePurchase++; }

    /**
     * @brief Увеличивает возраст в днях.
     */
    void incrementAgeDays() { ageDays++; }

    /**
     * @brief Устанавливает отображаемое имя.
     * @param name Новое отображаемое имя.
     */
    void setDisplayName(const string& name) { displayName = name; }

    /**
     * @brief Устанавливает статус болезни.* @param sick Истина, чтобы отметить животное как больное.
     */
    void setSick(bool sick) { isSick = sick; }

    /**
     * @brief Размножает двух животных для создания новорожденного.
     * @param other Другое животное для размножения.
     * @return Новый объект Animal, представляющий новорожденного.
     * @throws runtime_error Если животные не в одном вольере, одного пола или слишком молоды (<= 5 дней).
     */
    Animal operator+(const Animal& other) const {
        if (enclosureId != other.enclosureId || gender == other.gender || ageDays <= 5 || other.ageDays <= 5) {
            throw runtime_error("Невозможно размножить: должны быть разных видов, противоположного пола, старше 5 дней и в одном вольере.");
        }
        string newSpecies = species.substr(0, species.length() / 2) + other.species.substr(other.species.length() / 2);
        string newName = newSpecies + "_Новорождённый";
        Gender newGender = (rand() % 2 == 0) ? Gender::MALE : Gender::FEMALE;
        double newWeight = (weight + other.weight) / 4;
        int newPrice = (price + other.price) / 2;
        return Animal(newSpecies, newName, 0, newWeight, preferredClimate, newPrice, type, newGender, true, enclosureId, 0, { displayName, other.displayName });
    }
};

/** @brief Статический счетчик для генерации уникальных идентификаторов животных. */
int Animal::nextId = 1;

/**
 * @class Enclosure
 * @brief Представляет вольер в зоопарке.
 *
 * Управляет коллекцией животных с определенной вместимостью, типом животных и климатом.
 */
class Enclosure {
private:
    int id;                    /**< Уникальный идентификатор вольера */
    int capacity;              /**< Максимальное количество животных в вольере */
    AnimalType animalType;     /**< Тип животных, разрешенных в вольере */
    Climate climate;           /**< Климат вольера */
    int dailyCost;             /**< Ежедневная стоимость содержания */
    vector<Animal> animals;    /**< Список животных в вольере */

public:
    /**
     * @brief Создает объект вольера.
     * @param i Идентификатор вольера.
     * @param cap Максимальная вместимость.
     * @param t Тип разрешенных животных.
     * @param c Климат вольера.
     * @param cost Ежедневная стоимость содержания.
     */
    Enclosure(int i, int cap, AnimalType t, Climate c, int cost)
        : id(i), capacity(cap), animalType(t), climate(c), dailyCost(cost) {
    }

    /**
     * @brief Получает идентификатор вольера.
     * @return Идентификатор вольера.
     */
    int getId() const { return id; }

    /**
     * @brief Получает вместимость.
     * @return Максимальное количество животных.
     */
    int getCapacity() const { return capacity; }

    /**
     * @brief Получает тип животных.
     * @return Тип разрешенных животных.
     */
    AnimalType getAnimalType() const { return animalType; }

    /**
     * @brief Получает климат.
     * @return Климат вольера.
     */
    Climate getClimate() const { return climate; }

    /**
     * @brief Получает ежедневную стоимость.
     * @return Ежедневная стоимость содержания.
     */
    int getDailyCost() const { return dailyCost; }

    /**
     * @brief Получает количество животных.
     * @return Количество животных в вольере.
     */
    size_t getAnimalCount() const { return animals.size(); }

    /**
     * @brief Получает список животных.
     * @return Ссылка на вектор животных.
     */
    const vector<Animal>& getAnimals() const { return animals; }

    /**
     * @brief Проверяет, можно ли добавить животное в вольер.
     * @param animal Животное для проверки.
     * @return Истина, если животное можно добавить (соответствует типу, климату и вместимость не превышена).
     */
    bool canAddAnimal(const Animal& animal) const {
        return animals.size() < static_cast<size_t>(capacity) &&
            animal.getType() == animalType &&
            animal.getPreferredClimate() == climate;
    }

    /**
     * @brief Добавляет животное в вольер.
     * @param animal Животное для добавления.
     */
    void addAnimal(const Animal& animal) {
        animals.push_back(animal);
    }

    /**
     * @brief Удаляет животное из вольера.
     * @param uniqueId Уникальный идентификатор животного для удаления.
     */
    void removeAnimal(int uniqueId) {
        animals.erase(remove_if(animals.begin(), animals.end(), [uniqueId](const Animal& a) { return a.getUniqueId() == uniqueId; }),
            animals.end());
    }

    /**
     * @brief Обновляет животное в вольере.
     * @param updatedAnimal Обновленный объект животного.
     */
    void updateAnimal(const Animal& updatedAnimal) {
        for (auto& animal : animals) {
            if (animal.getUniqueId() == updatedAnimal.getUniqueId()) {
                animal = updatedAnimal;
                return;
            }
        }
    }
};

/**
 * @class Worker
 * @brief Представляет работника зоопарка.
 *
 * Управляет информацией о работнике, включая роль, зарплату и назначенные вольеры.
 */
class Worker {
private:
    string name;                   /**< Имя работника */
    WorkerType type;               /**< Тип работника (директор, ветеринар и т.д.) */
    int salary;                    /**< Ежедневная зарплата */
    vector<int> assignedEnclosures;/**< Идентификаторы назначенных вольеров */
    int daysAssigned;              /**< Количество дней назначения на вольеры */
    int daysWorked;                /**< Общее количество отработанных дней */
    int maxAnimals;                /**< Максимальное количество животных для ветеринара (0 для других) */

public:
    /**
     * @brief Создает объект работника.
     * @param n Имя работника.
     * @param t Тип работника.
     * @param sal Ежедневная зарплата.
     * @param maxA Максимальное количество животных (для ветеринаров, по умолчанию 0).
     * @param encs Идентификаторы назначенных вольеров (по умолчанию пусто).
     * @param daysAss Дни назначения на вольеры (по умолчанию 0).
     * @param daysW Общее количество отработанных дней (по умолчанию 0).
     */
    Worker(string n, WorkerType t, int sal, int maxA = 0, vector<int> encs = {}, int daysAss = 0, int daysW = 0)
        : name(n), type(t), salary(sal), assignedEnclosures(encs), daysAssigned(daysAss), daysWorked(daysW), maxAnimals(maxA) {
    }

    /**
     * @brief Получает зарплату для типа работника.
     * @param t Тип работника.
     * @return Ежедневная зарплата для типа.
     */
    static int getSalaryForType(WorkerType t) {
        switch (t) {
        case WorkerType::DIRECTOR: return 60;
        case WorkerType::VETERINARIAN: return 50;
        case WorkerType::CLEANER: return 30;
        case WorkerType::FEEDER: return 40;
        default: return 0;
        }
    }

    /**
     * @brief Получает имя работника.
     * @return Имя работника.
     */
    string getName() const { return name; }

    /**
     * @brief Получает тип работника.
     * @return Тип работника.
     */
    WorkerType getType() const { return type; }

    /**
     * @brief Получает ежедневную зарплату.
     * @return Ежедневная зарплата.
     */
    int getSalary() const { return salary; }

    /**
     * @brief Получает идентификаторы назначенных вольеров.
     * @return Ссылка на вектор идентификаторов вольеров.
     */
    const vector<int>& getAssignedEnclosures() const { return assignedEnclosures; }

    /**
     * @brief Получает количество дней назначения.
     * @return Дни назначения.
     */
    int getDaysAssigned() const { return daysAssigned; }

    /**
     * @brief Получает общее количество отработанных дней.
     * @return Отработанные дни.
     */
    int getDaysWorked() const { return daysWorked; }

    /**
     * @brief Получает максимальное количество животных (для ветеринаров).
     * @return Максимальное количество животных.
     */
    int getMaxAnimals() const { return maxAnimals; }

    /**
     * @brief Получает строковое представление типа работника.
     * @return Тип работника в виде строки (например, "Директор").
     */
    string getTypeString() const {
        switch (type) {
        case WorkerType::DIRECTOR: return "Директор";
        case WorkerType::VETERINARIAN: return "Ветеринар";case WorkerType::CLEANER: return "Уборщик";
        case WorkerType::FEEDER: return "Кормилец";
        default: return "Неизвестно";
        }
    }

    /**
     * @brief Назначает вольер работнику.
     * @param encId Идентификатор вольера для назначения.
     */
    void assignEnclosure(int encId) {
        if (find(assignedEnclosures.begin(), assignedEnclosures.end(), encId) == assignedEnclosures.end()) {
            assignedEnclosures.push_back(encId);
        }
    }

    /**
     * @brief Очищает все назначенные вольеры.
     */
    void clearAssignedEnclosures() { assignedEnclosures.clear(); }

    /**
     * @brief Устанавливает количество дней назначения.
     * @param days Количество дней.
     */
    void setDaysAssigned(int days) { daysAssigned = days; }

    /**
     * @brief Уменьшает дни назначения (если положительны).
     */
    void decrementDaysAssigned() { if (daysAssigned > 0) daysAssigned--; }

    /**
     * @brief Увеличивает общее количество отработанных дней.
     */
    void incrementDaysWorked() { daysWorked++; }
};

/**
 * @class Zoo
 * @brief Представляет зоопарк и его операции.
 *
 * Управляет всеми аспектами зоопарка, включая животных, вольеры, работников, финансы и прогресс игры.
 */
class Zoo {
private:
    string name;                   /**< Название зоопарка */
    double money;                  /**< Текущие денежные средства */
    int food;                      /**< Доступные единицы еды */
    double popularity;             /**< Очки популярности */
    vector<Animal> animals;        /**< Список животных в зоопарке */
    vector<Enclosure> enclosures;  /**< Список вольеров */
    vector<Worker> workers;        /**< Список работников */
    vector<Loan> loans;            /**< Список активных кредитов */
    int day;                       /**< Текущий день в игре */
    int visitors;                  /**< Количество посетителей сегодня */
    int totalAnimals;              /**< Общее количество животных */
    string specialVisitorType;     /**< Тип особых посетителей (например, "Знаменитость") */
    int specialVisitorCount;       /**< Количество особых посетителей */
    vector<Animal> marketAnimals;  /**< Животные, доступные для покупки */
    int animalsBoughtToday;        /**< Животные, купленные сегодня */

    /**
     * @brief Генерирует случайное число в диапазоне.
     * @param min Минимальное значение (включительно).
     * @param max Максимальное значение (включительно).
     * @return Случайное целое число.
     */
    static int random(int min, int max) { return min + rand() % (max - min + 1); }

    /**
     * @brief Получает допустимый ввод пользователя в диапазоне.
     * @param prompt Приглашение для ввода.
     * @param minVal Минимальное допустимое значение.
     * @param maxVal Максимальное допустимое значение.
     * @return Допустимое целое число.
     */
    int getValidInput(const string& prompt, int minVal, int maxVal) const {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value && value >= minVal && value <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            else {
                cout << "Некорректный ввод. Введите число от " << minVal << " до " << maxVal << ".\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    /**
     * @brief Обновляет рынок животных новыми животными.
     */
    void refreshMarket() {
        marketAnimals.clear();
        auto available = getAvailableAnimals();
        vector<int> indices(available.size());
        iota(indices.begin(), indices.end(), 0);

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indices.begin(), indices.end(), g); // Исправлено

        for (int i = 0; i < min(10, (int)available.size()); ++i) {
            marketAnimals.push_back(available[indices[i]]);
        }
    }

public:
    /**
     * @brief Создает объект зоопарка.* @param n Название зоопарка.
     */
    Zoo(const string& n) : name(n), money(1488), food(100), popularity(50.0), day(1), visitors(0), totalAnimals(0),
        specialVisitorType("None"), specialVisitorCount(0), animalsBoughtToday(0) {
        workers.emplace_back("К.З", WorkerType::DIRECTOR, Worker::getSalaryForType(WorkerType::DIRECTOR));
        workers.emplace_back("тринити", WorkerType::CLEANER, Worker::getSalaryForType(WorkerType::CLEANER), 0, vector<int>{1});
        workers.emplace_back("морф", WorkerType::VETERINARIAN, Worker::getSalaryForType(WorkerType::VETERINARIAN), 20);
        workers.emplace_back("диференс", WorkerType::FEEDER, Worker::getSalaryForType(WorkerType::FEEDER), 0, vector<int>{2});
        enclosures.emplace_back(1, 5, AnimalType::HERBIVORE, Climate::TEMPERATE, 10);
        refreshMarket();
    }

    /**
     * @brief Получает текущие денежные средства.
     * @return Сумма денег.
     */
    double getMoney() const { return money; }

    /**
     * @brief Получает доступные единицы еды.
     * @return Единицы еды.
     */
    int getFood() const { return food; }

    /**
     * @brief Получает очки популярности.
     * @return Очки популярности.
     */
    double getPopularity() const { return popularity; }

    /**
     * @brief Получает текущий день.
     * @return Текущий день.
     */
    int getDay() const { return day; }

    /**
     * @brief Получает количество посетителей сегодня.
     * @return Количество посетителей.
     */
    int getVisitors() const { return visitors; }

    /**
     * @brief Получает общее количество животных.
     * @return Общее количество животных.
     */
    int getTotalAnimals() const { return totalAnimals; }

    /**
     * @brief Получает список животных.
     * @return Ссылка на вектор животных.
     */
    const vector<Animal>& getAnimals() const { return animals; }

    /**
     * @brief Получает список вольеров.
     * @return Ссылка на вектор вольеров.
     */
    const vector<Enclosure>& getEnclosures() const { return enclosures; }

    /**
     * @brief Получает список работников.
     * @return Ссылка на вектор работников.
     */
    const vector<Worker>& getWorkers() const { return workers; }

    /**
     * @brief Отображает текущий статус зоопарка.
     */
    void displayStatus() const {
        cout << "\n--- Статус зоопарка \"" << name << "\" (День " << day << ") ---\n";
        cout << "Деньги: $" << money << endl;
        cout << "Еда: " << food << " единиц" << endl;
        cout << "Популярность: " << popularity << endl;
        cout << "Всего животных: " << totalAnimals << endl;
        cout << "Посетителей сегодня: " << visitors << endl;
        if (specialVisitorType != "None") {
            cout << "Особые гости: " << specialVisitorCount << " " << (specialVisitorType == "Celebrity" ? "Знаменитостей" : "Фотографов") << endl;
        }
        cout << "Работников: " << workers.size() << endl;
        cout << "Вольеров: " << enclosures.size() << endl;
    }

    /**
     * @brief Возвращает список животных, доступных для покупки.
     * @return Вектор доступных животных.
     */
    static vector<Animal> getAvailableAnimals() {
        return {
            {"Олень", "Олень", 10, 200, Climate::TEMPERATE, 150, AnimalType::HERBIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Слон", "Слон", 15, 6000, Climate::TROPICAL, 350, AnimalType::HERBIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Жираф", "Жираф", 12, 1800, Climate::TROPICAL, 300, AnimalType::HERBIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Зебра", "Зебра", 8, 400, Climate::TROPICAL, 200, AnimalType::HERBIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Кролик", "Кролик", 3, 5, Climate::TEMPERATE, 100, AnimalType::HERBIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Лев", "Лев", 10, 300, Climate::TROPICAL, 400, AnimalType::CARNIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},{"Волк", "Волк", 7, 150, Climate::TEMPERATE, 250, AnimalType::CARNIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Белый медведь", "Белый медведь", 14, 800, Climate::ARCTIC, 450, AnimalType::CARNIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Тигр", "Тигр", 9, 350, Climate::TROPICAL, 350, AnimalType::CARNIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)},
            {"Лисица", "Лисица", 5, 100, Climate::TEMPERATE, 200, AnimalType::CARNIVORE, (rand() % 2 ? Gender::MALE : Gender::FEMALE)}
        };
    }

    /**
     * @brief Управляет операциями с животными (покупка, продажа, переименование и т.д.).
     */
    void manageAnimals() {
        while (true) {
            string prompt = "\nУправление животными:\n"
                "1. Купить животное\n"
                "2. Продать животное\n"
                "3. Просмотреть информацию о животных\n"
                "4. Переименовать животное\n"
                "5. Обновить рынок животных ($50)\n"
                "6. Назад\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 6);
            if (choice == 1) {
                if (day > 10 && animalsBoughtToday >= 1) {
                    cout << "После 10-го дня можно купить только одно животное в день.\n";
                    continue;
                }
                if (marketAnimals.empty()) {
                    cout << "Рынок пуст. Обновите рынок.\n";
                    continue;
                }
                cout << "\nДоступные животные для покупки:\n";
                for (size_t i = 0; i < marketAnimals.size(); ++i) {
                    cout << i + 1 << ". " << marketAnimals[i].getSpecies()
                        << " (" << marketAnimals[i].getDisplayName() << "), Цена: $" << marketAnimals[i].getPrice()
                        << ", Пол: " << (marketAnimals[i].getGender() == Gender::MALE ? "М" : "Ж")
                        << ", Климат: ";
                    switch (marketAnimals[i].getPreferredClimate()) {
                    case Climate::TROPICAL: cout << "Тропический"; break;
                    case Climate::TEMPERATE: cout << "Умеренный"; break;
                    case Climate::ARCTIC: cout << "Арктический"; break;
                    }
                    cout << ", Тип: " << (marketAnimals[i].getType() == AnimalType::HERBIVORE ? "Травоядное" : "Хищник") << "\n";
                }
                int animalChoice = getValidInput("Выберите животное для покупки (1-" + to_string(marketAnimals.size()) + ") или 0 для отмены: ", 0, marketAnimals.size());
                if (animalChoice >= 1 && animalChoice <= static_cast<int>(marketAnimals.size())) {
                    Animal selected = marketAnimals[animalChoice - 1];
                    if (money >= selected.getPrice()) {
                        cout << "Выберите вольер (ID) для " << selected.getDisplayName() << ":\n";
                        bool validEnclosure = false;
                        vector<int> validEnclosureIds;
                        for (const auto& enc : enclosures) {
                            if (enc.canAddAnimal(selected)) {
                                cout << "ID " << enc.getId() << " (" << enc.getAnimalCount() << "/" << enc.getCapacity() << " животных)\n";
                                validEnclosureIds.push_back(enc.getId());
                                validEnclosure = true;
                            }
                        }
                        if (!validEnclosure) {
                            cout << "Нет подходящих вольеров для этого животного.\n";
                            continue;
                        }
                        int encId = getValidInput("Введите ID вольера: ", 1, enclosures.back().getId());
                        bool placed = false;
                        for (auto& enc : enclosures) {
                            if (enc.getId() == encId && enc.canAddAnimal(selected)) {
                                selected.setEnclosureId(encId);
                                enc.addAnimal(selected);
                                animals.push_back(selected);
                                money -= selected.getPrice();
                                totalAnimals++;
                                animalsBoughtToday++;
                                marketAnimals.erase(marketAnimals.begin() + (animalChoice - 1));
                                cout << selected.getDisplayName() << " куплено и размещено в вольере " << encId << ".\n";
                                placed = true;
                                break;
                            }
                        }
                        if (!placed) cout << "Неверный ID вольера или неподходящий вольер.\n";
                    }
                    else cout << "Недостаточно денег!\n";
                }
            }
            else if (choice == 2) {
                if (animals.empty()) {
                    cout << "Нет животных для продажи.\n";
                    continue;
                }
                for (size_t i = 0; i < animals.size(); ++i) {
                    cout << i + 1 << ". " << animals[i].getSpecies() << " (" << animals[i].getDisplayName() << "), ID вольера: " << animals[i].getEnclosureId() << "\n";
                }
                int sellChoice = getValidInput("Выберите животное для продажи (1-" + to_string(animals.size()) + ") или 0 для отмены: ", 0, animals.size());
                if (sellChoice >= 1 && sellChoice <= static_cast<int>(animals.size())) {
                    Animal sold = animals[sellChoice - 1];
                    money += sold.getPrice() / 2;
                    for (auto& enc : enclosures) {
                        if (enc.getId() == sold.getEnclosureId()) {
                            enc.removeAnimal(sold.getUniqueId());
                            break;
                        }
                    }
                    animals.erase(animals.begin() + (sellChoice - 1));
                    totalAnimals--;
                    cout << sold.getDisplayName() << " продано за $" << sold.getPrice() / 2 << ".\n";
                }
            }
            else if (choice == 3) {
                if (animals.empty()) {
                    cout << "В зоопарке нет животных.\n";
                    continue;
                }
                cout << "\nИнформация о животных:\n";
                for (const auto& animal : animals) {
                    cout << "Вид: " << animal.getSpecies() << ", Имя: " << animal.getDisplayName()
                        << ", Возраст: " << animal.getAgeDays() << " дней"
                        << ", Пол: " << (animal.getGender() == Gender::MALE ? "М" : "Ж")
                        << ", Вес: " << animal.getWeight() << " кг"
                        << ", Климат: ";
                    switch (animal.getPreferredClimate()) {
                    case Climate::TROPICAL: cout << "Тропический"; break;
                    case Climate::TEMPERATE: cout << "Умеренный"; break;
                    case Climate::ARCTIC: cout << "Арктический"; break;
                    }
                    cout << ", Тип: " << (animal.getType() == AnimalType::HERBIVORE ? "Травоядное" : "Хищник")
                        << ", ID вольера: " << animal.getEnclosureId() << ", Дней с покупки: " << animal.getDaysSincePurchase()
                        << ", Болен: " << (animal.getIsSick() ? "Да" : "Нет");
                    if (animal.getIsBornInZoo()) {
                        cout << ", Родители: " << animal.getParents().first << " и " << animal.getParents().second;
                    }
                    cout << "\n";
                }
            }
            else if (choice == 4) {
                if (animals.empty()) {
                    cout << "В зоопарке нет животных.\n";
                    continue;
                }
                for (size_t i = 0; i < animals.size(); ++i) {
                    cout << i + 1 << ". " << animals[i].getSpecies() << " (" << animals[i].getDisplayName() << "), ID вольера: " << animals[i].getEnclosureId() << "\n";
                }int renameChoice = getValidInput("Выберите животное для переименования (1-" + to_string(animals.size()) + ") или 0 для отмены: ", 0, animals.size());
                if (renameChoice >= 1 && renameChoice <= static_cast<int>(animals.size())) {
                    string newName;
                    cin.ignore();
                    cout << "Введите новое имя для " << animals[renameChoice - 1].getDisplayName() << ": ";
                    getline(cin, newName);
                    if (!newName.empty()) {
                        animals[renameChoice - 1].setDisplayName(newName);
                        for (auto& enc : enclosures) {
                            if (enc.getId() == animals[renameChoice - 1].getEnclosureId()) {
                                enc.updateAnimal(animals[renameChoice - 1]);
                                break;
                            }
                        }
                        cout << "Животное переименовано в " << newName << ".\n";
                    }
                    else cout << "Имя не может быть пустым.\n";
                }
            }
            else if (choice == 5) {
                if (money >= 50) {
                    money -= 50;
                    refreshMarket();
                    cout << "Рынок животных обновлён за $50.\n";
                }
                else cout << "Недостаточно денег для обновления рынка.\n";
            }
            else if (choice == 6) break;
        }
    }

    /**
     * @brief Управляет операциями с работниками (найм, увольнение, назначение на вольеры).
     */
    void manageWorkers() {
        while (true) {
            string prompt = "\nУправление работниками:\n"
                "1. Нанять работника\n"
                "2. Просмотреть работников\n"
                "3. Уволить работника\n"
                "4. Назначить работника на вольер\n"
                "5. Назад\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 5);
            if (choice == 1) {
                string name;
                cin.ignore();
                while (true) {
                    cout << "Введите имя работника: ";
                    getline(cin, name);
                    if (!name.empty()) break;
                    cout << "Имя работника не может быть пустым. Попробуйте снова.\n";
                }
                cout << "Выберите должность:\n";
                cout << "1. Ветеринар (до 20 животных)\n";
                cout << "2. Уборщик (1 вольер)\n";
                cout << "3. Кормильщик (до 2 вольеров)\n";
                int posChoice = getValidInput("Выберите должность (1-3): ", 1, 3);
                WorkerType position;
                int maxAnimals = 0;
                switch (posChoice) {
                case 1: position = WorkerType::VETERINARIAN; maxAnimals = 20; break;
                case 2: position = WorkerType::CLEANER; maxAnimals = 0; break;
                case 3: position = WorkerType::FEEDER; maxAnimals = 0; break;
                default: position = WorkerType::CLEANER;
                }
                int salary = Worker::getSalaryForType(position);
                vector<int> enclosureIds;
                Worker newWorker(name, position, salary, maxAnimals, enclosureIds);
                cout << name << " нанят как " << newWorker.getTypeString() << ".\n";
                if (enclosures.empty()) {
                    cout << "Нет вольеров для назначения.\n";
                }
                else {
                    if (position == WorkerType::CLEANER) {
                        cout << "Назначьте 1 вольер для уборщика:\n";
                        for (const auto& enc : enclosures) {
                            cout << "ID " << enc.getId() << " (" << enc.getAnimalCount() << "/" << enc.getCapacity() << " животных)\n";
                        }
                        int encId = getValidInput("Введите ID вольера: ", 1, enclosures.back().getId());
                        bool valid = false;
                        for (const auto& enc : enclosures) {
                            if (enc.getId() == encId) {
                                newWorker.assignEnclosure(encId);
                                valid = true;
                                break;
                            }
                        }
                        if (!valid) cout << "Неверный ID вольера. Назначение отменено.\n";
                    }
                    else if (position == WorkerType::FEEDER) {
                        cout << "Назначьте до 2 вольеров для кормильца (введите ID или 0 для завершения):\n";
                        for (int i = 0; i < 2; ++i) {
                            for (const auto& enc : enclosures) {
                                cout << "ID " << enc.getId() << " (" << enc.getAnimalCount() << "/" << enc.getCapacity() << " животных)\n";
                            }
                            int encId = getValidInput("Введите ID вольера (0 для завершения): ", 0, enclosures.back().getId());
                            if (encId == 0) break;
                            bool valid = false;
                            for (const auto& enc : enclosures) {
                                if (enc.getId() == encId) {
                                    newWorker.assignEnclosure(encId);
                                    valid = true;
                                    break;
                                }
                            }
                            if (!valid) cout << "Неверный ID вольера.\n";
                        }
                    }
                    else if (position == WorkerType::VETERINARIAN) {
                        int totalAnimalsAssigned = 0;
                        cout << "Назначайте вольеры для ветеринара (до 20 животных). Введите ID или 0 для завершения:\n";
                        while (true) {
                            for (const auto& enc : enclosures) {
                                cout << "ID " << enc.getId() << " (" << enc.getAnimalCount() << "/" << enc.getCapacity() << " животных)\n";
                            }
                            int encId = getValidInput("Введите ID вольера (0 для завершения): ", 0, enclosures.back().getId());
                            if (encId == 0) break;
                            bool valid = false;
                            for (const auto& enc : enclosures) {
                                if (enc.getId() == encId) {
                                    int animalCount = enc.getAnimalCount();
                                    if (totalAnimalsAssigned + animalCount > 20) {
                                        cout << "Превышен лимит в 20 животных.\n";
                                    }
                                    else {
                                        newWorker.assignEnclosure(encId);
                                        totalAnimalsAssigned += animalCount;
                                        cout << "Вольер " << encId << " назначен. Всего животных: " << totalAnimalsAssigned << "\n";
                                    }
                                    valid = true;
                                    break;
                                }
                            }
                            if (!valid) cout << "Неверный ID вольера.\n";
                        }
                    }
                }
                workers.push_back(newWorker);
            }
            else if (choice == 2) {
                if (workers.empty()) {
                    cout << "В зоопарке нет работников.\n";
                    continue;
                }
                cout << "\nИнформация о работниках:\n";
                for (size_t i = 0; i < workers.size(); ++i) {
                    const auto& worker = workers[i];
                    cout << i + 1 << ". Имя: " << worker.getName()
                        << ", Должность: " << worker.getTypeString()
                        << ", Зарплата: $" << worker.getSalary()
                        << ", Дней проработано: " << worker.getDaysWorked();
                    if (worker.getType() == WorkerType::VETERINARIAN) {
                        cout << ", Управляемых животных: " << worker.getMaxAnimals();
                    }
                    cout << ", Вольеры: ";
                    const auto& encIds = worker.getAssignedEnclosures();
                    if (encIds.empty()) cout << "Нет";
                    else {
                        for (size_t j = 0; j < encIds.size(); ++j) {
                            cout << encIds[j];
                            if (j < encIds.size() - 1) cout << ", ";
                        }
                    }
                    cout << ", Дней назначения: " << worker.getDaysAssigned() << "\n";
                }
            }
            else if (choice == 3) {
                if (workers.size() <= 1) {
                    cout << "Нельзя уволить работников. Директор должен остаться.\n";
                    continue;
                }
                cout << "\nВыберите работника для увольнения:\n";
                for (size_t i = 0; i < workers.size(); ++i) {
                    const auto& worker = workers[i];
                    cout << i + 1 << ". Имя: " << worker.getName() << ", Должность: " << worker.getTypeString() << "\n";
                }
                int fireChoice = getValidInput("Выберите работника (1-" + to_string(workers.size()) + ") или 0 для отмены: ", 0, workers.size());
                if (fireChoice >= 1 && fireChoice <= static_cast<int>(workers.size())) {
                    if (workers[fireChoice - 1].getType() == WorkerType::DIRECTOR) {
                        cout << "Нельзя уволить директора.\n";
                    }
                    else {
                        string firedName = workers[fireChoice - 1].getName();
                        workers.erase(workers.begin() + (fireChoice - 1));
                        cout << firedName << " уволен.\n";
                    }
                }
            }
            else if (choice == 4) {
                if (workers.empty()) {
                    cout << "В зоопарке нет работников.\n";
                    continue;
                }
                if (enclosures.empty()) {
                    cout << "В зоопарке нет вольеров.\n";
                    continue;
                }
                cout << "\nВыберите работника для назначения:\n";
                for (size_t i = 0; i < workers.size(); ++i) {
                    const auto& worker = workers[i];
                    cout << i + 1 << ". Имя: " << worker.getName() << ", Должность: " << worker.getTypeString() << "\n";
                }
                int workerChoice = getValidInput("Выберите работника (1-" + to_string(workers.size()) + ") или 0 для отмены: ", 0, workers.size());
                if (workerChoice == 0) continue;
                if (workerChoice < 1 || workerChoice > static_cast<int>(workers.size())) {
                    cout << "Неверный выбор работника.\n";
                    continue;
                }
                Worker& selectedWorker = workers[workerChoice - 1];
                if (selectedWorker.getType() == WorkerType::DIRECTOR) {
                    cout << "Директор не может быть назначен на вольеры.\n";
                    continue;
                }
                cout << "\nВыберите вольер для назначения:\n";
                for (const auto& enc : enclosures) {
                    cout << "ID " << enc.getId() << " (" << enc.getAnimalCount() << "/" << enc.getCapacity() << " животных)\n";
                }
                int encId = getValidInput("Введите ID вольера (0 для отмены): ", 0, enclosures.back().getId());
                if (encId == 0) continue;
                bool validEnclosure = false;
                for (const auto& enc : enclosures) {
                    if (enc.getId() == encId) {
                        validEnclosure = true;
                        break;
                    }
                }
                if (!validEnclosure) {
                    cout << "Неверный ID вольера.\n";
                    continue;
                }if (find(selectedWorker.getAssignedEnclosures().begin(), selectedWorker.getAssignedEnclosures().end(), encId) != selectedWorker.getAssignedEnclosures().end()) {
                    cout << "Работник уже назначен на этот вольер.\n";
                    continue;
                }
                int maxEnclosures = (selectedWorker.getType() == WorkerType::CLEANER) ? 1 : ((selectedWorker.getType() == WorkerType::FEEDER) ? 2 : numeric_limits<int>::max());
                if (selectedWorker.getAssignedEnclosures().size() >= static_cast<size_t>(maxEnclosures)) {
                    cout << "Этот работник уже назначен на максимальное количество вольеров.\n";
                    continue;
                }
                if (selectedWorker.getType() == WorkerType::VETERINARIAN) {
                    int totalAnimals = 0;
                    for (const auto& encIdAssigned : selectedWorker.getAssignedEnclosures()) {
                        for (const auto& enc : enclosures) {
                            if (enc.getId() == encIdAssigned) {
                                totalAnimals += enc.getAnimalCount();
                                break;
                            }
                        }
                    }
                    for (const auto& enc : enclosures) {
                        if (enc.getId() == encId) {
                            if (totalAnimals + enc.getAnimalCount() > selectedWorker.getMaxAnimals()) {
                                cout << "Назначение этого вольера приведет к превышению лимита в 20 животных.\n";
                                continue;
                            }
                            break;
                        }
                    }
                }
                int daysAssigned = getValidInput("Введите количество дней назначения: ", 1, 365);
                selectedWorker.assignEnclosure(encId);
                selectedWorker.setDaysAssigned(daysAssigned);
                cout << selectedWorker.getName() << " назначен на вольер " << encId << " на " << daysAssigned << " дней.\n";
            }
            else if (choice == 5) break;
        }
    }

    /**
     * @brief Управляет операциями по покупкам (еда, реклама, кредиты).
     */
    void managePurchases() {
        while (true) {
            string prompt = "\nУправление покупками:\n"
                "1. Купить еду\n"
                "2. Потратить на рекламу\n"
                "3. Взять кредит\n"
                "4. Просмотреть кредиты\n"
                "5. Назад\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 5);

            if (choice == 1) {
                int foodAmount = getValidInput("Введите количество еды для покупки ($2 за единицу): ", 0, 10000);
                if (money >= foodAmount * 2) {
                    food += foodAmount;
                    money -= foodAmount * 2;
                    cout << foodAmount << " единиц еды куплено.\n";
                }
                else cout << "Недостаточно денег!\n";
            }
            else if (choice == 2) {
                int adSpend = getValidInput("Введите сумму для рекламы ($200 = +5 популярности): ", 0, 10000);
                if (money >= adSpend) {
                    popularity += (adSpend / 200) * 5;
                    money -= adSpend;
                    cout << "Популярность увеличена на " << (adSpend / 200) * 5 << ".\n";
                }
                else cout << "Недостаточно денег!\n";
            }
            else if (choice == 3) {
                int amount = getValidInput("Введите сумму кредита: ", 1, 1000000);
                int days = getValidInput("Введите количество дней для погашения (1-20): ", 1, 20);
                loans.emplace_back(static_cast<double>(amount), days);
                money += amount;
                cout << "Кредит на $" << amount << " взят на " << days << " дней с дневной процентной ставкой 0.5%.\n";
            }
            else if (choice == 4) {
                if (loans.empty()) cout << "\nУ вас нет активных кредитов.\n";else {
                    cout << "\nТекущие кредиты:\n";
                    for (size_t i = 0; i < loans.size(); ++i) {
                        const auto& loan = loans[i];
                        cout << i + 1 << ". Сумма: $" << loan.principal
                            << ", Дневная процентная ставка: " << (loan.dailyInterestRate * 100) << "%"
                            << ", Осталось дней: " << loan.daysLeft << ", Ежедневный платеж: $" << loan.dailyRepayment
                            << ", Остаток долга: $" << loan.getRemainingDebt() << "\n";
                    }
                }
            }
            else if (choice == 5) break;
        }
    }

    /**
     * @brief Управляет операциями с вольерами (строительство и просмотр).
     */
    void manageEnclosures() {
        while (true) {
            string prompt = "\nУправление вольерами:\n"
                "1. Построить новый вольер\n"
                "2. Просмотреть вольеры\n"
                "3. Назад\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 3);
            if (choice == 1) {
                int capacity = getValidInput("Введите вместимость (макс. животных): ", 1, 100);
                int typeChoice = getValidInput("Выберите тип животных (1: Травоядные, 2: Хищники): ", 1, 2);
                AnimalType animalType = (typeChoice == 1) ? AnimalType::HERBIVORE : AnimalType::CARNIVORE;
                int climateChoice = getValidInput("Выберите климат (1: Тропический, 2: Умеренный, 3: Арктический): ", 1, 3);
                Climate climate;
                switch (climateChoice) {
                case 1: climate = Climate::TROPICAL; break;
                case 2: climate = Climate::TEMPERATE; break;
                case 3: climate = Climate::ARCTIC; break;
                default: climate = Climate::TEMPERATE;
                }
                int cost = capacity * 50;
                if (money >= cost) {
                    int newId = enclosures.empty() ? 1 : enclosures.back().getId() + 1;
                    enclosures.emplace_back(newId, capacity, animalType, climate, capacity * 2);
                    money -= cost;
                    cout << "Вольер " << newId << " построен за $" << cost << ".\n";
                }
                else cout << "Недостаточно денег!\n";
            }
            else if (choice == 2) {
                if (enclosures.empty()) {
                    cout << "В зоопарке нет вольеров.\n";
                    continue;
                }
                cout << "\nВольеры:\n";
                for (const auto& enc : enclosures) {
                    cout << "ID: " << enc.getId()
                        << ", Вместимость: " << enc.getCapacity()
                        << ", Животных: " << enc.getAnimalCount()
                        << ", Тип: " << (enc.getAnimalType() == AnimalType::HERBIVORE ? "Травоядные" : "Хищники")
                        << ", Климат: ";
                    switch (enc.getClimate()) {
                    case Climate::TROPICAL: cout << "Тропический"; break;
                    case Climate::TEMPERATE: cout << "Умеренный"; break;
                    case Climate::ARCTIC: cout << "Арктический"; break;
                    }
                    cout << ", Ежедневная стоимость: $" << enc.getDailyCost() << "\n";
                }
            }
            else if (choice == 3) break;
        }
    }

    /**
     * @brief Управляет операциями по размножению животных.
     */
    void manageBreeding() {
        while (true) {
            string prompt = "\nУправление размножением:\n"
                "1. Размножить животных\n"
                "2. Назад\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 2);
            if (choice == 1) {
                if (animals.size() < 2) {
                    cout << "Недостаточно животных для размножения.\n";
                    continue;
                }
                cout << "\nВыберите двух животных для размножения:\n";for (size_t i = 0; i < animals.size(); ++i) {
                    cout << i + 1 << ". " << animals[i].getSpecies() << " (" << animals[i].getDisplayName()
                        << "), Пол: " << (animals[i].getGender() == Gender::MALE ? "М" : "Ж")
                        << ", ID вольера: " << animals[i].getEnclosureId() << "\n";
                }
                int first = getValidInput("Выберите первое животное (1-" + to_string(animals.size()) + ") или 0 для отмены: ", 0, animals.size());
                if (first == 0) continue;
                int second = getValidInput("Выберите второе животное (1-" + to_string(animals.size()) + ") или 0 для отмены: ", 0, animals.size());
                if (second == 0) continue;
                if (first == second) {
                    cout << "Нельзя выбрать одно и то же животное.\n";
                    continue;
                }
                first--; second--;
                if (animals[first].getEnclosureId() != animals[second].getEnclosureId()) {
                    cout << "Животные должны быть в одном вольере для размножения.\n";
                    continue;
                }
                bool canAdd = false;
                for (auto& enc : enclosures) {
                    if (enc.getId() == animals[first].getEnclosureId() && enc.canAddAnimal(animals[first])) {
                        canAdd = true;
                        break;
                    }
                }
                if (!canAdd) {
                    cout << "Нет свободного места в вольере для новорожденного.\n";
                    continue;
                }
                try {
                    Animal newborn = animals[first] + animals[second];
                    for (auto& enc : enclosures) {
                        if (enc.getId() == newborn.getEnclosureId()) {
                            enc.addAnimal(newborn);
                            break;
                        }
                    }
                    animals.push_back(newborn);
                    totalAnimals++;
                    cout << "Новое животное родилось: " << newborn.getSpecies() << " (" << newborn.getDisplayName() << ").\n";
                }
                catch (const runtime_error& e) {
                    cout << e.what() << "\n";
                }
            }
            else break;
        }
    }

    /**
     * @brief Переходит к следующему дню, обновляя все операции зоопарка.
     *
     * Обновляет возраст животных, здоровье, пожертвования, количество посетителей и финансовые транзакции.
     * Обрабатывает случайные события, такие как болезни и смерть.
     */
    void nextDay() {
        day++;
        animalsBoughtToday = 0;
        refreshMarket();

        specialVisitorType = "None";
        specialVisitorCount = 0;

        for (auto it = animals.begin(); it != animals.end();) {
            it->incrementDaysSincePurchase();
            it->incrementAgeDays();
            if (it->getAgeDays() > 30 && random(0, 99) < it->getAgeDays()) {
                cout << it->getDisplayName() << " умерло от старости.\n";
                for (auto& enc : enclosures) {
                    if (enc.getId() == it->getEnclosureId()) {
                        enc.removeAnimal(it->getUniqueId());
                        break;
                    }
                }
                it = animals.erase(it);
                totalAnimals--;
            }
            else ++it;
        }
        for (auto& worker : workers) { // Исправлено: workers вместо work ers
            worker.incrementDaysWorked();
            worker.decrementDaysAssigned();
            if (worker.getDaysAssigned() == 0) worker.clearAssignedEnclosures();
        }
        for (auto& animal : animals) {
            if (!animal.getIsSick() && random(0, 99) < 10) {
                animal.setSick(true);
            }
        }

        for (auto& worker : workers) {
            if (worker.getType() == WorkerType::VETERINARIAN && worker.getDaysAssigned() > 0) {
                int treated = 0;for (auto& animal : animals) {
                    if (animal.getIsSick() && treated < worker.getMaxAnimals()) {
                        const auto& encIds = worker.getAssignedEnclosures();
                        if (find(encIds.begin(), encIds.end(), animal.getEnclosureId()) != encIds.end()) {
                            animal.setSick(false);
                            treated++;
                            for (auto& enc : enclosures) {
                                if (enc.getId() == animal.getEnclosureId()) {
                                    enc.updateAnimal(animal);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Расчет количества необходимой еды
        int foodNeeded = 0;
        for (const auto& animal : animals) {
            foodNeeded += (animal.getType() == AnimalType::HERBIVORE) ? 1 : 2;
        }
        if (food >= foodNeeded) food -= foodNeeded;
        else {
            for (auto it = animals.begin(); it != animals.end();) {
                if (random(0, 99) < 30) {
                    for (auto& enc : enclosures) {
                        if (enc.getId() == it->getEnclosureId()) {
                            enc.removeAnimal(it->getUniqueId());
                            break;
                        }
                    }
                    cout << it->getDisplayName() << " умерло от голода.\n";
                    it = animals.erase(it);
                    totalAnimals--;
                }
                else ++it;
            }
        }

        popularity *= (1.0 + (random(-10, 10) / 100.0));
        int sickCount = count_if(animals.begin(), animals.end(), [](const Animal& a) { return a.getIsSick(); });
        popularity -= sickCount;
        if (popularity < 0) popularity = 0;

        visitors = static_cast<int>(popularity);
        int specialRoll = random(0, 99);
        if (specialRoll < 20) {
            specialVisitorType = "None";
            specialVisitorCount = 0;
        }
        else if (specialRoll < 30) {
            specialVisitorType = "Celebrity";
            specialVisitorCount = random(1, 2);
            popularity += specialVisitorCount * 10;
        }
        else if (specialRoll < 50) {
            specialVisitorType = "Photographer";
            specialVisitorCount = random(1, 3);
            popularity += specialVisitorCount * 5;
        }

        money += visitors * totalAnimals;

        for (const auto& worker : workers) money -= worker.getSalary();
        for (const auto& enc : enclosures) money -= enc.getDailyCost();
        for (auto& loan : loans) {
            if (loan.daysLeft > 0) {
                money -= loan.dailyRepayment;
                loan.daysLeft--;
                if (loan.daysLeft == 0) cout << "Кредит на $" << loan.principal << " погашен.\n";
            }
        }
        loans.erase(remove_if(loans.begin(), loans.end(), [](const Loan& loan) { return loan.daysLeft <= 0; }), loans.end());
    }

    /**
     * @brief Запускает симуляцию зоопарка на срок до 20 дней.
     */
    void playGame() {
        const int maxDays = 20;
        while (day <= maxDays) {
            displayStatus();
            string prompt = "\nДействия:\n"
                "1. Управление животными\n"
                "2. Управление покупками\n"
                "3. Управление вольерами\n"
                "4. Управление работниками\n"
                "5. Управление размножением\n"
                "6. Следующий день\n"
                "Выберите действие: ";
            int choice = getValidInput(prompt, 1, 6);

            if (choice == 1) manageAnimals();
            else if (choice == 2) managePurchases();
            else if (choice == 3) manageEnclosures();
            else if (choice == 4) manageWorkers();
            else if (choice == 5) manageBreeding();
            else if (choice == 6) {
                nextDay();
                if (money < 0) {
                    cout << "\nИгра окончена! У вас закончились деньги на день " << day << ".\n";
                    return;
                }
            }
        }
        cout << "\nПоздравляем! Вы успешно управляли зоопарком \"" << name << "\" в течение " << maxDays << " дней!\n";
    }
};

/**
 * @brief Основная функция для запуска симуляции зоопарка.
 * @return 0 при успешном выполнении.
 */
int main() {
    setlocale(LC_ALL, "Russian_Russian.1251");
    srand(time(0));
    string name;
    while (true) {
        cout << "Введите название вашего зоопарка: ";
        getline(cin, name);
        if (!name.empty()) break;cout << "Название зоопарка не может быть пустым. Попробуйте снова.\n";
    }
    Zoo zoo(name);
    zoo.playGame();
    cin.get();
    return 0;
}