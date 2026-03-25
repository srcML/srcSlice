#define CATCH_CONFIG_MAIN
#include "./srcSliceTest.hpp"

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    ResetCount();
    std::cout << INFO << " Testing Multi-file Slicing" << std::endl;

    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
int main() {
    int a;
}
)",
R"(
int main() {
    int b;
}
)"
    }, {"file.cpp","file2.cpp"}));

    json expected = R"({
    "a-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:3:9"]
    },
    "b-3-9":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
void foo(int a) {
    a = 5;
}
)",
R"(
int main() {
    int b = 0;
    foo(b);
}
)"
    }, {"file.cpp","file2.cpp"}));

    json expected = R"({
    "a-2-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:14","file.cpp:3:5"]
    },
    "b-3-9":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"file2.cpp:4:5"}],
        "use":["file2.cpp:4:9","file.cpp:2:14","file.cpp:3:5"],
        "definition":["file2.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
void foo(int a) {
    a = 5;
}
)",
R"(
int main() {
    int b = 0;
    foo(b);
}
)"
    }, {"file.cpp","/utils/file2.cpp"}));

    json expected = R"({
    "a-2-14":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file.cpp:2:14","file.cpp:3:5"]
    },
    "b-3-9":{
        "file":"/utils/file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"b",
        "initial":"/utils/file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"file.cpp:2:1","invoke":"/utils/file2.cpp:4:5"}],
        "use":["/utils/file2.cpp:4:9","file.cpp:2:14","file.cpp:3:5"],
        "definition":["/utils/file2.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
void foo(int a) {
    a = 5;
}
)",
R"(
int bar() {
    int b = 0;
    foo(b);
}
)"
    }, {"/etc/file.cpp","/utils/file2.cpp"}));

    json expected = R"({
    "a-2-14":{
        "file":"/etc/file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"/etc/file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["/etc/file.cpp:2:14","/etc/file.cpp:3:5"]
    },
    "b-3-9":{
        "file":"/utils/file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"bar",
        "type":"int",
        "name":"b",
        "initial":"/utils/file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[{"functionName":"foo","parameter":"1","definitionPosition":"/etc/file.cpp:2:1","invoke":"/utils/file2.cpp:4:5"}],
        "use":["/utils/file2.cpp:4:9","/etc/file.cpp:2:14","/etc/file.cpp:3:5"],
        "definition":["/utils/file2.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
void foo(int a) {
    a = 5;
}
)",
R"(
int foo(int d, char c) {
    return d ^ (int)c;
}
)",
R"(
int bar() {
    int b = 0;
    foo(b);
    foo(b, 'v');
}
)"
    }, {"/etc/file.cpp","file3.cpp","/utils/file2.cpp"}));

    json expected = R"({
    "a-2-14":{
        "file":"/etc/file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"/etc/file.cpp:2:14",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["/etc/file.cpp:2:14","/etc/file.cpp:3:5"]
    },
    "d-2-13":{
        "file":"file3.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"d",
        "initial":"file3.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file3.cpp:3:12"],
        "definition":["file3.cpp:2:13"]
    },
    "c-2-21":{
        "file":"file3.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"char",
        "name":"c",
        "initial":"file3.cpp:2:21",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file3.cpp:3:21"],
        "definition":["file3.cpp:2:21"]
    },
    "b-3-9":{
        "file":"/utils/file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"bar",
        "type":"int",
        "name":"b",
        "initial":"/utils/file2.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"foo","parameter":"1","definitionPosition":"/etc/file.cpp:2:1","invoke":"/utils/file2.cpp:4:5"},
            {"functionName":"foo","parameter":"1","definitionPosition":"file3.cpp:2:1","invoke":"/utils/file2.cpp:5:5"}
        ],
        "use":["/utils/file2.cpp:4:9","/utils/file2.cpp:5:9","/etc/file.cpp:2:14","/etc/file.cpp:3:5","file3.cpp:2:13","file3.cpp:3:12"],
        "definition":["/utils/file2.cpp:3:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
int mycalc::sum(int a, int b) {
    return a+b;
}
)",
R"(
int main() {
    int x = 0;
    int y = 0;
    mycalc::sum(x,y);
}
)"
    }, {"calc.cpp","file.cpp"}));

    json expected = R"({
    "a-2-21":{
        "file":"calc.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"mycalc::sum",
        "type":"int",
        "name":"a",
        "initial":"calc.cpp:2:21",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["calc.cpp:3:12"],
        "definition":["calc.cpp:2:21"]
    },
    "b-2-28":{
        "file":"calc.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"mycalc::sum",
        "type":"int",
        "name":"b",
        "initial":"calc.cpp:2:28",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["calc.cpp:3:14"],
        "definition":["calc.cpp:2:28"]
    },
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"sum","parameter":"1","definitionPosition":"calc.cpp:2:1","invoke":"file.cpp:5:5"}
        ],
        "use":["file.cpp:5:17","calc.cpp:2:21","calc.cpp:3:12"],
        "definition":["file.cpp:3:9"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"sum","parameter":"2","definitionPosition":"calc.cpp:2:1","invoke":"file.cpp:5:5"}
        ],
        "use":["file.cpp:5:19","calc.cpp:2:28","calc.cpp:3:14"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
class Ether {
public:
    Ether(): amount(0) {}
    Ether(const Ether& rhs) {
        amount = rhs.amount;
    }
    int GetAmount() const { return amount; }
    void SetAmount(int a) { amount += a; }
private:
    int amount;
};
)",
R"(
int main() {
    Ether eth;
    int savings = 1000;
    eth.SetAmount(savings);
    std::cout << eth.GetAmount() << std::endl;
}
)"
    }, {"ether.cpp","file.cpp"}));

    json expected = R"({
    "amount-11-9":{
        "file":"ether.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Ether",
        "function":"",
        "type":"int",
        "name":"amount",
        "initial":"ether.cpp:11:9",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["ether.cpp:8:36","ether.cpp:9:29"],
        "definition":["ether.cpp:4:14","ether.cpp:6:9","ether.cpp:9:29","ether.cpp:11:9"]
    },
    "a-9-24":{
        "file":"ether.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Ether",
        "function":"SetAmount",
        "type":"int",
        "name":"a",
        "initial":"ether.cpp:9:24",
        "dependence":[
            {"amount":"ether.cpp:9:39"}
        ],
        "aliases":[],
        "calls":[],
        "use":["ether.cpp:9:39"],
        "definition":["ether.cpp:9:24"]
    },
    "rhs-5-24":{
        "file":"ether.cpp",
        "language":"C++",
        "namespace":[],
        "class":"Ether",
        "function":"Ether",
        "type":"Ether&",
        "name":"rhs",
        "initial":"ether.cpp:5:24",
        "dependence":[
            {"amount":"ether.cpp:6:18"}
        ],
        "aliases":[],
        "calls":[],
        "use":["ether.cpp:6:18"],
        "definition":["ether.cpp:5:24"]
    },
    "eth-3-11":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"Ether",
        "name":"eth",
        "initial":"file.cpp:3:11",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:5:5","file.cpp:6:18"],
        "definition":["file.cpp:3:11"]
    },
    "savings-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"savings",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"SetAmount","parameter":"1","definitionPosition":"ether.cpp:9:5","invoke":"file.cpp:5:5"}
        ],
        "use":["file.cpp:5:19","ether.cpp:9:24","ether.cpp:9:39"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
int foo(int a) {}
int foo(int b, int c) {}
int foo(int d, char e) {}
int foo(int f, int g, int h, int i = 10, int j = 3) {}
)",
R"(
int main() {
    int x = 0;
    int y = 0;
    foo(x,y);
}
)"
    }, {"file2.cpp","file.cpp"}));

    json expected = R"({
    "a-2-13":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"a",
        "initial":"file2.cpp:2:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:2:13"]
    },
    "b-3-13":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"b",
        "initial":"file2.cpp:3:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:3:13"]
    },
    "d-4-13":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"d",
        "initial":"file2.cpp:4:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:4:13"]
    },
    "f-5-13":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"f",
        "initial":"file2.cpp:5:13",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:5:13"]
    },
    "c-3-20":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"c",
        "initial":"file2.cpp:3:20",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:3:20"]
    },
    "e-4-21":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"char",
        "name":"e",
        "initial":"file2.cpp:4:21",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:4:21"]
    },
    "g-5-20":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"g",
        "initial":"file2.cpp:5:20",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:5:20"]
    },
    "h-5-27":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"h",
        "initial":"file2.cpp:5:27",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:5:27"]
    },
    "i-5-34":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"i",
        "initial":"file2.cpp:5:34",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:5:34"]
    },
    "j-5-46":{
        "file":"file2.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"foo",
        "type":"int",
        "name":"j",
        "initial":"file2.cpp:5:46",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":[],
        "definition":["file2.cpp:5:46"]
    },
    "x-3-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"x",
        "initial":"file.cpp:3:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"foo","parameter":"1","definitionPosition":"file2.cpp:3:1","invoke":"file.cpp:5:5"},
            {"functionName":"foo","parameter":"1","definitionPosition":"file2.cpp:4:1","invoke":"file.cpp:5:5"}
        ],
        "use":["file.cpp:5:9","file2.cpp:3:13","file2.cpp:4:13"],
        "definition":["file.cpp:3:9"]
    },
    "y-4-9":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"main",
        "type":"int",
        "name":"y",
        "initial":"file.cpp:4:9",
        "dependence":[],
        "aliases":[],
        "calls":[
            {"functionName":"foo","parameter":"2","definitionPosition":"file2.cpp:3:1","invoke":"file.cpp:5:5"},
            {"functionName":"foo","parameter":"2","definitionPosition":"file2.cpp:4:1","invoke":"file.cpp:5:5"}
        ],
        "use":["file.cpp:5:11","file2.cpp:3:20","file2.cpp:4:21"],
        "definition":["file.cpp:4:9"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

// test slice fragment gluing for class member variables and global variables
TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(
#include <iostream>
#include <file.hpp>

int sigma = 10;

Sun::Sun() {
    temp = 82465;
}
double Sun::getTemp() const { return temp; }

void euclid() {
    std::cout << (sigma + 10) << std::endl;
}

int main() {
    euclid();
    std::cout << sigma << std::endl;
    std::cout << EV << std::endl;
    return 0;
}
)",
R"(
#ifndef MY_SUN
#define MY_SUN

extern int sigma;

inline const char* EV = "the_world!";

class Sun {
public:
    Sun();
    double getTemp() const;
private:
    double temp;
};

#endif
)"
    }, {"file.cpp","file.hpp"}));

    json expected = R"({
    "sigma-5-5":{
        "file":"file.cpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"",
        "type":"int",
        "name":"sigma",
        "initial":"file.cpp:5:5",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:13:19","file.cpp:18:18"],
        "definition":["file.cpp:5:5"]
    },
    "temp-14-12":{
        "file":"file.hpp",
        "language":"C++",
        "namespace":[],
        "class":"Sun",
        "function":"",
        "type":"double",
        "name":"temp",
        "initial":"file.hpp:14:12",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:10:38"],
        "definition":["file.cpp:8:5","file.hpp:14:12"]
    },
    "EV-7-20":{
        "file":"file.hpp",
        "language":"C++",
        "namespace":[],
        "class":"",
        "function":"",
        "type":"char*",
        "name":"EV",
        "initial":"file.hpp:7:20",
        "dependence":[],
        "aliases":[],
        "calls":[],
        "use":["file.cpp:19:18"],
        "definition":["file.hpp:7:20"]
    }
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}

TEST_CASE( TestName("Multi-file Test"), "[srcslice]" ) {
    json produced = json::parse(FetchSlices((std::vector<std::string>){
R"(

)",
R"(

)"
    }, {"file.cpp","file.hpp"}));

    json expected = R"({
    })"_json;

    std::string testName = Catch::getResultCapture().getCurrentTestName();
    REQUIRE( CompareJson("", testName, produced, expected) );
}