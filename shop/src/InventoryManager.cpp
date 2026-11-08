#include "InventoryManager.h"
#include "Algorithms.h"
#include "ComponentFactory.h"
#include "Logger.h"
#include "constants.h"
#include "exceptions.h"
#include "validation.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

namespace
{
std::vector<std::string> split(const std::string& line, char sep)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream ss(line);
    while (std::getline(ss, part, sep))
        parts.push_back(trim(part));
    return parts;
}

std::string lower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}
}

InventoryManager::InventoryManager(std::string data_dir) : dir_(std::move(data_dir)) {}

std::string InventoryManager::path(const std::string& file) const
{
    return dir_.empty() ? file : dir_ + "/" + file;
}

void InventoryManager::rebuild_tree()
{
    sku_tree_.clear();
    for (size_t i = 0; i < items_.size(); ++i)
    {
        sku_tree_.add(SkuRecord{items_[i]->article(), items_[i].get()});
    }
}

bool InventoryManager::has_article(const std::string& article) const
{
    return sku_tree_.find(SkuRecord{article, nullptr}) != nullptr;
}

size_t InventoryManager::index_of(const std::string& article) const
{
    for (size_t i = 0; i < items_.size(); ++i)
        if (items_[i]->article() == article)
            return i;
    throw NotFoundException("Item with SKU [" + article + "] was not found.");
}

void InventoryManager::push_done(std::unique_ptr<Command> cmd)
{
    Logger::log(cmd->description());
    history_.push(std::move(cmd));
    modified_ = true;
    rebuild_tree();
}

size_t InventoryManager::load_all()
{
    std::ifstream cat(path("categories.txt"));
    if (!cat)
        throw FileException("Failed to open category manifest: " + path("categories.txt"));

    items_ = Items();
    categories_.clear();
    sku_tree_.clear();
    history_ = std::stack<std::unique_ptr<Command>>();
    size_t bad = 0;

    std::string line;
    while (std::getline(cat, line))
    {
        if (trim(line).empty())
            continue;
        auto parts = split(line, ';');
        try
        {
            if (parts.size() < 3)
                throw FileException("Malformed line in categories.txt");
            CategoryInfo info;
            info.id = parse_int(parts[0], "Category ID");
            info.name = parts[1];
            info.file = parts[2];
            ComponentFactory::create(info.id);
            categories_[info.id] = info;
        }
        catch (const InventoryException& e)
        {
            ++bad;
            std::cerr << "categories.txt error: " << e.what() << '\n';
        }
    }

    for (const auto& [id, info] : categories_)
    {
        std::ifstream in(path(info.file));
        if (!in)
        {
            Logger::log("Data file not found for: " + info.file);
            continue;
        }

        int line_no = 0;
        while (std::getline(in, line))
        {
            ++line_no;
            if (trim(line).empty())
                continue;
            try
            {
                auto item = ComponentFactory::create(id);
                std::istringstream ss(line);
                item->deserialize(ss);
                if (ss.peek() != std::char_traits<char>::eof())
                    throw FileException("Extraneous tokens at end of record");
                if (has_article(item->article()))
                    throw ValidationException("Duplicate SKU: " + item->article());

                sku_tree_.add(SkuRecord{item->article(), item.get()});
                items_.push_back(std::move(item));
            }
            catch (const InventoryException& e)
            {
                ++bad;
                std::cerr << info.file << ':' << line_no << ": " << e.what() << '\n';
            }
        }
    }

    rebuild_tree();
    modified_ = false;
    Logger::log("Catalog database loaded successfully.");
    return bad;
}

void InventoryManager::save_all()
{
    for (const auto& [id, info] : categories_)
    {
        std::ofstream out(path(info.file), std::ios::trunc);
        if (!out)
            throw FileException("Unable to write file: " + path(info.file));
        for (const auto& item : items_)
        {
            if (item->category_id() != id)
                continue;
            item->serialize(out);
            out << '\n';
        }
    }
    modified_ = false;
    Logger::log("Database successfully persisted to storage.");
}

int InventoryManager::next_id(int category_id) const
{
    int max_id = category_id * 100;
    for (const auto& item : items_)
        if (item->category_id() == category_id)
            max_id = std::max(max_id, item->id());
    return max_id + 1;
}

void InventoryManager::add(std::unique_ptr<Component> item)
{
    if (!item)
        throw ValidationException("Null item pointer provided.");
    if (!categories_.count(item->category_id()))
        throw NotFoundException("Category has not been loaded.");
    if (has_article(item->article()))
        throw ValidationException("SKU [" + item->article() + "] already exists.");

    auto cmd = std::make_unique<AddCommand>(items_, std::move(item));
    cmd->execute();
    push_done(std::move(cmd));
}

void InventoryManager::remove(const std::string& article)
{
    auto cmd = std::make_unique<RemoveCommand>(items_, index_of(article));
    cmd->execute();
    push_done(std::move(cmd));
}

void InventoryManager::edit(const std::string& article, int field_no, const std::string& value)
{
    auto cmd = std::make_unique<EditCommand>(items_, index_of(article), field_no, value);
    cmd->execute();
    push_done(std::move(cmd));
}

void InventoryManager::undo()
{
    if (history_.empty())
        throw InventoryException("Action history is empty. Nothing to undo.");
    auto cmd = std::move(history_.top());
    history_.pop();
    cmd->undo();
    rebuild_tree();
    modified_ = true;
    Logger::log("Reverted: " + cmd->description());
}

const Component& InventoryManager::find_by_article(const std::string& article) const
{
    const Component* found = find_by_article_bst(article);
    if (!found)
        throw NotFoundException("Product with SKU [" + article + "] was not found.");
    return *found;
}

