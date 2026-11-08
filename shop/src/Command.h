#pragma once
#include "Component.h"
#include "VectorContainer.h"
#include <memory>
#include <string>

using Items = VectorContainer<std::unique_ptr<Component>>;

class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;
};

class AddCommand : public Command
{
    Items& items_;
    std::unique_ptr<Component> item_;
    std::string article_;

public:
    AddCommand(Items& items, std::unique_ptr<Component> item)
        : items_(items), item_(std::move(item)), article_(item_->article()) {}

    void execute() override { items_.push_back(std::move(item_)); }
    void undo() override    { item_ = items_.erase(items_.size() - 1); }
    std::string description() const override { return "Add Item [" + article_ + "]"; }
};

class RemoveCommand : public Command
{
    Items& items_;
    size_t index_;
    std::string article_;
    std::unique_ptr<Component> removed_;

public:
    RemoveCommand(Items& items, size_t index)
        : items_(items), index_(index), article_(items[index]->article()) {}

    void execute() override { removed_ = items_.erase(index_); }
    void undo() override    { items_.insert(index_, std::move(removed_)); }
    std::string description() const override { return "Remove Item [" + article_ + "]"; }
};

class EditCommand : public Command
{
    Items& items_;
    size_t index_;
    int field_;
    std::string value_;
    std::string article_;
    std::unique_ptr<Component> old_;

public:
    EditCommand(Items& items, size_t index, int field, std::string value)
        : items_(items), index_(index), field_(field), value_(std::move(value)),
          article_(items[index]->article()) {}

    void execute() override
    {
        auto backup = items_[index_]->clone();
        items_[index_]->set_field(field_, value_);
        old_ = std::move(backup);
    }
    void undo() override { items_[index_] = std::move(old_); }
    std::string description() const override { return "Edit Item [" + article_ + "]"; }
};
