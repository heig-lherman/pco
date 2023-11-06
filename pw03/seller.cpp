#include "seller.h"
#include <algorithm>
#include <random>
#include <cassert>

Seller *Seller::chooseRandomSeller(std::vector<Seller *> &sellers) {
    assert(sellers.size());
    std::vector<Seller*> out;
    std::sample(sellers.begin(), sellers.end(), std::back_inserter(out),
            1, std::mt19937{std::random_device{}()});
    return out.front();
}

ItemType Seller::chooseRandomItem(std::map<ItemType, int> &itemsForSale) {
    if (!itemsForSale.size()) {
        return ItemType::Nothing;
    }
    std::vector<std::pair<ItemType, int> > out;
    std::sample(itemsForSale.begin(), itemsForSale.end(), std::back_inserter(out),
            1, std::mt19937{std::random_device{}()});
    return out.front().first;
}

int getCostPerUnit(ItemType item) {
    switch (item) {
        case ItemType::Sand : return SAND_COST;
        case ItemType::Copper : return COPPER_COST;
        case ItemType::Petrol : return PETROL_COST;
        case ItemType::Chip : return CHIP_COST;
        case ItemType::Plastic : return PLASTIC_COST;
        case ItemType::Robot : return ROBOT_COST;
        default : return 0;
    }
}

QString getItemName(ItemType item) {
    switch (item) {
        case ItemType::Sand : return "Sand";
        case ItemType::Copper : return "Copper";
        case ItemType::Petrol : return "Petrol";
        case ItemType::Chip : return "Chip";
        case ItemType::Plastic : return "Plastic";
        case ItemType::Robot : return "Robot";
        case ItemType::Nothing : return "Nothing";
        default : return "???";
    }
}

EmployeeType getEmployeeThatProduces(ItemType item) {
    switch (item) {
        case ItemType::Sand : /* fallthrough */
        case ItemType::Copper : /* fallthrough */
        case ItemType::Petrol : return EmployeeType::Extractor;
        case ItemType::Chip : return EmployeeType::Electrician;
        case ItemType::Plastic : return EmployeeType::Plasturgist;
        case ItemType::Robot : return EmployeeType::Engineer;
        default : return EmployeeType::Extractor; // Why not
    }
}

int getEmployeeSalary(EmployeeType employee) {
    switch (employee) {
        case EmployeeType::Extractor : return EXTRACOTR_COST;
        case EmployeeType::Electrician : return ELECTRICIAN_COST;
        case EmployeeType::Engineer : return ENGINEER_COST;
        case EmployeeType::Plasturgist : return PLASTIC_COST;
        default : return 0;
    }
}
