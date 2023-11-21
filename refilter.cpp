#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <string>

enum class Serverity : uint8_t {
    Informational,
    Notice,
    Warning,
    Error,
    Critical
};

struct set {
    std::vector<std::string> categories{};
    std::vector<std::string> groups{};
    std::vector<std::string> devices{};
    std::vector<std::string> units{};
    std::vector<std::string> users{};
    std::vector<std::string> procedures{};
    std::vector<std::string> type_ids{};

    std::vector<Serverity> severities{};

    std::chrono::system_clock::time_point minTime{};
    std::chrono::system_clock::time_point maxTime{};

    int event_id = 0;
    int max_id = 0;
    std::string context{};
    bool include_service_event = false;
    bool acknowledged_events = false;
};

class filter {
public:
    virtual void add_to_set(set& set) const = 0;
};

enum class Property {
    EventId,
    Context
    // ProceduralContext,
    // Priority,
    // Timestamp,
    // TypeId,
    // Message,
    // Subject,
    // Invalid
};

template <typename T, Property property>
class simple_filter : public filter {
public:
    enum class Operator {
        Equals
        // NotEquals,
        // Greater,
        // Less,
        // In,
        // Contains,
        // GreaterOrEquals,
        // LessOrEquals,
        // Invalid
    };

    simple_filter(Operator op, const T& val) : operator_(op), values_{val} {}

    void add_to_set(set& set) const override {
        switch (operator_) {
            case Operator::Equals:
                // Assuming property is a valid enum, set its value to val
                setValue(set);
                break;
            // Handle other cases as needed
            default:
                // Handle other operators
                break;
        }
    }

private:
    void setValue(set& set) const {
        switch (property) {
            case Property::EventId:
                set.event_id = values_[0];
                break;
            case Property::Context:
                set.context = values_[0];
                break;
            // Handle other cases as needed
            default:
                // Handle other properties
                break;
        }
    }

    Operator operator_;
    std::vector<T> values_;
};

int main() {
    // Example usage for "Equals" operator in JSON op: "eq"
    simple_filter<int, Property::EventId> eventIdFilter(simple_filter<int, Property::EventId>::Operator::Equals, 42);

    set mySet;
    eventIdFilter.add_to_set(mySet);

    // Display the result
    std::cout << "Event ID in set: " << mySet.event_id << std::endl;

    return 0;
}
