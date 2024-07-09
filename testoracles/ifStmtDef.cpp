int main() {
    srand(time(nullptr));
    if (int r = rand() % 100 - 1; r % 2 == 0) {
        std::cout << r << std::endl;
    }
    return 0;
}