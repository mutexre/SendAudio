//
//  Option.h
//  SceneGraph
//
//  Created by mutexre on 09/10/15.
//  Copyright © 2015 mutexre. All rights reserved.
//

#pragma once

#include <functional>

using namespace std;

template <typename T>
class Option
{
public:
    bool defined;
    T value;

    Option() : defined(false) {}
    Option(T val) : defined(true), value(val) {}

    void define(T&& value) {
        defined = true;
        this->value = move(value);
    }

    void define(const T& value) {
        defined = true;
        this->value = value;
    }

    void undefine() {
        defined = false;
    }

    T get() const {
        return value;
    }

    T getOrElse(T defaultValue) {
        return defined ? value : defaultValue;
    }

    T getOrExecute(function<void()> f) {
        if (!defined) f();
        return value;
    }

    const T* operator->() const {
        return &value;
    }

    T* operator->() {
        return &value;
    }

    bool operator==(const Option<T>& o) const {
        if (defined == o.defined) {
            if (defined)
                return value == o.value;
            else
                return true;
        }

        return false;
    }

    bool operator!=(const Option<T>& o) const {
        return !operator==(o);
    }

    operator bool() const {
        return defined;
    }
};
