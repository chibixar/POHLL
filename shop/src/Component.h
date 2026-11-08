#pragma once
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

enum class Department { Peripheral, Internal };

class Component
{
protected:
    int id_ = 0;
    std::string article_, name_, manufacturer_;
    double price_ = 0.0;
    int quantity_ = 0;

public:
    Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;
    virtual ~Component() = default;

    int id() const { return id_; }
    const std::string& article() const { return article_; }
    const std::string& name() const { return name_; }
    const std::string& manufacturer() const { return manufacturer_; }
    double price() const { return price_; }
    int quantity() const { return quantity_; }
    double total() const { return price_ * quantity_; }

    void set_id(int value);
    void set_article(const std::string& value);
    void set_name(const std::string& value);
    void set_manufacturer(const std::string& value);
    void set_price(double value);
    void set_quantity(int value);

    virtual int category_id() const = 0;
    virtual std::string category_name() const = 0;
    virtual Department department() const = 0;

    virtual std::vector<std::string> field_names() const;
    virtual void set_field(int field_no, const std::string& value);

    virtual void serialize(std::ostream& os) const;
    virtual void deserialize(std::istream& is);

    virtual void print_header(std::ostream& os) const;
    virtual void print_row(std::ostream& os) const;

    virtual std::unique_ptr<Component> clone() const = 0;

    bool operator==(const Component& other) const { return article_ == other.article_; }
};

std::string read_token(std::istream& is, const std::string& field);
