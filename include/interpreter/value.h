#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include <functional>

#include "interpreter/exception.h"
#include "utils.h"

namespace Asampl::Interpreter {

namespace Handler {

class IHandler;

}

struct Undefined {
    constexpr static const char* type_name = "Undefined";

    std::string to_string() const {
        return "UNDEFINED";
    }

    Undefined clone() const {
        return Undefined{};
    }

    bool operator==(const Undefined&) const {
        return true;
    }
};

struct Value;
struct ValuePtr {
    std::shared_ptr<Value> ptr;

    //ValuePtr(ValuePtr&&) = default;
    ValuePtr() : ValuePtr{Undefined{}} {};
    ValuePtr(const ValuePtr&) = default;
    ValuePtr(std::nullptr_t) : ptr{nullptr} {}
    ValuePtr(Value&& value)
        : ptr{std::make_shared<Value>(std::move(value))}
    {
    }

    template< typename T >
    ValuePtr(T&& value)
    {
        if constexpr (std::is_same_v<std::decay_t<T>, ValuePtr>) {
            ptr = value.ptr;
        } else {
            ptr = std::make_shared<Value>(std::forward<T>(value));
        }
    }

    ValuePtr& operator=(const ValuePtr&) = default;
    ValuePtr& operator=(ValuePtr&&) = default;

    Value& operator*() {
        return *ptr;
    }

    Value& operator*() const {
        return *ptr;
    }

    Value* operator->() {
        return ptr.get();
    }

    Value* operator->() const {
        return ptr.get();
    }

    bool operator==(std::nullptr_t) const {
        return ptr == nullptr;
    }

    bool operator!=(std::nullptr_t) const {
        return ptr != nullptr;
    }
};

}


namespace std {

template<> struct hash<Asampl::Interpreter::ValuePtr> {
    std::size_t operator()(const Asampl::Interpreter::ValuePtr& ptr) const noexcept {
        hash<std::shared_ptr<Asampl::Interpreter::Value>> hasher;
        return hasher(ptr.ptr);
    }
};

}

namespace Asampl::Interpreter {

using Byte = uint8_t;

struct Number {
    constexpr static const char* type_name = "Number";

    double value;

    Number(double value)
        : value{value}
    {
    }

    Number(size_t value)
        : value{static_cast<double>(value)}
    {
    }

    std::string to_string() const {
        return std::to_string(value);
    }

    Number clone() const {
        return *this;
    }

    bool operator==(const Number& other) const {
        return value == other.value;
    }

    operator double() const {
        return value;
    }

    int32_t as_int() const {
        return value;
    }

    size_t as_size() const {
        return value;
    }
};

struct Bool {
    constexpr static const char* type_name = "Bool";

    bool value;

    std::string to_string() const {
        return value ? "TRUE" : "FALSE";
    }

    Bool clone() const {
        return *this;
    }

    bool operator==(const Bool& other) const {
        return value == other.value;
    }
};

struct String {
    constexpr static const char* type_name = "String";

    std::string value;

    String(std::string value)
        : value{std::move(value)}
    {
    }

    std::string to_string() const {
        return value;
    }

    String clone() const {
        return *this;
    }

    bool operator==(const String& other) const {
        return value == other.value;
    }
};

struct Image {
    constexpr static const char* type_name = "Image";

    size_t width;
    size_t height;
    std::vector<Byte> data;

    constexpr static size_t channels = 3;

    std::string to_string() const {
        return "<Image " + std::to_string(width) + " " + std::to_string(height) + ">";
    }

    Image clone() const {
        return *this;
    }

    bool operator==(const Image& other) const {
        return width == other.width && height == other.height && data == other.data;
    }
};

struct Tuple {
    constexpr static const char* type_name = "Tuple";

    std::vector<ValuePtr> values;

    std::string to_string() const;

    Tuple clone() const {
        return *this;
    }

    bool operator==(const Tuple& other) const;

    size_t convert_index(int32_t i) const {
        if (i >= 0) {
            return i;
        } else {
            return values.size() + i;
        }
    }

    ValuePtr& operator[](int32_t i) {
        const auto idx = convert_index(i);
        if (idx < values.size()) {
            return values[idx];
        } else {
            throw InterpreterException("Tuple index out of range");
        }
    }

    const ValuePtr& operator[](int32_t i) const {
        const auto idx = convert_index(i);
        if (idx < values.size()) {
            return values[idx];
        } else {
            throw InterpreterException("Tuple index out of range");
        }
    }
};

struct Map {
    constexpr static const char* type_name = "Map";

    std::unordered_map<std::string, ValuePtr> string_map;

    std::string to_string() const;

    // TODO
    void set(const ValuePtr& key, const ValuePtr& value);
    ValuePtr& get(const ValuePtr& key);
    bool has(const ValuePtr& key);

