#include <iostream>
#include <Poco/NumberParser.h>
#include <Poco/Exception.h>

// Abstract class for comparison operations
class Comparator {
public:
    virtual bool compare(int a, int b) const = 0;
    virtual ~Comparator() = default;
};

// Concrete class for greater than comparison
class GreaterThan : public Comparator {
public:
    bool compare(int a, int b) const override {
        return a > b;
    }
};

// Concrete class for less than comparison
class LessThan : public Comparator {
public:
    bool compare(int a, int b) const override {
        return a < b;
    }
};

// Function to perform the comparison based on the input string
bool performComparison(const std::string& input, int value1, int value2, const Comparator& comparator) {
    return comparator.compare(value1, value2);
}

int main() {
    try {
        std::string inputString;
        std::cout << "Enter a string containing two integers and a comparison operator (e.g., '5 10 >'): ";
        std::getline(std::cin, inputString);

        int value1, value2;
        char op;

        // Parse the input string
        std::istringstream iss(inputString);
        if (!(iss >> value1 >> value2 >> op)) {
            throw Poco::SyntaxException("Invalid input format");
        }

        // Create the appropriate comparator based on the comparison operator
        std::unique_ptr<Comparator> comparator;
        switch (op) {
            case '>':
                comparator = std::make_unique<GreaterThan>();
                break;
            case '<':
                comparator = std::make_unique<LessThan>();
                break;
            default:
                throw Poco::SyntaxException("Invalid operator");
        }

        // Perform the comparison and print the result
        bool result = performComparison(inputString, value1, value2, *comparator);
        std::cout << "Result: " << std::boolalpha << result << std::endl;
    } catch (const Poco::Exception& e) {
        std::cerr << "Error: " << e.displayText() << std::endl;
    }

    return 0;
}

#include <iostream>
#include <Poco/NumberParser.h>
#include <Poco/Exception.h>
#include <vector>
#include <stdexcept>

// Abstract class for different types
class Type {
public:
    virtual ~Type() = default;
    virtual bool isEqual(const Type& other) const = 0;
    virtual bool isNotEqual(const Type& other) const = 0;
    virtual bool isGreater(const Type& other) const = 0;
    virtual bool isLess(const Type& other) const = 0;
    virtual void print() const = 0;
};

// Concrete class for integer type
class IntegerType : public Type {
private:
    int value;

public:
    IntegerType(int val) : value(val) {}

    bool isEqual(const Type& other) const override {
        const IntegerType* otherInt = dynamic_cast<const IntegerType*>(&other);
        return otherInt && value == otherInt->value;
    }

    bool isNotEqual(const Type& other) const override {
        return !isEqual(other);
    }

    bool isGreater(const Type& other) const override {
        const IntegerType* otherInt = dynamic_cast<const IntegerType*>(&other);
        return otherInt && value > otherInt->value;
    }

    bool isLess(const Type& other) const override {
        const IntegerType* otherInt = dynamic_cast<const IntegerType*>(&other);
        return otherInt && value < otherInt->value;
    }

    void print() const override {
        std::cout << "Integer: " << value;
    }
};

// Concrete class for boolean type
class BooleanType : public Type {
private:
    bool value;

public:
    BooleanType(bool val) : value(val) {}

    bool isEqual(const Type& other) const override {
        const BooleanType* otherBool = dynamic_cast<const BooleanType*>(&other);
        return otherBool && value == otherBool->value;
    }

    bool isNotEqual(const Type& other) const override {
        return !isEqual(other);
    }

    bool isGreater(const Type& other) const override {
        throw std::runtime_error("Cannot compare boolean values");
    }

    bool isLess(const Type& other) const override {
        throw std::runtime_error("Cannot compare boolean values");
    }

    void print() const override {
        std::cout << "Boolean: " << std::boolalpha << value;
    }
};

