//
// Created by Stepan Usatiuk on 27.12.2023.
//

#include "Compiler.h"


#include <functional>
#include <stack>
#include <stdexcept>
#include <string>

#include "Command.h"
#include "Logger.h"


using namespace Command;


static std::unordered_map<std::string_view, CommandE> builtins{
        {"+", ADD}, {"-", SUB},     {"cons", CONS}, {"car", CAR},     {"cdr", CDR},   {"=", EQ},
        {">", GT},  {"<", LT},      {"nil", NIL},   {"nil?", NILC},   {"atom", ATOM}, {"*", MULT},
        {"/", DIV}, {"read", READ}, {"eval", EVAL}, {"print", PRINT}, {"quit", QUIT}};

Handle Compiler::compile(const Handle &src, Handle fake_env, const Handle &suffix) {
    Handle out;

    std::function<Handle(Handle)> compileArgsRaw = [&](Handle args) {
        Handle out;
        std::stack<Handle> rev;
        while (!args.null()) {
            rev.push(args.car());
            args = args.cdr();
        }
        while (!rev.empty()) {
            out.splice(compile(rev.top(), fake_env));
            rev.pop();
        }
        return out;
    };

    std::function<Handle(Handle, Handle)> compileArgsList = [&](Handle args, const Handle &env) {
        Handle out;
        out.append(make_cmd(NIL));
        std::stack<Handle> rev;
        while (!args.null()) {
            rev.push(args.car());
            args = args.cdr();
        }
        while (!rev.empty()) {
            out.splice(compile(rev.top(), env));
            out.append(make_cmd(CONS));
            rev.pop();
        }
        return out;
    };


    Handle expr = src;
    if (expr.null()) {
        out.append(make_cmd(NIL));
    } else if (expr.atom()) {
        if (expr.type() == CellType::NUMATOM) {
            out.append(make_cmd(LDC));
            out.append(expr);
        } else if (expr.type() == CellType::STRATOM) {
            Handle idx = findIndex(expr, fake_env);
            if (idx == nullptr) {
                out.append(make_cmd(LDC));
                out.append(expr);
            } else {
                out.append(make_cmd(LD));
                out.append(idx);
            }
        }
    } else {
        Handle car = expr.car();
        Handle cdr = expr.cdr();
        if (car.atom()) {
            if (car.strval() == "quote") {
                out.append(make_cmd(LDC));
                out.splice(Handle::cons(cdr.car(), cdr.cdr()));
            } else if (builtins.find(car.strval()) != builtins.end()) {
                out.splice(compileArgsRaw(cdr));
                out.append(make_cmd(builtins.at(car.strval())));
            } else if (car.strval() == "lambda") {
                out.append(make_cmd(LDF));
                out.append(compile(cdr.cdr().car(), Handle::cons(cdr.car(), fake_env), make_cmd(RET)));
            } else if (car.strval() == "if") {
                out.splice(compile(cdr.car(), fake_env));
                out.append(make_cmd(SEL));
                out.append(compile(cdr.cdr().car(), fake_env, make_cmd(JOIN)));
                out.append(compile(cdr.cdr().cdr().car(), fake_env, make_cmd(JOIN)));
            } else if (car.strval() == "define") {
                fake_env.car().append(Handle(std::string(cdr.car().car().strval())));
                out.append(make_cmd(LDG));
                out.append(compile(cdr.cdr().car(), Handle::cons(cdr.car().cdr(), fake_env), make_cmd(RET)));
            } else if (car.strval() == "let" || car.strval() == "letrec") {
                Handle definitions = cdr.car();

                Handle argNames;
                Handle argBodies;

                Handle body = cdr.cdr().car();

                while (!definitions.null()) {
                    argNames.append(definitions.car().car());
                    argBodies.append(definitions.car().cdr().car());
                    definitions = definitions.cdr();
                }

                Handle newenv = Handle::cons(argNames, fake_env);
                if (car.strval() == "let") {
                    out.splice(compileArgsList(argBodies, fake_env));
                    out.append(make_cmd(LDF));
                    out.append(compile(body, newenv, make_cmd(RET)));
                    out.append(make_cmd(AP));
                } else if (car.strval() == "letrec") {
                    out.append(make_cmd(DUM));
                    out.splice(compileArgsList(argBodies, newenv));
                    out.append(make_cmd(LDF));
                    out.append(compile(body, newenv, make_cmd(RET)));
                    out.append(make_cmd(RAP));
                }
            } else {
                out.splice(compileArgsList(cdr, fake_env));

                out.append(make_cmd(LD));
                Handle idx = findIndex(car, fake_env);
                if (idx.null())
                    throw std::invalid_argument("Could not find function to apply: " + std::string(car.strval()));
                out.append(idx);
                out.append(make_cmd(AP));
            }
        } else {
            out.splice(compileArgsList(cdr, fake_env));
            out.splice(compile(car, fake_env, make_cmd(AP)));
        }
    }
    out.splice(suffix);
    return out;
}
Handle Compiler::findIndex(const Handle &symbol, const Handle &env) {
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
