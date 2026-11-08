#include "Internals.h"
#include "constants.h"
#include "exceptions.h"
#include "validation.h"
#include <iomanip>
#include <istream>
#include <ostream>

void InternalHardware::set_tdp(int value)
{
    check_range(value, MIN_TDP, MAX_TDP, "TDP");
    tdp_ = value;
}

std::vector<std::string> InternalHardware::field_names() const
{
    auto names = Component::field_names();
    names.push_back("Power TDP (Watts)");
    return names;
}

void InternalHardware::set_field(int field_no, const std::string& value)
{
    if (field_no == 5)
        set_tdp(parse_int(value, "TDP"));
    else
        Component::set_field(field_no, value);
}

void InternalHardware::serialize(std::ostream& os) const
{
    Component::serialize(os);
    os << ';' << tdp_;
}

void InternalHardware::deserialize(std::istream& is)
{
    Component::deserialize(is);
    set_tdp(parse_int(read_token(is, "TDP"), "TDP"));
}

void InternalHardware::print_header(std::ostream& os) const
{
    Component::print_header(os);
    os << "  " << std::left << std::setw(8) << "TDP (W)";
}

void InternalHardware::print_row(std::ostream& os) const
{
    Component::print_row(os);
    os << "  " << std::left << std::setw(8) << (std::to_string(tdp_) + "W");
}

void Processor::set_clock_mhz(int value)
{
    check_range(value, MIN_CLOCK, MAX_CLOCK, "Base Clock");
    clock_mhz_ = value;
}

int Processor::category_id() const { return CAT_PROCESSOR; }

std::vector<std::string> Processor::field_names() const
{
    auto names = InternalHardware::field_names();
    names.push_back("Clock Speed (MHz)");
    return names;
}

void Processor::set_field(int field_no, const std::string& value)
{
    if (field_no == 6)
        set_clock_mhz(parse_int(value, "Clock Speed"));
    else
        InternalHardware::set_field(field_no, value);
}

void Processor::serialize(std::ostream& os) const
{
    InternalHardware::serialize(os);
    os << ';' << clock_mhz_;
}

void Processor::deserialize(std::istream& is)
{
    InternalHardware::deserialize(is);
    set_clock_mhz(parse_int(read_token(is, "Clock Speed"), "Clock Speed"));
}

void Processor::print_header(std::ostream& os) const
{
    InternalHardware::print_header(os);
    os << "  " << std::left << std::setw(10) << "Clock (MHz)";
}

void Processor::print_row(std::ostream& os) const
{
    InternalHardware::print_row(os);
    os << "  " << std::left << std::setw(10) << (std::to_string(clock_mhz_) + " MHz");
}

std::unique_ptr<Component> Processor::clone() const { return std::make_unique<Processor>(*this); }

void RAM::set_capacity_gb(int value)
{
    check_range(value, MIN_CAPACITY, MAX_CAPACITY, "Module Capacity");
    capacity_gb_ = value;
}

int RAM::category_id() const { return CAT_RAM; }

std::vector<std::string> RAM::field_names() const
{
    auto names = InternalHardware::field_names();
    names.push_back("Capacity (GB)");
    return names;
}

void RAM::set_field(int field_no, const std::string& value)
{
    if (field_no == 6)
        set_capacity_gb(parse_int(value, "Capacity"));
    else
        InternalHardware::set_field(field_no, value);
}

void RAM::serialize(std::ostream& os) const
{
    InternalHardware::serialize(os);
    os << ';' << capacity_gb_;
}

void RAM::deserialize(std::istream& is)
{
    InternalHardware::deserialize(is);
    set_capacity_gb(parse_int(read_token(is, "Capacity"), "Capacity"));
}

void RAM::print_header(std::ostream& os) const
{
    InternalHardware::print_header(os);
    os << "  " << std::left << std::setw(10) << "Size (GB)";
}

void RAM::print_row(std::ostream& os) const
{
    InternalHardware::print_row(os);
    os << "  " << std::left << std::setw(10) << (std::to_string(capacity_gb_) + " GB");
}

std::unique_ptr<Component> RAM::clone() const { return std::make_unique<RAM>(*this); }
