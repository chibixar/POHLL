#pragma once
#include "Component.h"

class PeripheralDevice : public Component
{
protected:
    std::string connection_ = "USB";

public:
    Department department() const override { return Department::Peripheral; }
    const std::string& connection() const { return connection_; }
    void set_connection(const std::string& value);

    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
};

class Mouse : public PeripheralDevice
{
    int dpi_ = 800;

public:
    int dpi() const { return dpi_; }
    void set_dpi(int value);

    int category_id() const override;
    std::string category_name() const override { return "Mouse"; }
    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
    std::unique_ptr<Component> clone() const override;
};

class Keyboard : public PeripheralDevice
{
    std::string switch_type_ = "Mechanical";

public:
    const std::string& switch_type() const { return switch_type_; }
    void set_switch_type(const std::string& value);

    int category_id() const override;
    std::string category_name() const override { return "Keyboard"; }
    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
    std::unique_ptr<Component> clone() const override;
};

class Monitor : public PeripheralDevice
{
    int refresh_rate_ = 60;

public:
    int refresh_rate() const { return refresh_rate_; }
    void set_refresh_rate(int value);

    int category_id() const override;
    std::string category_name() const override { return "Monitor"; }
    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
    std::unique_ptr<Component> clone() const override;
};
