// ... (previous code)

namespace filters {

class FilterUtils {
public:
    static std::string propertyToString(Property prop) {
        switch (prop) {
            case Property::EventId: return "EventId";
            case Property::Context: return "Context";
            // Add other cases as needed
            default: return "Invalid";
        }
    }

    static std::string operatorToString(Operator op) {
        switch (op) {
            case Operator::Equals: return "Equals";
            // Add other cases as needed
            default: return "Invalid";
        }
    }
};

enum class Property { /* ... */ };
enum class Operator { /* ... */ };
enum class Serverity : uint8_t { /* ... */ };

// ... (previous code)

template <typename T, Property property>
class SimpleFilter : public Filter {
public:
    SimpleFilter(Operator op, const T& val) : operator_(op), values_{val} {}

    void AddToSet(Set& set) const override {
        switch (operator_) {
            case Operator::Equals:
                SetValue(set);
                break;
            // Handle other cases as needed
            default:
                // Handle other operators
                break;
        }
    }

private:
    void SetValue(Set& set) const {
        switch (property) {
            case Property::EventId: set.event_id = values_[0]; break;
            case Property::Context: set.context = values_[0]; break;
            // Add other cases as needed
            default: // Handle other properties
                break;
        }
    }

    Operator operator_;
    std::vector<T> values_;
};

}  // namespace filters

int main() {
    using namespace filters;

    SimpleFilter<int, Property::EventId> eventIdFilter(Operator::Equals, 42);

    Set mySet;
    eventIdFilter.AddToSet(mySet);

    std::cout << FilterUtils::propertyToString(Property::EventId) << " in set: " << mySet.event_id << std::endl;

    return 0;
}
