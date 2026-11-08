#include "ComponentFactory.h"
#include "InventoryManager.h"
#include "Logger.h"
#include "constants.h"
#include "exceptions.h"
#include "validation.h"
#include <iostream>
#include <string>

namespace
{
struct EndOfInput {};
enum class Role { Admin = 1, User = 2 };

std::string read_line(const std::string& prompt)
{
    std::cout << UI::WHITE << prompt << UI::RESET;
    std::string s;
    if (!std::getline(std::cin, s))
        throw EndOfInput{};
    return trim(s);
}

int read_int(const std::string& prompt, int lo, int hi)
{
    while (true)
    {
        try
        {
            int v = parse_int(read_line(prompt), "Input");
            check_range(v, lo, hi, "Input");
            return v;
        }
        catch (const ValidationException& e)
        {
            std::cout << UI::RED << "Error: " << e.what() << UI::RESET << '\n';
        }
    }
}

double read_double(const std::string& prompt)
{
    while (true)
    {
        try { return parse_double(read_line(prompt), "Input"); }
        catch (const ValidationException& e) { std::cout << UI::RED << "Error: " << e.what() << UI::RESET << '\n'; }
    }
}

int choose_category(const InventoryManager& mgr)
{
    std::cout << UI::CYAN << "\nAvailable Categories:\n" << UI::RESET;
    for (const auto& [id, info] : mgr.categories())
        std::cout << "  [" << id << "] " << info.name << '\n';
    int id = read_int("Select category ID (0 to cancel): ", 0, 99);
    if (id != 0 && !mgr.categories().count(id))
    {
        std::cout << UI::RED << "Category does not exist.\n" << UI::RESET;
        return 0;
    }
    return id;
}

void show_filter_menu(const InventoryManager& mgr)
{
    std::cout << UI::CYAN << "\n--- View & Filtering Options ---\n" << UI::RESET;
    std::cout << "1. Display All Items\n"
              << "2. Filter by Category (Mouse, RAM, etc.)\n"
              << "3. Filter by Department (Peripherals vs Internals)\n"
              << "4. View Low Stock Items (<= " << LOW_STOCK_THRESHOLD << " units)\n";
    int choice = read_int("Select view filter (0 to cancel): ", 0, 4);
    switch (choice)
    {
    case 1:
        print_table(std::cout, mgr.all());
        break;
    case 2:
    {
        int cat = choose_category(mgr);
        if (cat != 0)
            print_table(std::cout, mgr.filter_by_category(cat));
        break;
    }
    case 3:
    {
        std::cout << "1. Peripherals (Mouse, Keyboard, Monitor)\n2. Internal Hardware (CPU, RAM)\n";
        int dept = read_int("Choose department: ", 1, 2);
        print_table(std::cout, mgr.filter_by_department(dept == 1 ? Department::Peripheral : Department::Internal));
        break;
    }
    case 4:
        std::cout << UI::YELLOW << "\nItems needing restock:\n" << UI::RESET;
        print_table(std::cout, mgr.filter_low_stock(LOW_STOCK_THRESHOLD));
        break;
    default:
        break;
    }
}

void search(const InventoryManager& mgr)
{
    std::cout << UI::CYAN << "\n--- Search Catalog ---\n" << UI::RESET;
    std::cout << "1. Fast BST Lookup by SKU / Article\n"
              << "2. Search by Manufacturer\n"
              << "3. Search by Name (Substring)\n"
              << "4. Search by Price Range\n";
    int kind = read_int("Select search criteria (0 to cancel): ", 0, 4);
    switch (kind)
    {
    case 1:
    {
        std::string sku = read_line("Enter SKU: ");
        const Component* found = mgr.find_by_article_bst(sku);
        if (found)
            print_card(std::cout, *found);
        else
            std::cout << UI::RED << "No product found with SKU: " << sku << UI::RESET << '\n';
        break;
    }
    case 2:
        print_table(std::cout, mgr.search_by_manufacturer(read_line("Manufacturer: ")));
        break;
    case 3:
        print_table(std::cout, mgr.search_by_name(read_line("Name keyword: ")));
        break;
    case 4:
    {
        double lo = read_double("Min Price ($): ");
        double hi = read_double("Max Price ($): ");
        print_table(std::cout, mgr.search_by_price(lo, hi));
        break;
    }
    default: break;
    }
}

void sort_menu(const InventoryManager& mgr)
{
    std::cout << UI::CYAN << "\n--- Sort Catalog ---\n" << UI::RESET;
    std::cout << "1. Sort by Price\n2. Sort by Name\n3. Sort by Quantity\n4. Sort by Total Value\n";
    int key = read_int("Select sort key (0 to cancel): ", 0, 4);
    if (key == 0) return;

    std::cout << "1. Custom Selection Sort (Algorithm requirement)\n2. Standard std::sort\n";
    int method = read_int("Choose algorithm: ", 1, 2);
    print_table(std::cout, mgr.sorted(static_cast<SortKey>(key), method == 1));
}

void add_item(InventoryManager& mgr)
{
    int cat = choose_category(mgr);
    if (cat == 0) return;

    auto item = ComponentFactory::create(cat);
    item->set_id(mgr.next_id(cat));
    item->set_article(read_line("Enter Unique SKU: "));

    auto names = item->field_names();
    for (size_t i = 0; i < names.size(); ++i)
    {
        while (true)
        {
            try
            {
                item->set_field(static_cast<int>(i) + 1, read_line(names[i] + ": "));
                break;
            }
            catch (const ValidationException& e)
            {
                std::cout << UI::RED << e.what() << UI::RESET << '\n';
            }
        }
    }

    mgr.add(std::move(item));
    std::cout << UI::GREEN << "Item added successfully.\n" << UI::RESET;
}

void edit_item(InventoryManager& mgr)
{
    std::string article = read_line("Enter SKU to edit: ");
    const Component& item = mgr.find_by_article(article);
    print_card(std::cout, item);

    auto names = item.field_names();
    for (size_t i = 0; i < names.size(); ++i)
        std::cout << "  [" << i + 1 << "] " << names[i] << '\n';
    int field = read_int("Select field to modify (0 to cancel): ", 0, static_cast<int>(names.size()));
    if (field == 0) return;

    mgr.edit(article, field, read_line("Enter new value: "));
    std::cout << UI::GREEN << "Item updated successfully.\n" << UI::RESET;
}

void print_banner()
{
    std::cout << UI::CYAN << UI::BOLD;
    std::cout << "=====================================================\n";
    std::cout << "     PC HARDWARE INVENTORY & WAREHOUSE SYSTEM        \n";
    std::cout << "=====================================================\n";
    std::cout << UI::RESET;
}

void print_menu(Role role)
{
    std::cout << "\n" << UI::MAGENTA << UI::BOLD << "MAIN MENU" << UI::RESET << "\n";
    std::cout << " 1. Catalog & View Filters (Categories / Depts / Stock)\n";
    std::cout << " 2. Search Items (SKU, Name, Price, Brand)\n";
    std::cout << " 3. Sort Catalog (Custom vs STL)\n";
    std::cout << " 4. Display Article Binary Search Tree (Data Structure)\n";
    std::cout << " 5. Export Inventory Audit Report\n";

    if (role == Role::Admin)
    {
        std::cout << UI::YELLOW;
        std::cout << " 6. Add New Product\n";
        std::cout << " 7. Edit Product Details\n";
        std::cout << " 8. Remove Product\n";
        std::cout << " 9. Undo Last Action\n";
        std::cout << "10. Save Changes to Disk\n";
        std::cout << UI::RESET;
    }
    std::cout << " 0. Exit System\n";
}
}

