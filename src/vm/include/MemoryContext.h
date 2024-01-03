//
// Created by Stepan Usatiuk on 25.12.2023.
//

#ifndef PSIL_MEMORYCONTEXT_H
#define PSIL_MEMORYCONTEXT_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <thread>

#include "Cell.h"
#include "Handle.h"
#include "Logger.h"
#include "Options.h"

class Handle;

class MemoryContext {
public:
    friend Handle;

    MemoryContext();
    ~MemoryContext();

    static MemoryContext &get();

    template<typename CT, typename... Args>
    Handle create_cell(Args... args) {
        return alloc_cell<CT>(std::forward<Args>(args)...);
    }


    void request_gc_and_wait() {
        std::unique_lock l(_gc_done_m);
        _gc_done = false;
        { request_gc(); }
        if (!_gc_done) _gc_done_cv.wait(l, [&] { return _gc_done.load(); });
    }

    void request_gc() {
        std::lock_guard l(_gc_request_m);
        _gc_request = true;
        _gc_request_cv.notify_all();
    }

    size_t cell_count() { return _cells_num; }

    template<typename R>
    R run_dirty(const std::function<R(std::function<void(Cell *)>)> &f) {
        std::lock_guard l(_gc_dirty_notif_queue_lock);
        return f([&](Cell *c) {
            if (c == nullptr) return;
            Logger::log("MemoryContext", [&](std::ostream &out) { out << "marked dirty: " << c; }, Logger::DEBUG);
            _gc_dirty_notif_queue.emplace(c);
        });
    }

private:
    template<typename CT, typename... Args>
    Handle alloc_cell(Args... args) {

        size_t tcellnum;
        {
            std::lock_guard tmplg(_new_roots_lock);
            tcellnum = _temp_cells.size();
        }

        if ((_cells_num + tcellnum) >= (Options::get_int("cell_limit"))) {
            // We might need to run GC twice as it has to process the messages;
            Logger::log("MemoryContext", "Running forced gc", Logger::ERROR);
            for (int i = 0; i < 3 && (_cells_num + tcellnum) >= (Options::get_int("cell_limit")); i++) {
                request_gc_and_wait();
                {
                    std::lock_guard tmplg(_new_roots_lock);
                    tcellnum = _temp_cells.size();
                }
            }
            if ((_cells_num + tcellnum) >= (Options::get_int("cell_limit"))) {
                Logger::log("MemoryContext", "Out of cells", Logger::ERROR);

                throw std::runtime_error("Out of cells");
            }
        }

        CT *cell = new CT(std::forward<Args>(args)...);

        {
            std::lock_guard tmplg(_new_roots_lock);
            Handle ret(cell);
            _temp_cells.emplace_back(cell);
            if ((_cells_num + _temp_cells.size() + 1) >= (size_t) (Options::get_int("cell_limit") / 2)) { request_gc(); }
            return ret;
        }
    }


    void add_root(Cell *c);
    void remove_root(Cell *c);

    std::list<Cell *> _cells;
    std::atomic<size_t> _cells_num = 0;
    std::list<Cell *> _temp_cells;

    void gc_thread_entry();

    std::map<Cell *, int64_t> _roots;
    std::map<Cell *, int64_t> _new_roots;
    std::recursive_mutex _new_roots_lock;

    std::set<Cell *> _gc_dirty_notif_queue;
    std::mutex _gc_dirty_notif_queue_lock;

    std::atomic<bool> _gc_request = false;
    std::mutex _gc_request_m;
    std::condition_variable _gc_request_cv;

    std::atomic<bool> _gc_done = false;
    std::mutex _gc_done_m;
    std::condition_variable _gc_done_cv;

    std::thread _gc_thread;
    std::atomic<bool> _gc_thread_stop = false;
};

#endif//PSIL_MEMORYCONTEXT_H
