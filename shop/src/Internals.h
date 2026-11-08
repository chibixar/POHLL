#pragma once
#include "Component.h"

class InternalHardware : public Component
{
protected:
    int tdp_ = 65;

public:
    Department department() const override { return Department::Internal; }
    int tdp() const { return tdp_; }
    void set_tdp(int value);

    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
};

class Processor : public InternalHardware
{
    int clock_mhz_ = 3500;

public:
    int clock_mhz() const { return clock_mhz_; }
    void set_clock_mhz(int value);

    int category_id() const override;
    std::string category_name() const override { return "Processor"; }
    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
    std::unique_ptr<Component> clone() const override;
};

class RAM : public InternalHardware
{
    int capacity_gb_ = 16;

public:
    int capacity_gb() const { return capacity_gb_; }
    void set_capacity_gb(int value);

    int category_id() const override;
    std::string category_name() const override { return "RAM"; }
    std::vector<std::string> field_names() const override;
    void set_field(int field_no, const std::string& value) override;
    void serialize(std::ostream& os) const override;
    void deserialize(std::istream& is) override;
    void print_header(std::ostream& os) const override;
    void print_row(std::ostream& os) const override;
    std::unique_ptr<Component> clone() const override;
};
