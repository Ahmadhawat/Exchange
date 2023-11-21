#include <iostream>

class ImageFilter {
public:
    ImageFilter(int value) : filterValue(value) {}

    // Pure virtual function to apply the filter
    virtual void applyFilter() const = 0;

    // Non-virtual function shared by all subclasses
    void commonFunction() const {
        std::cout << "This is a common function for all image filters. Filter value: " << filterValue << std::endl;
    }

    // Virtual function that modifies internal state
    virtual void adjustContrast() {
        std::cout << "Adjusting contrast in the base class." << std::endl;
        // Logic specific to adjusting contrast
        filterValue += 10;
    }

    // Virtual function that is not const
    virtual void modifyFilter() {
        std::cout << "Modifying the filter in the base class." << std::endl;
        // Logic specific to modifying the filter
        filterValue *= 2;
    }

    // Virtual destructor to ensure proper cleanup
    virtual ~ImageFilter() {
        // Implementation in the base class if needed
    }

private:
    int filterValue;
};

class GrayscaleFilter : public ImageFilter {
public:
    GrayscaleFilter(int value) : ImageFilter(value) {}

    // Implementing the pure virtual function
    void applyFilter() const override {
        std::cout << "Applying Grayscale Filter." << std::endl;
        // Logic specific to the grayscale filter
    }

    // Overriding the non-const function
    void modifyFilter() override {
        std::cout << "Modifying the filter in the GrayscaleFilter class." << std::endl;
        // Additional logic specific to modifying the filter in grayscale filter
        ImageFilter::modifyFilter(); // Calling the base class implementation
    }
};

class BlurFilter : public ImageFilter {
public:
    BlurFilter(int value) : ImageFilter(value) {}

    // Implementing the pure virtual function
    void applyFilter() const override {
        std::cout << "Applying Blur Filter." << std::endl;
        // Logic specific to the blur filter
    }

    // Overriding the non-const function
    void modifyFilter() override {
        std::cout << "Modifying the filter in the BlurFilter class." << std::endl;
        // Additional logic specific to modifying the filter in blur filter
        ImageFilter::modifyFilter(); // Calling the base class implementation
    }
};

int main() {
    // Example usage of the abstract class and its subclasses
    GrayscaleFilter grayscaleFilter(42);
    BlurFilter blurFilter(87);

    grayscaleFilter.applyFilter();
    grayscaleFilter.commonFunction();
    grayscaleFilter.adjustContrast();
    grayscaleFilter.modifyFilter(); // Modifying internal state

    blurFilter.applyFilter();
    blurFilter.commonFunction();
    blurFilter.adjustContrast();
    blurFilter.modifyFilter(); // Modifying internal state

    return 0;
}
