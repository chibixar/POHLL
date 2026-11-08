#include "Peripherals.h"
#include "constants.h"
#include "exceptions.h"
#include "validation.h"
#include <algorithm>
#include <array>
#include <iomanip>
#include <istream>
#include <ostream>

void PeripheralDevice::set_connection(const std::string& value)
{
    static const std::array<std::string, 4> allowed = {"USB", "Wireless", "HDMI", "DisplayPort"};
    if (std::find(allowed.begin(), allowed.end(), value) == allowed.end())
        throw ValidationException("Connection must be: USB, Wireless, HDMI, or DisplayPort.");
    connection_ = value;
}

std::vector<std::string> PeripheralDevice::field_names() const
{
    auto names = Component::field_names();
    names.push_back("Interface Connection");
    return names;
}

void PeripheralDevice::set_field(int field_no, const std::string& value)
{
    if (field_no == 5)
        set_connection(trim(value));
    else
        Component::set_field(field_no, value);
}

void PeripheralDevice::serialize(std::ostream& os) const
{
    Component::serialize(os);
    os << ';' << connection_;
}

void PeripheralDevice::deserialize(std::istream& is)
{
    Component::deserialize(is);
    set_connection(read_token(is, "Connection"));
}

void PeripheralDevice::print_header(std::ostream& os) const
{
    Component::print_header(os);
    os << "  " << std::left << std::setw(14) << "Interface";
}

void PeripheralDevice::print_row(std::ostream& os) const
{
    Component::print_row(os);
    os << "  " << std::left << std::setw(14) << connection_;
}

void Mouse::set_dpi(int value)
{
    check_range(value, MIN_DPI, MAX_DPI, "Sensor DPI");
    dpi_ = value;
}

int Mouse::category_id() const { return CAT_MOUSE; }

std::vector<std::string> Mouse::field_names() const
{
    auto names = PeripheralDevice::field_names();
    names.push_back("Max DPI");
    return names;
}

void Mouse::set_field(int field_no, const std::string& value)
{
    if (field_no == 6)
        set_dpi(parse_int(value, "DPI"));
    else
        PeripheralDevice::set_field(field_no, value);
}

void Mouse::serialize(std::ostream& os) const
{
    PeripheralDevice::serialize(os);
    os << ';' << dpi_;
}

void Mouse::deserialize(std::istream& is)
{
    PeripheralDevice::deserialize(is);
    set_dpi(parse_int(read_token(is, "DPI"), "DPI"));
}

void Mouse::print_header(std::ostream& os) const
{
    PeripheralDevice::print_header(os);
    os << "  " << std::left << std::setw(8) << "DPI";
}

void Mouse::print_row(std::ostream& os) const
{
    PeripheralDevice::print_row(os);
    os << "  " << std::left << std::setw(8) << dpi_;
}

std::unique_ptr<Component> Mouse::clone() const { return std::make_unique<Mouse>(*this); }

void Keyboard::set_switch_type(const std::string& value)
{
    check_text(value, "Switch Type", NAME_LEN);
    switch_type_ = value;
}

int Keyboard::category_id() const { return CAT_KEYBOARD; }

std::vector<std::string> Keyboard::field_names() const
{
    auto names = PeripheralDevice::field_names();
    names.push_back("Switch Mechanism");
    return names;
}

void Keyboard::set_field(int field_no, const std::string& value)
{
    if (field_no == 6)
        set_switch_type(trim(value));
    else
        PeripheralDevice::set_field(field_no, value);
}

void Keyboard::serialize(std::ostream& os) const
{
    PeripheralDevice::serialize(os);
    os << ';' << switch_type_;
}

void Keyboard::deserialize(std::istream& is)
{
    PeripheralDevice::deserialize(is);
    set_switch_type(read_token(is, "Switch Type"));
}

void Keyboard::print_header(std::ostream& os) const
{
    PeripheralDevice::print_header(os);
    os << "  " << std::left << std::setw(16) << "Switches";
}

void Keyboard::print_row(std::ostream& os) const
{
    PeripheralDevice::print_row(os);
    os << "  " << std::left << std::setw(16) << switch_type_;
}

std::unique_ptr<Component> Keyboard::clone() const { return std::make_unique<Keyboard>(*this); }

void Monitor::set_refresh_rate(int value)
{
    check_range(value, MIN_REFRESH, MAX_REFRESH, "Refresh Rate");
    refresh_rate_ = value;
}

int Monitor::category_id() const { return CAT_MONITOR; }

std::vector<std::string> Monitor::field_names() const
{
    auto names = PeripheralDevice::field_names();
    names.push_back("Refresh Rate (Hz)");
    return names;
}

void Monitor::set_field(int field_no, const std::string& value)
{
    if (field_no == 6)
        set_refresh_rate(parse_int(value, "Refresh Rate"));
    else
        PeripheralDevice::set_field(field_no, value);
}

void Monitor::serialize(std::ostream& os) const
{
    PeripheralDevice::serialize(os);
    os << ';' << refresh_rate_;
}

void Monitor::deserialize(std::istream& is)
{
    PeripheralDevice::deserialize(is);
    set_refresh_rate(parse_int(read_token(is, "Refresh Rate"), "Refresh Rate"));
}

void Monitor::print_header(std::ostream& os) const
{
    PeripheralDevice::print_header(os);
    os << "  " << std::left << std::setw(8) << "Refresh";
}

void Monitor::print_row(std::ostream& os) const
{
    PeripheralDevice::print_row(os);
    os << "  " << std::left << std::setw(8) << (std::to_string(refresh_rate_) + "Hz");
}

std::unique_ptr<Component> Monitor::clone() const { return std::make_unique<Monitor>(*this); }
