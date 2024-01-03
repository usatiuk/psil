#include <iostream>

#include "MemoryContext.h"
#include "Parser.h"
#include "VM.h"

int main() {
    Handle repl;
    {
        Parser parser;
        parser.loadStr("(READ EVAL PRINT STOP)");
        repl = parser.parseExpr();
    }

    while (true) {
        VM vm;
        std::cout << std::endl;
        vm.loadControl(repl);
        vm.run();
        std::cout << std::endl;
    }

    return 0;
}
