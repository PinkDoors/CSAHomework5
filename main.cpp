#include <iostream>
#include <thread>
#include <cstdlib>
#include <mutex>
#include <semaphore>

std::counting_semaphore pot(0);
std::mutex eat_lock;
int missionaries_supply;
int pot_capacity;
int savage_number;

/**
 * Повар наполняет горшок мясом миссионеров.
 */
void callChef() {
    if (missionaries_supply < pot_capacity) {
        pot.release(missionaries_supply);
        missionaries_supply = 0;
        std::cout << "The pot ran out of meat. One of the savages asked the cook to cook more.\n";
    } else {
        std::cout << "The pot ran out of meat. One of the savages asked the cook to cook more.\n";
        pot.release(pot_capacity);
        missionaries_supply -= pot_capacity;
    }
}

/**
 * Один из дикарей пробует достать мясо из горшка и съесть его, после этого он идет заниматься своими делами.
 * @return Наличие мяса в горшке.
 */
bool tryToEat() {
    bool try_to_eat = pot.try_acquire();
    if (try_to_eat) {
        std::cout << "The savage took a piece of the missionary and ate it.\n";
        return true;
    } else {
        return false;
    }
}

/**
 * Метод имитирующий работу после еды.
 */
void sleep() {
    // Генерируем случайное время работы.
    srand(time(0));
    int hunger = rand() % 24;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 * hunger));
}

/**
 * Метод, иллюстрирующий жизнь дикаря.
 */
void startDinner() {
    while (missionaries_supply != 0) {
        eat_lock.lock();
        // Вызываем повара, если еда закончилась.
        if (!tryToEat()) {
            callChef();
        }
        eat_lock.unlock();
        sleep();
    }
    while (tryToEat()) {
        sleep();
    }
    std::cout << "One of the savages found out that his tribe ran out of meat and he went fishing.\n";
}

void incorrentInputMessage() {
    std::cout << "incorrect command line!\n"
                 "  Waited:\n"
                 "     -i <Savege number> <Pot capacity> <Supply>\n"
                 "  or\n"
                 "     -r";
}

bool checkCorrectness() {
    if (savage_number < 1 || savage_number > 100) {
        std::cout << "An incorrect number of savages was entered,"
                     " make sure it is greater than 0, but less than 100.\n";
        return false;
    }
    if (pot_capacity < 1 || pot_capacity > 10) {
        std::cout << "An incorrect pot capacity was entered,"
                     " make sure it is greater than 0, but less than 100.\n";
        return false;
    }
    if (missionaries_supply < 1 || missionaries_supply > 1000) {
        std::cout << "An incorrect missionaries supply was entered,"
                     " make sure it is greater than 0, but less than 1000.\n";
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if ((!strcmp(argv[1],"-i") && argc != 5) && !strcmp(argv[1],"-r") && argc != 1) {
        incorrentInputMessage();
        return 1;
    }

    if (!strcmp(argv[1],"-r")) {
        srand(time(0));
        savage_number = rand() % 101;
        pot_capacity = rand() % 10 + 1;
        missionaries_supply = rand() % 1000 + 1;
    } else if (!strcmp(argv[1],"-i")){
        savage_number = std::strtol(argv[2], nullptr, 10);
        pot_capacity = std::strtol(argv[3], nullptr, 10);
        missionaries_supply = std::strtol(argv[4], nullptr, 10);
    }

    if (!checkCorrectness()) {
        return 1;
    }
    callChef();

    // Создаем потоки, иллюстрирующие дикарей.
    std::thread *savages = new std::thread[savage_number];
    for (int i = 0; i < savage_number; ++i) {
        savages[i] = std::thread(startDinner);
    }
    for (int i = 0; i < savage_number; ++i) {
        savages[i].join();
    }

    return 0;
}