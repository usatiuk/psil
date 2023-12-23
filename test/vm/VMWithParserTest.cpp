#include <gtest/gtest.h>

#include "VM.h"
#include "Parser.h"

TEST(VMWithParserTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        Parser parser(vm);
        parser.loadSecd("( LDC 104 PUTCHAR STOP )");
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "h");
}

TEST(VMWithParserTest, BasicFunction) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        Parser parser(vm);
        parser.loadSecd("( NIL LDC 1 CONS LDC 2 CONS LDF ( LD ( 1 . 1 ) LD ( 1 . 2 ) ADD RET ) AP PUTNUM STOP )");
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "3");
}