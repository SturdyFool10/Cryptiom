#ifndef RESULT_H
#define RESULT_H

#include <stdexcept>
#include <utility>
#include <type_traits>
#include <sstream>

namespace result {

template<typename T, typename E>
class Result {

    bool is_ok_;
    union {
        T ok_value;
        E err_value;
    };

public:
    explicit Result(const T& value) : is_ok_(true), ok_value(value) {}
    explicit Result(const E& error) : is_ok_(false), err_value(error) {}

    Result(const Result& other) : is_ok_(other.is_ok_) {
        if (is_ok_) {
            new (&ok_value) T(other.ok_value);
        } else {
            new (&err_value) E(other.err_value);
        }
    }

    Result& operator=(const Result& other) {
        if (this != &other) {
            this->~Result();
            is_ok_ = other.is_ok_;
            if (is_ok_) {
                new (&ok_value) T(other.ok_value);
            } else {
                new (&err_value) E(other.err_value);
            }
        }
        return *this;
    }

    Result(Result&& other) noexcept : is_ok_(other.is_ok_) {
        if (is_ok_) {
            new (&ok_value) T(std::move(other.ok_value));
        } else {
            new (&err_value) E(std::move(other.err_value));
        }
    }

    Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            this->~Result();
            is_ok_ = other.is_ok_;
            if (is_ok_) {
                new (&ok_value) T(std::move(other.ok_value));
            } else {
                new (&err_value) E(std::move(other.err_value));
            }
        }
        return *this;
    }

    ~Result() {
        if (is_ok_) {
            ok_value.~T();
        } else {
            err_value.~E();
        }
    }

    [[nodiscard]] bool is_ok() const noexcept {
        return is_ok_;
    }

    [[nodiscard]] bool is_err() const noexcept {
        return !is_ok_;
    }

    [[nodiscard]] T unwrap() const & {
        if (!is_ok_) {
            std::stringstream ss;
            ss << "Called unwrap() on an error value containing: " << err_value;
            throw std::runtime_error(ss.str());
        }
        return ok_value;
    }

    [[nodiscard]] T unwrap() && {
        if (!is_ok_) {
            std::stringstream ss;
            ss << "Called unwrap() on an error value containing: " << err_value;
            throw std::runtime_error(ss.str());
        }
        return std::move(ok_value);
    }

    template<typename U = T>
    [[nodiscard]] T unwrap_or(U&& default_value) const & {
        static_assert(std::is_convertible<U, T>::value, "Default value must be convertible to T");
        if (is_ok_) {
            return ok_value;
        }
        return std::forward<U>(default_value);
    }

    [[nodiscard]] E unwrap_err() const & {
        if (is_ok_) {
            throw std::runtime_error("Called unwrap_err() on an ok value");
        }
        return err_value;
    }

    [[nodiscard]] E unwrap_err() && {
        if (is_ok_) {
            throw std::runtime_error("Called unwrap_err() on an ok value");
        }
        return std::move(err_value);
    }

    static Result<T,E> Ok(T value) {
        return Result<T,E>(std::forward<T>(value));
    }

    static Result<T,E> Err(E error) {
        return Result<T,E>(std::forward<E>(error));
    }
};

} // namespace result

#endif
