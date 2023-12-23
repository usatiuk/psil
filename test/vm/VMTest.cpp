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