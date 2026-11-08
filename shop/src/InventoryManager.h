#pragma once
#include "Command.h"
#include "Component.h"
#include "tree.h"
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

enum class SortKey { Price = 1, Name = 2, Quantity = 3, TotalValue = 4 };

struct SkuRecord
{
    std::string article;
    const Component* component = nullptr;

    bool operator<(const SkuRecord& o) const { return article < o.article; }
    bool operator==(const SkuRecord& o) const { return article == o.article; }
};

inline std::ostream& operator<<(std::ostream& os, const SkuRecord& r)
{
    os << "[" << r.article << "]";
    return os;
}

struct CategoryInfo
{
    int id = 0;
    std::string name;
    std::string file;
};

class InventoryManager
{
    std::string dir_;
    Items items_;
    std::map<int, CategoryInfo> categories_;
    std::stack<std::unique_ptr<Command>> history_;
    bool modified_ = false;

    BinaryTree<SkuRecord> sku_tree_;

    std::string path(const std::string& file) const;
    bool has_article(const std::string& article) const;
    size_t index_of(const std::string& article) const;
    void push_done(std::unique_ptr<Command> cmd);
    void rebuild_tree();

public:
    explicit InventoryManager(std::string data_dir);

    size_t load_all();
    void save_all();

    int next_id(int category_id) const;
    const std::map<int, CategoryInfo>& categories() const { return categories_; }

    void add(std::unique_ptr<Component> item);
    void remove(const std::string& article);
    void edit(const std::string& article, int field_no, const std::string& value);
    void undo();

    const Component& find_by_article(const std::string& article) const;
    const Component* find_by_article_bst(const std::string& article) const;
    void display_sku_tree(std::ostream& os) const;

    std::vector<const Component*> all() const;
    std::vector<const Component*> filter_by_category(int category_id) const;
    std::vector<const Component*> filter_by_department(Department dept) const;
    std::vector<const Component*> filter_low_stock(int threshold) const;
    std::vector<const Component*> search_by_manufacturer(const std::string& text) const;
    std::vector<const Component*> search_by_name(const std::string& text) const;
    std::vector<const Component*> search_by_price(double lo, double hi) const;
    std::vector<const Component*> sorted(SortKey key, bool own_algorithm) const;

    void write_report(const std::string& file) const;

    size_t size() const { return items_.size(); }
    bool can_undo() const { return !history_.empty(); }
    bool modified() const { return modified_; }
};

void print_table(std::ostream& os, const std::vector<const Component*>& list);
void print_card(std::ostream& os, const Component& item);
