#include <gtest/gtest.h>

#include "Parser.h"
#include "VM.h"

class Environment : public ::testing::Environment {
public:
    ~Environment() override {}

    void SetUp() override {
        Options::set<size_t>("cell_limit", 2000);
        Logger::set_level("Compiler", Logger::DEBUG);
    }

    void TearDown() override {
        Options::reset();
        Logger::reset();
    }
};

testing::Environment *const env = testing::AddGlobalTestEnvironment(new Environment);


TEST(VMWithParserTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC 104 PUTCHAR STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "h");
}

TEST(VMWithParserTest, BasicBranch) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC 1 SEL (LDC 10 PUTNUM JOIN) (LDC 20 PUTNUM JOIN) LDC 0 SEL (LDC 30 PUTNUM JOIN) (LDC 40 "
                       "PUTNUM JOIN) STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "1040");
}

TEST(VMWithParserTest, BasicFunction) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(NIL LDC 1 CONS LDC 2 CONS LDF (LD (1 . 1) LD (1.2) ADD RET) AP PUTNUM STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "3");
}

TEST(VMWithParserTest, RecFunction) {
    std::stringstream ssin;
    std::stringstream ssout;

    {
        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("( DUM NIL LDF ( LD ( 1 . 1 ) SEL ( LD ( 1 . 1 ) LDC -1 ADD SEL ( NIL LD ( 1 . 1 ) LDC -1 ADD "
                       "CONS LD ( 2 . 1 ) AP "
                       "NIL LD ( 1 . 1 ) LDC -2 ADD CONS LD ( 2 . 1 ) AP ADD JOIN ) ( LDC 1 JOIN ) JOIN ) ( LDC 0 JOIN "
                       ") RET ) CONS LDF ( "
                       "NIL LDC 20 CONS LD ( 1 . 1 ) AP RET ) RAP PUTNUM STOP )");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
    ssout.flush();
    EXPECT_EQ(ssout.str(), "6765");
}