#include "Component.h"
#include "constants.h"
#include "exceptions.h"
#include "validation.h"
#include <iomanip>
#include <istream>
#include <ostream>

void Component::set_id(int value)
{
    check_range(value, 1, 1'000'000, "ID");
    id_ = value;
}

void Component::set_article(const std::string& value)
{
    check_text(value, "Article / SKU", ARTICLE_LEN);
    article_ = value;
}

void Component::set_name(const std::string& value)
{
    check_text(value, "Product Name", NAME_LEN);
    name_ = value;
}

void Component::set_manufacturer(const std::string& value)
{
    check_text(value, "Manufacturer", NAME_LEN);
    manufacturer_ = value;
}

void Component::set_price(double value)
{
    check_range(value, MIN_PRICE, MAX_PRICE, "Unit Price");
    price_ = value;
}

void Component::set_quantity(int value)
{
    check_range(value, MIN_QUANTITY, MAX_QUANTITY, "Stock Quantity");
    quantity_ = value;
}

std::vector<std::string> Component::field_names() const
{
    return {"Price ($)", "Quantity", "Product Name", "Manufacturer"};
}

void Component::set_field(int field_no, const std::string& value)
{
    switch (field_no)
    {
    case 1: set_price(parse_double(value, "Price")); break;
    case 2: set_quantity(parse_int(value, "Quantity")); break;
    case 3: set_name(trim(value)); break;
    case 4: set_manufacturer(trim(value)); break;
    default: throw ValidationException("Invalid field index.");
    }
}

std::string read_token(std::istream& is, const std::string& field)
{
    std::string token;
    if (!std::getline(is, token, ';'))
        throw FileException("Missing record field: " + field);
    return trim(token);
}

void Component::serialize(std::ostream& os) const
{
    os << id_ << ';' << article_ << ';' << name_ << ';' << manufacturer_ << ';'
       << format_double(price_, 2) << ';' << quantity_;
}

void Component::deserialize(std::istream& is)
{
    set_id(parse_int(read_token(is, "ID"), "ID"));
    set_article(read_token(is, "Article"));
    set_name(read_token(is, "Product Name"));
    set_manufacturer(read_token(is, "Manufacturer"));
    set_price(parse_double(read_token(is, "Price"), "Price"));
    set_quantity(parse_int(read_token(is, "Quantity"), "Quantity"));
}

void Component::print_header(std::ostream& os) const
{
    os << std::left << std::setw(6)  << "ID" 
       << std::setw(12) << "SKU"
       << std::setw(30) << "Product Name" 
       << std::setw(18) << "Manufacturer"
       << std::right 
       << std::setw(10) << "Price ($)" 
       << std::setw(7)  << "Stock";
}

void Component::print_row(std::ostream& os) const
{
    os << std::left << std::setw(6)  << id_ 
       << std::setw(12) << article_
       << std::setw(30) << name_ 
       << std::setw(18) << manufacturer_
       << std::right 
       << std::setw(10) << format_double(price_, 2) 
       << std::setw(7)  << quantity_;
}
