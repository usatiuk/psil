//
// Created by Stepan Usatiuk on 27.12.2023.
//

#include "Compiler.h"

#include <string>
#include <functional>

Handle Compiler::compile(Handle src, Handle fake_env, Handle suffix) {
    Handle out;

    std::function<Handle(Handle)> compileArgsRaw = [&](Handle args) {
        Handle out;
        while (!args.null()) {
            out.splice(compile(args.car(), fake_env));
            args = args.cdr();
        }
        return out;
    };

    std::function<Handle(Handle, Handle)> compileArgsList = [&](Handle args, Handle env) {
        Handle out;
        out.append(Handle("NIL"));
        while (!args.null()) {
            out.splice(compile(args.car(), env));
            out.append(Handle("CONS"));
            args = args.cdr();
        }
        return out;
    };


    Handle expr = src;
    if (expr.null()) {
        out.append(Handle("NIL"));
    } else if (expr.atom()) {
        if (expr.type() == CellType::NUMATOM) {
            out.append(Handle("LDC"));
            out.append(expr);
        } else if (expr.type() == CellType::STRATOM) {
            Handle idx = findIndex(expr, fake_env);
            if (idx == nullptr) {
                out.append(Handle("LDC"));
                out.append(expr);
            } else {
                out.append(Handle("LD"));
                out.append(idx);
            }
        }
    } else {
        Handle car = expr.car();
        Handle cdr = expr.cdr();
        if (car.atom()) {
            if (car.strval() == "+") {
                out.splice(compileArgsRaw(cdr));
                out.append(Handle("ADD"));
            } else if (car.strval() == "read") {
                out.append(Handle("READ"));
            } else if (car.strval() == "lambda") {
                out.append(Handle("LDF"));
                out.append(compile(cdr.cdr().car(), Handle::cons(cdr.car(), fake_env), Handle("RET")));
            } else if (car.strval() == "if") {
                out.splice(compile(cdr.car(), fake_env));
                out.append(Handle("SEL"));
                out.append(compile(cdr.cdr().car(), fake_env, Handle("JOIN")));
                out.append(compile(cdr.cdr().cdr().car(), fake_env, Handle("JOIN")));
            } else if (car.strval() == "let" || car.strval() == "letrec") {
                std::vector<std::pair<Handle, Handle>> argBody;

                Handle definitions = cdr.car();

                Handle argNames;
                Handle argBodies;

                Handle body = cdr.cdr().car();

                while (!definitions.null()) {
                    argBody.emplace_back(definitions.car().car(), definitions.car().cdr().car());
                    argNames.append(definitions.car().car());
                    argBodies.append(definitions.car().cdr().car());
                    definitions = definitions.cdr();
                }

                Handle newenv = Handle::cons(argNames, fake_env);
                if (car.strval() == "let")
                    out.splice(compileArgsList(argBodies, fake_env));
                else if (car.strval() == "letrec") {
                    out.append(Handle("DUM"));
                    out.splice(compileArgsList(argBodies, newenv));
                }

                out.append(Handle("LDF"));
                out.append(compile(body, newenv, Handle("RET")));
                if (car.strval() == "let")
                    out.append(Handle("AP"));
                else
                    out.append(Handle("RAP"));
            } else {
                out.splice(compileArgsList(cdr, fake_env));

                out.append(Handle("LD"));
                Handle idx = findIndex(car, fake_env);
                out.append(idx);
                out.append(Handle("AP"));
            }
        } else {
            out.splice(compileArgsList(cdr, fake_env));
            out.splice(compile(car, fake_env, Handle("AP")));
        }
    }
    out.splice(suffix);
    return out;
}
Handle Compiler::findIndex(Handle symbol, Handle env) {
    int64_t frame = 1;

    Handle curFrame = env;

    while (!curFrame.null()) {
        int64_t arg = 1;
        Handle curArg = curFrame.car();

        while (!curArg.null()) {
            if (curArg.car() == symbol) return Handle::cons(frame, arg);
            curArg = curArg.cdr();
            arg++;
        }

        curFrame = curFrame.cdr();
        frame++;
    }

    return Handle(nullptr);
}