const Component* InventoryManager::find_by_article_bst(const std::string& article) const
{
    const SkuRecord* rec = sku_tree_.find(SkuRecord{article, nullptr});
    return rec ? rec->component : nullptr;
}

void InventoryManager::display_sku_tree(std::ostream& os) const
{
    os << UI::CYAN << "--- Visual Binary Search Tree (SKU Index) ---\n" << UI::RESET;
    sku_tree_.print(os);
    os << UI::GRAY << "Tree Node Count: " << sku_tree_.size() << UI::RESET << "\n";
}

std::vector<const Component*> InventoryManager::all() const
{
    std::vector<const Component*> result;
    for (const auto& item : items_)
        result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::filter_by_category(int category_id) const
{
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (item->category_id() == category_id)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::filter_by_department(Department dept) const
{
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (item->department() == dept)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::filter_low_stock(int threshold) const
{
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (item->quantity() <= threshold)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::search_by_manufacturer(const std::string& text) const
{
    std::string needle = lower(text);
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (lower(item->manufacturer()).find(needle) != std::string::npos)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::search_by_name(const std::string& text) const
{
    std::string needle = lower(text);
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (lower(item->name()).find(needle) != std::string::npos)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::search_by_price(double lo, double hi) const
{
    if (lo > hi)
        throw ValidationException("Lower price bound cannot exceed upper bound.");
    std::vector<const Component*> result;
    for (const auto& item : items_)
        if (item->price() >= lo && item->price() <= hi)
            result.push_back(item.get());
    return result;
}

std::vector<const Component*> InventoryManager::sorted(SortKey key, bool own_algorithm) const
{
    auto view = all();
    auto less = [key](const Component* a, const Component* b)
    {
        switch (key)
        {
        case SortKey::Price:      return a->price() < b->price();
        case SortKey::Name:       return a->name() < b->name();
        case SortKey::TotalValue: return a->total() < b->total();
        case SortKey::Quantity:
        default:                  return a->quantity() < b->quantity();
        }
    };

    if (own_algorithm)
        sort_own(view.begin(), view.end(), less);
    else
        std::sort(view.begin(), view.end(), less);
    return view;
}

void InventoryManager::write_report(const std::string& file) const
{
    std::ofstream out(path(file), std::ios::trunc);
    if (!out)
        throw FileException("Unable to generate report: " + path(file));

    out << "==========================================================================================\n";
    out << "                             HARDWARE INVENTORY AUDIT REPORT                              \n";
    out << "==========================================================================================\n";
    out << std::left << std::setw(5)  << "No"
        << std::setw(12) << "SKU"
        << std::setw(30) << "Product Name"
        << std::setw(18) << "Manufacturer"
        << std::right 
        << std::setw(8)  << "Qty" 
        << std::setw(14) << "Total Val ($)" << '\n';
    out << "------------------------------------------------------------------------------------------\n";

    int n = 1;
    for (const auto& item : items_)
    {
        out << std::left << std::setw(5)  << n++
            << std::setw(12) << item->article()
            << std::setw(30) << item->name()
            << std::setw(18) << item->manufacturer()
            << std::right 
            << std::setw(8)  << item->quantity()
            << std::setw(14) << format_double(item->total(), 2) << '\n';
    }

    double sum = std::accumulate(items_.data(), items_.data() + items_.size(), 0.0,
                                 [](double s, const std::unique_ptr<Component>& p) { return s + p->total(); });

    out << "==========================================================================================\n";
    out << "Total Inventory Units: " << items_.size() << " items | Gross Valuation: $" << format_double(sum, 2) << '\n';

    Logger::log("Exported formal inventory report: " + file);
}

void print_table(std::ostream& os, const std::vector<const Component*>& list)
{
    if (list.empty())
    {
        os << UI::YELLOW << "No matching records found.\n" << UI::RESET;
        return;
    }

    std::map<int, std::vector<const Component*>> groups;
    for (const Component* c : list)
        groups[c->category_id()].push_back(c);

    for (const auto& [id, group] : groups)
    {
        os << UI::CYAN << UI::BOLD << "\n=== " << group.front()->category_name()
           << " Section (" << group.size() << " items) ===\n" << UI::RESET;
        os << UI::BLUE;
        group.front()->print_header(os);
        os << UI::RESET << '\n';
        os << std::string(85, '-') << '\n';
        for (const Component* c : group)
        {
            if (c->quantity() <= LOW_STOCK_THRESHOLD)
                os << UI::RED;
            c->print_row(os);
            os << UI::RESET << '\n';
        }
    }
}

void print_card(std::ostream& os, const Component& item)
{
    os << UI::CYAN << "┌──────────────────────────────────────────────┐\n";
    os << "│ Product Details: " << std::left << std::setw(28) << item.name() << "│\n";
    os << "├──────────────────────────────────────────────┤\n" << UI::RESET;
    os << " SKU:          " << item.article() << "\n";
    os << " Category:     " << item.category_name() << "\n";
    os << " Manufacturer: " << item.manufacturer() << "\n";
    os << " Unit Price:   $" << format_double(item.price(), 2) << "\n";
    os << " Stock Level:  " << item.quantity();
    if (item.quantity() <= LOW_STOCK_THRESHOLD)
        os << UI::RED << " [LOW STOCK!]" << UI::RESET;
    os << "\n Total Value:  $" << format_double(item.total(), 2) << "\n";
    os << UI::CYAN << "└──────────────────────────────────────────────┘\n" << UI::RESET;
}
