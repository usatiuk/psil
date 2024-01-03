//
// Created by Stepan Usatiuk on 27.12.2023.
//
#include <gtest/gtest.h>

#include "MemoryContext.h"
#include "Options.h"
#include "Parser.h"
#include "VM.h"

class Environment : public ::testing::Environment {
public:
    ~Environment() override {}

    void SetUp() override {
        Options::set_bool("command_strs", true);
        Logger::set_level("Compiler", Logger::DEBUG);
    }

    void TearDown() override {
        Options::reset();
        Logger::reset();
    }
};

testing::Environment *const env = testing::AddGlobalTestEnvironment(new Environment);

TEST(CompilerTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC 3 EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "3");
}

TEST(CompilerTest, BasicLet) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((x 1)) x) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "1");
}

TEST(CompilerTest, BasicFn) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a b) (+ a b)))) (plfn 2 3)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "5");
}

TEST(CompilerTest, BasicFnIfT) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a) (if a 1 2)))) (plfn 1)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "1");
}
TEST(CompilerTest, BasicFnIfF) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a) (if a 1 2)))) (plfn 0)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "2");
}
TEST(CompilerTest, RecursiveFn) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr(
                "(LDC (letrec ((fib (lambda (n) (if n (if (+ n -1) (+ (fib (+ n -1)) (fib(+ n -2))) 1) 0) ))) (fib 10)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "55");
}

TEST(CompilerTest, GlobalDefine) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one) 1) EVAL LDC (one) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "1");
}


TEST(CompilerTest, GlobalDefineFn) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one x y) (+ x y)) EVAL LDC (one 2 3) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "5");
}

TEST(CompilerTest, GlobalDefineFnMulti) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one x y) (+ x y)) EVAL LDC (define (two x y) (one (+ x 1) y)) EVAL LDC (two 2 3) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "6");
}

TEST(CompilerTest, GlobalDefineFnRec) {
    std::stringstream ssin;
    std::stringstream ssout;
    Options::set_bool("command_strs", false);
    Logger::set_level("MemoryContext", Logger::INFO);
    {
        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr(
                "(LDC (define (fib n) (if n (if (+ n -1) (+ (fib (+ n -1)) (fib(+ n -2))) 1) 0) ) EVAL LDC (fib 20) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    Options::set_bool("command_strs", true);
    Logger::set_level("MemoryContext", Options::get_int("default_log_level"));
    ssout.flush();
    EXPECT_EQ(ssout.str(), "6765");
}