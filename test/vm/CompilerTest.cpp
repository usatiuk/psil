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
        Options::set<size_t>("cell_limit", 50000);
        Options::set<bool>("command_strs", true);
        Logger::set_level(Logger::VM, Logger::DEBUG);
        Logger::set_level(Logger::Compiler, Logger::DEBUG);
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
    ASSERT_EQ(ssout.str(), "3\n");
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
    ASSERT_EQ(ssout.str(), "1\n");
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
    ASSERT_EQ(ssout.str(), "5\n");
}

TEST(CompilerTest, BasicFn2) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a b) (- a b)))) (plfn 2 3)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "-1\n");
}

TEST(CompilerTest, MultiLet) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a b) (- a b))) (plfn2 (lambda (a b) (- b a)))) (plfn 2 3)) EVAL "
                "PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (let ((plfn (lambda (a b) (- a b))) (plfn2 (lambda (a b) (- b a)))) (plfn2 2 3)) EVAL "
                "PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "-1\n1\n");
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
    ASSERT_EQ(ssout.str(), "1\n");
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
    ASSERT_EQ(ssout.str(), "2\n");
}

TEST(CompilerTest, RecursiveFn) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (letrec ((fib (lambda (n) (if n (if (+ n -1) (+ (fib (+ n -1)) (fib(+ n -2))) 1) 0) ))) "
                "(fib 10)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "55\n");
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
    ASSERT_EQ(ssout.str(), "1\n");
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
    ASSERT_EQ(ssout.str(), "5\n");
}

TEST(CompilerTest, GlobalDefineFnQuote) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one) (quote (1 2))) EVAL LDC (one) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "(1 2)\n");
}

TEST(CompilerTest, GlobalDefineFnCar) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (carr l) (car l)) EVAL LDC (carr (quote (1 2))) EVAL PRINT LDC (carr (cdr (quote "
                "(1 2)))) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "1\n2\n");
}

TEST(CompilerTest, GlobalDefineFnEq) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (eqtest l) (= l ())) EVAL LDC (eqtest (quote ())) EVAL PRINT LDC (eqtest (nil)) "
                "EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (eqtest l) (= l (nil))) EVAL LDC (eqtest (quote ())) EVAL PRINT LDC (eqtest "
                "(nil)) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "1\n1\n1\n1\n");
}

TEST(CompilerTest, GlobalDefineFnMulti) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one x y) (+ x y)) EVAL LDC (define (two x y) (one (+ x 1) y)) EVAL LDC (two 2 3) "
                "EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "6\n");
}

TEST(CompilerTest, GlobalDefineFnMultiTwo) {
    std::stringstream ssin;
    std::stringstream ssout;
    {

        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (one x y) (- x y)) EVAL LDC (define (two x y) (one (+ x 1) y)) EVAL LDC (two 2 3) "
                "EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    ssout.flush();
    ASSERT_EQ(ssout.str(), "0\n");
}


TEST(CompilerTest, GlobalDefineFnRec) {
    std::stringstream ssin;
    std::stringstream ssout;
    Options::set<bool>("command_strs", false);
    Logger::set_level(Logger::VM, Options::get<size_t>("default_log_level"));
    {
        VM vm(ssin, ssout);
        Parser parser;
        parser.loadStr("(LDC (define (fib n) (if n (if (+ n -1) (+ (fib (+ n -1)) (fib(+ n -2))) 1) 0) ) EVAL LDC (fib "
                "20) EVAL PRINT STOP)");
        vm.loadControl(parser.parseExpr());
        vm.run();
    }
    Options::set<bool>("command_strs", true);
    Logger::set_level(Logger::VM, Logger::DEBUG);
    ssout.flush();
    ASSERT_EQ(ssout.str(), "6765\n");
}