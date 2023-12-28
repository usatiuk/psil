//
// Created by Stepan Usatiuk on 27.12.2023.
//
#include <gtest/gtest.h>

#include "MemoryContext.h"
#include "Parser.h"
#include "VM.h"

TEST(CompilerTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        MemoryContext mc;
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
        MemoryContext mc;
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
        MemoryContext mc;
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
        MemoryContext mc;
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
        MemoryContext mc;
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
        MemoryContext mc;
        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (letrec ((fib (lambda (n) (if n (if (+ n -1) (+ (fib (+ n -1)) (fib(+ n -2))) 1) 0) ))) (fib 10)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "55");
}