#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class Property { EventId, Context, ProceduralContext, Priority, Timestamp, TypeId, Message, Subject, Invalid };

template <typename T, Property property>
class simple_filter {
public:
    enum class Operator { Equals, NotEquals, Greater, Less, In, Contains, GreaterOrEquals, LessOrEquals, Invalid };

    Operator op;
    std::vector<T> values;

    simple_filter(Operator op, std::vector<T> values) : op(op), values(std::move(values)) {}
};

class filter {
public:
    virtual void add_to_set(std::set<int>& set) const = 0;
};

template <typename T, Property property>
class simple_filter : public filter {
public:
    using Operator = typename simple_filter<T, property>::Operator;

    Operator op;
    std::vector<T> values;

    simple_filter(Operator op, std::vector<T> values) : op(op), values(std::move(values)) {}

    void add_to_set(std::set<int>& set) const override {
        // Implement logic to add filter to the set based on the property, operator, and values
        // This is just a placeholder; you need to implement it according to your needs.
        // You may need to use a switch statement based on the property enum.
        // For simplicity, I'm just printing the information here.
        std::cout << "Adding filter to set: Property=" << static_cast<int>(property)
                  << ", Operator=" << static_cast<int>(op) << ", Values=";
        for (const auto& value : values) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    std::string simpleFilterJson = R"({"name":"id", "op":"eq","val":["string"]})";

    json j = json::parse(simpleFilterJson);

    std::string name = j["name"];
    std::string op = j["op"];

    if (op == "eq") {
        std::vector<std::string> values = j["val"];
        simple_filter<std::string, Property::EventId> myFilter(simple_filter<std::string, Property::EventId>::Operator::Equals, values);
        // You can use myFilter as needed, for example, add it to a set
        std::set<int> mySet;
        myFilter.add_to_set(mySet);
    }

    return 0;
}
