#ifndef OPTION_H
#define OPTION_H

#include <stdexcept>
#include <utility>

template<typename T>
class Option {
private:
    bool has_value_ = false;
    union {
        T value_;
    };

public:
    Option() noexcept : has_value_(false) {}

    Option(const T& value) : has_value_(true) {
        new (&value_) T(value);
    }

    Option(T&& value) noexcept : has_value_(true) {
        new (&value_) T(std::move(value));
    }

    Option(const Option& other) : has_value_(other.has_value_) {
        if (has_value_) {
            new (&value_) T(other.value_);
        }
    }

    Option(Option&& other) noexcept : has_value_(other.has_value_) {
        if (has_value_) {
            new (&value_) T(std::move(other.value_));
        }
        other.has_value_ = false;
    }

    ~Option() {
        if (has_value_) {
            value_.~T();
        }
    }

    Option& operator=(const Option& other) {
        if (this != &other) {
            if (has_value_) {
                value_.~T();
            }
            has_value_ = other.has_value_;
            if (has_value_) {
                new (&value_) T(other.value_);
            }
        }
        return *this;
    }

    Option& operator=(Option&& other) noexcept {
        if (this != &other) {
            if (has_value_) {
                value_.~T();
            }
            has_value_ = other.has_value_;
            if (has_value_) {
                new (&value_) T(std::move(other.value_));
            }
            other.has_value_ = false;
        }
        return *this;
    }

    bool has_value() const noexcept {
        return has_value_;
    }

    T& value() & {
        if (!has_value_) throw std::runtime_error("Option has no value");
        return value_;
    }

    const T& value() const& {
        if (!has_value_) throw std::runtime_error("Option has no value");
        return value_;
    }

    T value_or(const T& default_value) const {
        return has_value_ ? value_ : default_value;
    }

    static Option<T> some(T val) {
        return Option<T>(val);
    }

    static Option<T> none() {
        return Option<T>();
    }
};

#endif // OPTION_H