    void set(const std::string& key, const ValuePtr& value);
    ValuePtr& get(const std::string& key);
    bool has(const std::string& key);

    void set(const char* key, const ValuePtr& value);
    ValuePtr& get(const char* key);
    bool has(const char* key);

    ValuePtr& strict_get(const std::string& key);

    Map clone() const {
        return *this;
    }

    bool operator==(const Map& other) const {
        return false; // TODO
    }
};

struct ByteArray {
    constexpr static const char* type_name = "ByteArray";

    std::vector<Byte> data;

    std::string to_string() const;

    ByteArray clone() const {
        return *this;
    }

    bool operator==(const ByteArray& other) const {
        return data == other.data;
    }
};

struct Function {
    constexpr static const char* type_name = "Function";

    std::string name;
    std::function<ValuePtr(Utils::Slice<ValuePtr>)> func;

    std::string to_string() const {
        return "<Function " + name + ">";
    }

    Function clone() const {
        return *this;
    }

    bool operator==(const Function& other) const {
        assert(false && "function comparison not implemented");
    }
};

struct HandlerValue {
    constexpr static const char* type_name = "Handler";

    std::string name;
    std::shared_ptr<Handler::IHandler> handler_ptr;

    std::string to_string() const {
        return "<handler " + name + ">";
    }

    HandlerValue clone() const {
        return *this;
    }

    bool operator==(const HandlerValue& other) const {
        assert(false && "handler comparison not implemented");
    }
};

template< typename T >
struct ValueVisitor {
public:
    using Result = T;
    
    virtual Result operator()(Undefined&) = 0;
    virtual Result operator()(Number&) = 0;
    virtual Result operator()(Bool&) = 0;
    virtual Result operator()(String&) = 0;
    virtual Result operator()(Image&) = 0;
    virtual Result operator()(Tuple&) = 0;
    virtual Result operator()(ByteArray&) = 0;
    virtual Result operator()(Map&) = 0;
    virtual Result operator()(Function&) = 0;
    virtual Result operator()(HandlerValue&) = 0;
};

struct Value {
    using VariantType = std::variant<
        Undefined,
        Number,
        Bool,
        String,
        Image,
        Tuple,
        ByteArray,
        Map,
        Function,
        HandlerValue
        >;

    VariantType variant;

    Value(const Value&) = default;
    Value(Value&&) = default;

    template< typename T >
    Value(T&& t)
        : variant(t)
    {
    }

    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = default;

    template< typename T >
    static ValuePtr make_ptr(T&& t) {
        return ValuePtr{Value{std::forward<T>(t)}};
    }

    template< typename T >
    constexpr bool is() const {
        return std::holds_alternative<std::decay_t<T>>(variant);
    }

    template< typename T >
    constexpr T& get() {
        if (is<T>()) {
            return std::get<std::decay_t<T>>(variant);
        } else {
            const auto& expected = std::decay_t<T>::type_name;
            const auto& got = std::visit([](const auto& val) {
                return std::decay_t<decltype(val)>::type_name;
            }, variant);

            throw InterpreterException(std::string{"Wrong type. Expected "} + expected + ", got " + got);
        }
    }

    template< typename T >
    constexpr const T& get() const {
        if (is<T>()) {
            return std::get<std::decay_t<T>>(variant);
        } else {
            const auto& expected = std::decay_t<T>::type_name;
            const auto& got = std::visit([](const auto& val) {
                return std::decay_t<decltype(val)>::type_name;
            }, variant);

            throw InterpreterException(std::string{"Wrong type. Expected "} + expected + ", got " + got);
        }
    }

    template< typename T >
    constexpr std::optional<std::reference_wrapper<T>> try_get() {
        if (is<T>()) {
            return get<T>();
        } else {
            return std::nullopt;
        }
    }

    template< typename T >
    constexpr std::optional<std::reference_wrapper<const T>> try_get() const {
        if (is<T>()) {
            return get<T>();
        } else {
            return std::nullopt;
        }
    }

    Value clone() const {
        return std::visit([](const auto& value) {
            return Value{value.clone()};
        }, variant);
    }

    ValuePtr clone_ptr() const {
        return ValuePtr(clone());
    }

    std::string to_string() const {
        return std::visit([](const auto& value) {
                return value.to_string();
        }, variant);
    }

    template< typename V >
    typename V::Result apply(V& visitor) {
        return std::visit(visitor, variant);
    }

    bool operator==(const Value& other) const {
        if (variant.index() != other.variant.index()) {
            return false;
        }

        return std::visit([&](const auto& value) -> bool {
            return value == other.get<decltype(value)>();
        }, variant);
    }

    bool operator!=(const Value& other) const {
        return !(*this == other);
    }
};

}
