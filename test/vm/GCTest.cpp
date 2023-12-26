//
// Created by Stepan Usatiuk on 26.12.2023.
//

#include <gtest/gtest.h>

#include "ConsUtils.h"
#include "MemoryContext.h"

using namespace ConsUtils;

TEST(GCTest, GCTest) {
    MemoryContext mc;
    {
        MCHandle c = cons(nullptr, nullptr);
        mc.request_gc_and_wait();
        append(c, makeNumCell(1));
        append(c, makeNumCell(2));
        mc.request_gc_and_wait();
        EXPECT_EQ(val(car(c)), 1);
        EXPECT_EQ(val(car(cdr(c))), 2);
    }
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    EXPECT_EQ(mc.cell_count(), 0);
    {
        MCHandle c = cons(nullptr, nullptr);
        mc.request_gc_and_wait();
        push(c, makeNumCell(1));
        push(c, makeNumCell(2));
        mc.request_gc_and_wait();
        EXPECT_EQ(val(car(c)), 2);
        EXPECT_EQ(val(car(cdr(c))), 1);
    }
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    EXPECT_EQ(mc.cell_count(), 0);
}

TEST(GCTest, GCTestAppend) {
    MemoryContext mc;
    for (int i = 0; i < 25000; i++) {
        MCHandle c = cons(nullptr, nullptr);
        mc.request_gc();
        append(c, makeNumCell(1));
        mc.request_gc();
        EXPECT_EQ(val(car(c)), 1);
    }
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    EXPECT_EQ(mc.cell_count(), 0);
}
TEST(GCTest, GCTestPop) {
    MemoryContext mc;
    {
        MCHandle c = cons(nullptr, nullptr);
        static constexpr int test_size = 20000;
        for (int i = 0; i < test_size; i++) {
            mc.request_gc();
            push(c, makeNumCell(i));
        }
        for (int i = test_size - 1; i >= 0; i--) {
            mc.request_gc();
            EXPECT_EQ(i, val(pop(c)));
        }
    }
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    EXPECT_EQ(mc.cell_count(), 0);
}

TEST(GCTest, GCTestAppend2) {
    MemoryContext mc;
    MCHandle c = cons(nullptr, nullptr);
    static constexpr int test_size = 2000;
    for (int i = 0; i < test_size; i++) {
        mc.request_gc();
        append(c, makeNumCell(i));
    }
    for (int i = 0; i < test_size; i++) {
        mc.request_gc();
        EXPECT_EQ(i, val(pop(c)));
    }
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    mc.request_gc_and_wait();
    EXPECT_EQ(mc.cell_count(), 0);
}
