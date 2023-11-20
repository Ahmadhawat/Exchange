#include <algorithm>  // For std::find

class filter {
public:
    virtual bool apply(const set& s) const = 0;
};

enum class Property { EventId, Context, ProceduralContext, Priority, Timestamp, TypeId, Message, Subject, Invalid };

template<typename T, Property property>
class simple_filter : public filter {
public:
    enum class Operator { Equals, NotEquals, Greater, Less, In, Contains, GreaterOrEquals, LessOrEquals, Invalid };

    simple_filter(Operator op, const std::vector<T>& values)
        : operator_(op), values_(values) {}

    bool apply(const set& s) const override {
        // Implement logic to apply the filter based on the property and operator
        switch (property) {
            case Property::EventId:
                return applyFilter(s.event_id);
            // Add cases for other properties if needed
            default:
                return false;  // Unsupported property
        }
    }

private:
    bool applyFilter(const T& propertyValue) const {
        // Implement logic based on the operator and property value
        switch (operator_) {
            case Operator::Equals:
                return std::find(values_.begin(), values_.end(), propertyValue) != values_.end();
            // Add cases for other operators if needed
            default:
                return false;  // Unsupported operator
        }
    }

private:
    Operator operator_;
    std::vector<T> values_;
};

class id_filter : public simple_filter<int64_t, Property::EventId> {
public:
    id_filter(Operator op, const std::vector<int64_t>& values)
        : simple_filter<int64_t, Property::EventId>(op, values) {}
};

class filter_parameter {
public:
    filter_parameter(const std::string& json);

private:
    std::unique_ptr<filter> parsedFilter_;
    // ... (rest of the class remains the same)

    bool applyFilter(const set& s) const {
        if (parsedFilter_) {
            return parsedFilter_->apply(s);
        }
        return false;  // No filter or parsing error
    }
};

// Update parseSimpleFilter to create the appropriate filter object
void filter_parameter::parseSimpleFilter(const Poco::JSON::Object::Ptr& object) {
    std::string name = object->getValue<std::string>("name");
    std::string op = object->getValue<std::string>("op");
    Poco::Dynamic::Var val = object->get("val");

    // Create the appropriate filter based on the property
    if (name == "id") {
        parsedFilter_ = std::make_unique<id_filter>(parseOperator(op), parseValues<int64_t>(val));
    }
    // Add cases for other properties if needed
}

// Helper method to convert operator string to enum
simple_filter<int64_t, Property::EventId>::Operator parseOperator(const std::string& op) {
    // Implement logic to convert operator string to enum
    // Example: if (op == "eq") return simple_filter<int64_t, Property::EventId>::Operator::Equals;
    // Add cases for other operators if needed
}

// Helper method to convert Dynamic::Var to vector of values
template <typename T>
std::vector<T> parseValues(const Poco::Dynamic::Var& val) {
    // Implement logic to convert Dynamic::Var to vector of values
    // Example: if (val.isInteger()) return {val.convert<T>()};
    // Add cases for other types if needed
}
