#include <iostream>

#include "MemoryContext.h"
#include "Parser.h"
#include "VM.h"

int main(int argc, char *argv[]) {
    try {
        Handle repl;
        {
            Parser parser;
            parser.loadStr("(READ EVAL PRINT STOP)");
            repl = parser.parseExpr();
        }
        VM vm;

        while (true) {
            std::cout << std::endl;
            vm.loadControl(repl);
            vm.run();
            std::cout << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "\nUncaught exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Crash!" << std::endl;
        return -1;
    }
    return 0;
}
