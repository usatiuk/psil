#include <gtest/gtest.h>

#include "VM.h"

TEST(VMTest, BasicHello) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::STOP));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.appendCommand(vm.makeCell<IntCell>('h'));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "h");
}

TEST(VMTest, SelTest) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::STOP));

        // True branch true test
        ConsCell *tbtt = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(tbtt, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(tbtt, vm.makeCell<IntCell>('1'));
        vm.push(tbtt, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        // False branch true test
        ConsCell *fbtt = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(fbtt, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fbtt, vm.makeCell<IntCell>('2'));
        vm.push(fbtt, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        // True branch false test
        ConsCell *tbft = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(tbft, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(tbft, vm.makeCell<IntCell>('3'));
        vm.push(tbft, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        // False branch false test
        ConsCell *fbft = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(fbft, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fbft, vm.makeCell<IntCell>('4'));
        vm.push(fbft, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        vm.appendCommand(fbft);
        vm.appendCommand(tbft);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::SEL));
        vm.appendCommand(vm.makeCell<IntCell>(0));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.appendCommand(fbtt);
        vm.appendCommand(tbtt);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::SEL));
        vm.appendCommand(vm.makeCell<IntCell>(1));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "14");
}

TEST(VMTest, SimpleFunction) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::STOP));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));

        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));

        // Add function
        ConsCell *addfn = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::RET));
        vm.push(addfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(addfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(2)));
        vm.push(addfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(addfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(addfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.appendCommand(addfn);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDF));
        vm.appendCommand(
                vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<ConsCell>(vm.makeCell<IntCell>('2'))));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "3");
}

TEST(VMTest, RecursiveFunction) {
    std::stringstream ssin;
    std::stringstream ssout;
    {
        VM vm(ssin, ssout);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::STOP));

        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::RAP));

        // Fib function
        ConsCell *fibfn = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::RET));

        // 0 case
        ConsCell *zcase = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(zcase, vm.makeCell<IntCell>(0));
        vm.push(zcase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        // 1 case
        ConsCell *ocase = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(ocase, vm.makeCell<IntCell>(1));
        vm.push(ocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));

        // >1 case
        ConsCell *gocase = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));

        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(2), vm.makeCell<IntCell>(1)));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));

        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(gocase, vm.makeCell<IntCell>(-2));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(2), vm.makeCell<IntCell>(1)));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));

        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(gocase, vm.makeCell<IntCell>(-1));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(gocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(gocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        // >=1 case
        ConsCell *geocase = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::JOIN));
        vm.push(geocase, ocase);
        vm.push(geocase, gocase);
        vm.push(geocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::SEL));
        vm.push(geocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(geocase, vm.makeCell<IntCell>(-1));
        vm.push(geocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(geocase, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(geocase, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));

        vm.push(fibfn, zcase);
        vm.push(fibfn, geocase);
        vm.push(fibfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::SEL));
        vm.push(fibfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));

        // Fib caller function
        ConsCell *fibcallfn = vm.makeCell<ConsCell>(vm.makeCell<CommandCell>(CommandCell::CommandNum::RET));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTNUM));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fibcallfn, vm.makeCell<IntCell>(' '));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(fibcallfn, vm.makeCell<IntCell>(10));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(fibcallfn, vm.makeCell<IntCell>(6));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(fibcallfn, vm.makeCell<IntCell>(5));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(fibcallfn, vm.makeCell<IntCell>(4));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::PUTCHAR));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::ADD));
        vm.push(fibcallfn, vm.makeCell<IntCell>('0'));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::AP));
        vm.push(fibcallfn, vm.makeCell<ConsCell>(vm.makeCell<IntCell>(1), vm.makeCell<IntCell>(1)));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LD));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.push(fibcallfn, vm.makeCell<IntCell>(3));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::LDC));
        vm.push(fibcallfn, vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));


        vm.appendCommand(fibcallfn);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDF));

        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::CONS));
        vm.appendCommand(fibfn);
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::LDF));
        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::NIL));

        vm.appendCommand(vm.makeCell<CommandCell>(CommandCell::CommandNum::DUM));
        vm.run();
    }
    ssout.flush();
    EXPECT_EQ(ssout.str(), "2358 55");
}