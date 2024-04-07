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
#include <queue>
#include <set>

#ifndef NO_THREADS
#include <mutex>
#include <thread>
#endif

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
#ifndef NO_THREADS
        std::unique_lock l(_gc_done_m);
        _gc_done = false;
        { request_gc(); }
        if (!_gc_done) _gc_done_cv.wait(l, [&] { return _gc_done.load(); });
#else
        gc_thread_entry();
#endif
    }

    void request_gc() {
#ifndef NO_THREADS
        std::lock_guard l(_gc_request_m);
        _gc_request = true;
        _gc_request_cv.notify_all();
#endif
    }

    size_t cell_count() const { return _cells_num; }

    template<typename R>
    R run_dirty(const std::function<R(std::function<void(Cell *)>)> &f) {
#ifndef NO_THREADS
        std::lock_guard l(_gc_dirty_notif_queue_lock);
        return f([&](Cell *c) {
            if (c == nullptr) return;
            Logger::log(Logger::MemoryContext, [&](std::ostream &out) { out << "marked dirty: " << c; }, Logger::DEBUG);
            _gc_dirty_notif_queue.emplace(c);
        });
#else
        return f([](Cell *c) {});
#endif
    }

private:
    template<typename CT, typename... Args>
    Handle alloc_cell(Args... args) {

        size_t tcellnum;
        {
#ifndef NO_THREADS
            std::lock_guard tmplg(_new_roots_lock);
#endif
            tcellnum = _temp_cells.size();
        }

        if ((_cells_num + tcellnum) >= (Options::get<size_t>("cell_limit"))) {
            // We might need to run GC twice as it has to process the messages;
            Logger::log(Logger::MemoryContext, "Running forced gc", Logger::ERROR);
            for (int i = 0; i < 3 && (_cells_num + tcellnum) >= (Options::get<size_t>("cell_limit")); i++) {
                request_gc_and_wait();
                {
#ifndef NO_THREADS
                    std::lock_guard tmplg(_new_roots_lock);
#endif
                    tcellnum = _temp_cells.size();
                }
            }
            if ((_cells_num + tcellnum) >= (Options::get<size_t>("cell_limit"))) {
                Logger::log(Logger::MemoryContext, "Out of cells", Logger::ERROR);

                throw std::runtime_error("Out of cells");
            }
        }

        CT *cell = new CT(std::forward<Args>(args)...);

        {
#ifndef NO_THREADS
            std::lock_guard tmplg(_new_roots_lock);
#endif
            Handle ret(cell);
            _temp_cells.emplace_back(cell);
            if ((_cells_num + _temp_cells.size()) >=
                ((Options::get<size_t>("cell_limit") * Options::get<size_t>("gc_threshold")) / 100ULL)) {
                request_gc();
            }
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
#ifndef NO_THREADS
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
#endif
};

#endif//PSIL_MEMORYCONTEXT_H
