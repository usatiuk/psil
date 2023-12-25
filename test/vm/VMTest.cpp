#include <gtest/gtest.h>

#include "Command.h"
#include "ConsUtils.h"
#include "VM.h"

TEST(VMTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        MemoryContext mc;
        VM vm(ssin, ssout);
        MCHandle newc(ConsUtils::cons(nullptr, nullptr));
        ConsUtils::append(newc, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
        ConsUtils::append(newc, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
        ConsUtils::append(newc, ConsUtils::makeIntCell('h'));
        ConsUtils::append(newc, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
        ConsUtils::append(newc, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::STOP)));
        vm.loadControl(newc);
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "h");
}

//TEST(VMTest, SelTest) {
//    std::stringstream ssin;
//    std::stringstream ssout;
//    {
//        VM vm(ssin, ssout);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::STOP)));
//
//        // True branch true test
//        ConsCell *tbtt = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(tbtt, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//        vm.push(tbtt, vm.makeCell<IntCell>('1'));
//        vm.push(tbtt, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        // False branch true test
//        ConsCell *fbtt = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(fbtt, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//        vm.push(fbtt, vm.makeCell<IntCell>('2'));
//        vm.push(fbtt, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        // True branch false test
//        ConsCell *tbft = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(tbft, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//        vm.push(tbft, vm.makeCell<IntCell>('3'));
//        vm.push(tbft, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        // False branch false test
//        ConsCell *fbft = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(fbft, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//        vm.push(fbft, vm.makeCell<IntCell>('4'));
//        vm.push(fbft, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        vm.appendCommand(fbft);
//        vm.appendCommand(tbft);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SEL)));
//        vm.appendCommand(vm.makeCell<IntCell>(0));
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//        vm.appendCommand(fbtt);
//        vm.appendCommand(tbtt);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SEL)));
//        vm.appendCommand(vm.makeCell<IntCell>(1));
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//        vm.run();
//    }
//    ssout.flush();
//    EXPECT_EQ(ssout.str(), "14");
//}
//
//TEST(VMTest, SimpleFunction) {
//    std::stringstream ssin;
//    std::stringstream ssout;
//    {
//        VM vm(ssin, ssout);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::STOP)));
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//
//        // Add function
//        ConsCell *addfn = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RET)));
//        vm.push(addfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//        vm.push(addfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(2)));
//        vm.push(addfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//        vm.push(addfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//        vm.push(addfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//        vm.appendCommand(addfn);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDF)));
//        vm.appendCommand(
//                vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<ConsCell>(vm.makeCell<IntCell>('2'))));
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//        vm.run();
//    }
//    ssout.flush();
//    EXPECT_EQ(ssout.str(), "3");
//}
//
//TEST(VMTest, RecursiveFunction) {
//    std::stringstream ssin;
//    std::stringstream ssout;
//    {
//        VM vm(ssin, ssout);
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::STOP)));
//
//        vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RAP)));
//
//        // Fib function
//        ConsCell *fibfn = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RET)));
//
//        // 0 case
//        ConsCell *zcase = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(zcase, vm.makeCell<IntCell>(0));
//        vm.push(zcase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        // 1 case
//        ConsCell *ocase = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(ocase, vm.makeCell<IntCell>(1));
//        vm.push(ocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//
//        // >1 case
//        ConsCell *gocase = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(2), vm.makeCell<IntCell>(1)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//        vm.push(gocase, vm.makeCell<IntCell>(-2));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(2), vm.makeCell<IntCell>(1)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//        vm.push(gocase, vm.makeCell<IntCell>(-1));
//        vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(gocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//// >=1 case
//ConsCell *geocase = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::JOIN)));
//vm.push(geocase, ocase);
//vm.push(geocase, gocase);
//vm.push(geocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SEL)));
//vm.push(geocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//vm.push(geocase, vm.makeCell<IntCell>(-1));
//vm.push(geocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(geocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(geocase, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//
//vm.push(fibfn, zcase);
//vm.push(fibfn, geocase);
//vm.push(fibfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::SEL)));
//vm.push(fibfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//
//// Fib caller function
//ConsCell *fibcallfn = vm.makeCell<ConsCell>(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::RET)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTNUM)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(' '));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(10));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(6));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(5));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(4));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::PUTCHAR)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::ADD)));
//vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::AP)));
//vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LD)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.push(fibcallfn, vm.makeCell<IntCell>(3));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDC)));
//vm.push(fibcallfn, ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//
//vm.appendCommand(fibcallfn);
//vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDF)));
//
//vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::CONS)));
//vm.appendCommand(fibfn);
//vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::LDF)));
//vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::NIL)));
//
//vm.appendCommand(ConsUtils::makeIntCell(Command::cmd_to_int(Command::CommandNum::DUM)));
//vm.run();
//}
//ssout.flush();
//EXPECT_EQ(ssout.str(), "2358 55");
//}