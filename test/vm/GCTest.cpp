//
// Created by Stepan Usatiuk on 26.12.2023.
//

#include <gtest/gtest.h>

#include "MemoryContext.h"

TEST(GCTest, GCTest) {

    {
        Handle c = Handle::cons(nullptr, nullptr);
        MemoryContext::get().request_gc_and_wait();
        c.append(Handle::makeNumCell(1));
        c.append(Handle::makeNumCell(2));
        MemoryContext::get().request_gc_and_wait();
        EXPECT_EQ(c.car().val(), 1);
        EXPECT_EQ(c.cdr().car().val(), 2);
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
    {
        Handle c = Handle::cons(nullptr, nullptr);
        MemoryContext::get().request_gc_and_wait();
        c.push(Handle::makeNumCell(1));
        c.push(Handle::makeNumCell(2));
        MemoryContext::get().request_gc_and_wait();
        EXPECT_EQ(c.car().val(), 2);
        EXPECT_EQ(c.cdr().car().val(), 1);
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
}

TEST(GCTest, GCTestAppend) {

    for (int i = 0; i < 25000; i++) {
        Handle c = Handle::cons(nullptr, nullptr);
        MemoryContext::get().request_gc();
        c.append(Handle::makeNumCell(1));
        MemoryContext::get().request_gc();
        EXPECT_EQ(c.car().val(), 1);
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
}
TEST(GCTest, GCTestPop) {

    {
        Handle c = Handle::cons(nullptr, nullptr);
        static constexpr int test_size = 20000;
        for (int i = 0; i < test_size; i++) {
            MemoryContext::get().request_gc();
            c.push(Handle::makeNumCell(i));
        }
        for (int i = test_size - 1; i >= 0; i--) {
            MemoryContext::get().request_gc();
            EXPECT_EQ(i, c.pop().val());
        }
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
}

TEST(GCTest, GCTestAppend2) {

    Handle c = Handle::cons(nullptr, nullptr);
    static constexpr int test_size = 2000;
    for (int i = 0; i < test_size; i++) {
        MemoryContext::get().request_gc();
        c.append(Handle::makeNumCell(i));
    }
    for (int i = 0; i < test_size; i++) {
        MemoryContext::get().request_gc();
        EXPECT_EQ(i, c.pop().val());
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
}

TEST(GCTest, GCTestAppend3) {

    for (int i = 0; i < 250000; i++) {
        Handle c = Handle::cons(nullptr, nullptr);
        MemoryContext::get().request_gc();
        c.append(Handle::makeNumCell(1));
        c.append(Handle::makeNumCell(2));
        MemoryContext::get().request_gc();
        Handle n = c.cdr();
        c.setcdr(nullptr);
        EXPECT_EQ(n.car().val(), 2);
        EXPECT_EQ(c.car().val(), 1);
    }
    MemoryContext::get().request_gc_and_wait();
    MemoryContext::get().request_gc_and_wait();
    EXPECT_EQ(MemoryContext::get().cell_count(), 0);
}
