int main() {
    std::string* strPtr;
    std::string key = "abc1234!";
    strPtr = &key;
    std::cout << *strPtr << std::endl;
}