int main(int argc, char* argv[])
{
    std::string dir = argc > 1 ? argv[1] : "data";
    Logger::set_file(dir + "/logs.txt");
    InventoryManager mgr(dir);

    print_banner();

    try
    {
        size_t bad = mgr.load_all();
        if (bad > 0)
            std::cout << UI::YELLOW << "Warning: " << bad << " invalid records skipped during load.\n" << UI::RESET;
        else
            std::cout << UI::GREEN << "Database loaded successfully (" << mgr.size() << " records).\n" << UI::RESET;

        std::cout << "\nSelect Authentication Role:\n";
        std::cout << "  [1] Administrator (Full Management)\n";
        std::cout << "  [2] General User (Read-only)\n";
        Role role = static_cast<Role>(read_int("Role: ", 1, 2));
        Logger::set_role(role == Role::Admin ? "Administrator" : "StandardUser");

        while (true)
        {
            print_menu(role);
            int max_choice = (role == Role::Admin) ? 10 : 5;
            int action = read_int("\nSelect Action: ", MENU_EXIT, max_choice);
            if (action == MENU_EXIT)
                break;

            try
            {
                switch (action)
                {
                case 1: show_filter_menu(mgr); break;
                case 2: search(mgr); break;
                case 3: sort_menu(mgr); break;
                case 4: mgr.display_sku_tree(std::cout); break;
                case 5: mgr.write_report("inventory_report.txt");
                        std::cout << UI::GREEN << "Report saved to inventory_report.txt\n" << UI::RESET;
                        break;
                case 6: add_item(mgr); break;
                case 7: edit_item(mgr); break;
                case 8: mgr.remove(read_line("Enter SKU to remove: "));
                        std::cout << UI::GREEN << "Product removed.\n" << UI::RESET;
                        break;
                case 9: mgr.undo();
                        std::cout << UI::GREEN << "Previous action reverted.\n" << UI::RESET;
                        break;
                case 10: mgr.save_all();
                         std::cout << UI::GREEN << "All files saved to disk.\n" << UI::RESET;
                         break;
                default: break;
                }
            }
            catch (const InventoryException& e)
            {
                std::cout << UI::RED << "Operation Failed: " << e.what() << UI::RESET << '\n';
            }
        }

        if (role == Role::Admin && mgr.modified())
        {
            std::string ans = read_line("Unsaved modifications exist. Save before exiting? (y/n): ");
            if (ans == "y" || ans == "Y")
            {
                mgr.save_all();
                std::cout << UI::GREEN << "Saved. Good bye!\n" << UI::RESET;
            }
        }
    }
    catch (const EndOfInput&)
    {
        std::cout << "\nInput stream closed. Program terminated.\n";
    }
    catch (const InventoryException& e)
    {
        std::cerr << UI::RED << "Critical Error: " << e.what() << UI::RESET << '\n';
        return 1;
    }

    return 0;
}
