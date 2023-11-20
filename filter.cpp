#include <algorithm>  // For std::find
#include <iostream>

struct set {
    // ... (unchanged)

    int64_t event_id = 0;  // Added for testing purposes
    int priority = 0;      // Added for testing purposes
};

class filter {
public:
    virtual bool apply(const set& s) const = 0;
};

enum class Property { EventId, Context, ProceduralContext, Priority, Timestamp, TypeId, Message, Subject, Invalid };

template <typename T, Property property>
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

// Define other simple_filter classes for different properties if needed

class filter_parameter {
public:
    filter_parameter(const std::string& json);

private:
    std::unique_ptr<filter> parsedFilter_;

    bool applyFilter(const set& s) const;

    std::unique_ptr<filter> parseFilter(const Poco::JSON::Object::Ptr& object);
    std::unique_ptr<filter> parseCompositeFilter(const Poco::JSON::Object::Ptr& object, const std::string& logicalOperator);
    std::unique_ptr<filter> parseSimpleFilter(const Poco::JSON::Object::Ptr& object);
    simple_filter<int64_t, Property::EventId>::Operator parseOperator(const std::string& op);
    template <typename T> std::vector<T> parseValues(const Poco::Dynamic::Var& val);
};

filter_parameter::filter_parameter(const std::string& json) {
    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        parsedFilter_ = parseFilter(object);
    } catch (Poco::Exception& e) {
        std::cerr << "Wrong input: " << e.displayText() << std::endl;
    }
}

bool filter_parameter::applyFilter(const set& s) const {
    if (parsedFilter_) {
        return parsedFilter_->apply(s);
    }
    return false;  // No filter or parsing error
}

std::unique_ptr<filter> filter_parameter::parseFilter(const Poco::JSON::Object::Ptr& object) {
    if (!object->has("and") && !object->has("or") && !object->has("not")) {
        // If it's not a composite filter, treat it as a simple filter
        return parseSimpleFilter(object);
    }

    // It's a composite filter
    std::string logicalOperator;
    if (object->has("and")) {
        logicalOperator = "and";
    } else if (object->has("or")) {
        logicalOperator = "or";
    } else if (object->has("not")) {
        logicalOperator = "not";
    }

    return parseCompositeFilter(object, logicalOperator);
}

std::unique_ptr<filter> filter_parameter::parseCompositeFilter(const Poco::JSON::Object::Ptr& object, const std::string& logicalOperator) {
    try {
        if (object->has(logicalOperator)) {
            Poco::JSON::Array::Ptr filtersArray = object->getArray(logicalOperator);

            std::vector<std::unique_ptr<filter>> filters;

            for (size_t i = 0; i < filtersArray->size(); ++i) {
                Poco::JSON::Object::Ptr filterObject;
                try {
                    filterObject = filtersArray->getObject(i);
                } catch (const Poco::InvalidAccessException&) {
                    std::cerr << "Invalid filter object at index " << i << " in composite filter." << std::endl;
                    continue;
                }

                if (filterObject) {
                    std::unique_ptr<filter> parsedFilter = parseFilter(filterObject);
                    if (parsedFilter) {
                        filters.push_back(std::move(parsedFilter));
                    }
                } else {
                    std::cerr << "Empty filter object at index " << i << " in composite filter." << std::endl;
                }
            }

            // Create and return the composite filter
            if (logicalOperator == "and") {
                return std::make_unique<and_filter>(std::move(filters));
            } else if (logicalOperator == "or") {
                return std::make_unique<or_filter>(std::move(filters));
            } else if (logicalOperator == "not") {
                if (filters.size() == 1) {
                    return std::make_unique<not_filter>(std::move(filters[0]));
                } else {
                    std::cerr << "Invalid 'not' filter with more than one operand." << std::endl;
                }
            }
        } else {
            std::cerr << "No '" << logicalOperator << "' array found in composite filter." << std::endl;
        }
    } catch (const Poco::Exception& e) {
        std::cerr << "Error parsing composite filter: " << e.displayText() << std::endl;
    }

    return nullptr;
}

std::unique_ptr<filter> filter_parameter::parseSimpleFilter(const Poco::JSON::Object::Ptr& object) {
    std::string name = object->getValue<std::string>("name");
    std::string op = object->getValue<std::string>("op");
    Poco::Dynamic::Var val = object->get("val");

    // Create the appropriate simple filter based on the property
    if (name == "id") {
        return std::make_unique<id_filter>(parseOperator(op), parseValues<int64_t>(val));
    }
    // Add cases for other properties if needed

    return nullptr;
}

filter_parameter::id_filter::Operator filter_parameter::parseOperator(const std::string& op) {
    if (op == "eq") {
        return id_filter::Operator::Equals;
    } else if (op == "ne") {
        return id_filter::Operator::NotEquals;
    } else {
        std::cerr << "Unsupported operator: " << op << std::endl;
        return id_filter::Operator::Invalid;
    }
}

template <typename T>
std::vector<T> filter_parameter::parseValues(const Poco::Dynamic::Var& val) {
    std::vector<T> values;

    if (val.isVector()) {
        const Poco::JSON::Array::Ptr array = val.extract<Poco::JSON::Array::Ptr>();
        for (size_t i = 0; i < array->size(); ++i) {
            T value;
            try {
                value = array->get(i).convert<T>();
                values.push_back(value);
            } catch (const Poco::Exception& e) {
                std::cerr << "Error converting value at index " << i << ": " << e.displayText() << std::endl;
            }
        }
    } else {
        try {
            T value = val.convert<T>();
            values.push_back(value);
        } catch (const Poco::Exception& e) {
            std::cerr << "Error converting value: " << e.displayText() << std::endl;
        }
    }

    return values;
}

// Add other filter implementations as needed

int main() {
    std::string compositeFilterJson = R"({"or":[{"name":"id", "op":"eq","val":[13500, 13600]}, {"and":[{"name":"priority", "op":"eq","val":1},{"name":"priority", "op":"eq","val":0}]}]})";
    filter_parameter testComposite(compositeFilterJson);

    set exampleSet;
    // Initialize the set with some values for testing
    exampleSet.event_id = 13500;
    exampleSet.priority = 1;

    if (testComposite.applyFilter(exampleSet)) {
        std::cout << "Filter applied successfully!" << std::endl;
    } else {
        std::cout << "Filter did not match the set." << std::endl;
    }

    return 0;
}
