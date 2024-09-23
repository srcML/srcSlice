#include "../headers/srcsliceevent.hpp"

bool StringContainsCharacters(const std::string &str) {
    for (char ch : str) {
        if (std::isalpha(ch)) {
            return true;
        }
    }
    return false;
}