// Function to perform the operation based on the input string
bool performOperation(const std::string& operation, const Type& value1, const Type& value2) {
    if (operation == "equal") {
        return value1.isEqual(value2);
    } else if (operation == "notequal") {
        return value1.isNotEqual(value2);
    } else if (operation == "greater") {
        return value1.isGreater(value2);
    } else if (operation == "less") {
        return value1.isLess(value2);
    } else {
        throw Poco::SyntaxException("Invalid operation");
    }
}

int main() {
    try {
        std::string type, operation;
        std::cout << "Enter type (int/boolean): ";
        std::cin >> type;

        std::cout << "Enter operation (equal/notequal/greater/less): ";
        std::cin >> operation;

        // Create the appropriate type based on user input
        std::unique_ptr<Type> value1;
        std::unique_ptr<Type> value2;

        if (type == "int") {
            int intValue1, intValue2;
            std::cout << "Enter integer values: ";
            std::cin >> intValue1 >> intValue2;
            value1 = std::make_unique<IntegerType>(intValue1);
            value2 = std::make_unique<IntegerType>(intValue2);
        } else if (type == "boolean") {
            bool boolValue1, boolValue2;
            std::cout << "Enter boolean values (true/false): ";
            std::cin >> std::boolalpha >> boolValue1 >> boolValue2;
            value1 = std::make_unique<BooleanType>(boolValue1);
            value2 = std::make_unique<BooleanType>(boolValue2);
        } else {
            throw Poco::SyntaxException("Invalid type");
        }

        // Perform the operation and print the result
        bool result = performOperation(operation, *value1, *value2);
        std::cout << "Result: " << std::boolalpha << result << std::endl;

        // Print the values
        std::cout << "Value 1: ";
        value1->print();
        std::cout << std::endl;

        std::cout << "Value 2: ";
        value2->print();
        std::cout << std::endl;

    } catch (const Poco::Exception& e) {
        std::cerr << "Error: " << e.displayText() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

#include <iostream>
#include <Poco/NumberParser.h>
#include <Poco/Exception.h>
#include <vector>
#include <stdexcept>
#include <sstream>

template <typename T>
class Operator {
private:
    T value;

public:
    Operator(const std::string& strValue) {
        std::istringstream iss(strValue);
        if (!(iss >> value)) {
            throw Poco::SyntaxException("Invalid input format");
        }
    }

    bool isEqual(const T& other) const {
        return value == other;
    }

    bool isNotEqual(const T& other) const {
        return value != other;
    }

    bool isGreater(const T& other) const {
        return value > other;
    }

    bool isLess(const T& other) const {
        return value < other;
    }

    T get() const {
        return value;
    }
};

// Function to perform the operation based on the input string
template <typename T>
bool performOperation(const std::string& operation, const Operator<T>& value1, const Operator<T>& value2) {
    if (operation == "equal") {
        return value1.isEqual(value2.get());
    } else if (operation == "notequal") {
        return value1.isNotEqual(value2.get());
    } else if (operation == "greater") {
        return value1.isGreater(value2.get());
    } else if (operation == "less") {
        return value1.isLess(value2.get());
    } else {
        throw Poco::SyntaxException("Invalid operation");
    }
}

int main() {
    try {
        std::string inputString;
        std::cout << "Enter a string (e.g., '5 equal 10'): ";
        std::getline(std::cin, inputString);

        std::istringstream iss(inputString);
        std::string strValue1, operation, strValue2;

        if (!(iss >> strValue1 >> operation >> strValue2)) {
            throw Poco::SyntaxException("Invalid input format");
        }

        // Create the appropriate type based on user input
        Operator<int> value1(strValue1);
        Operator<int> value2(strValue2);

        // Perform the operation and print the result
        bool result = performOperation(operation, value1, value2);
        std::cout << "Result: " << std::boolalpha << result << std::endl;
    } catch (const Poco::Exception& e) {
        std::cerr << "Error: " << e.displayText() << std::endl;
    }

    return 0;
}

