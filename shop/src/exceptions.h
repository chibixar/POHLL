#pragma once
#include <stdexcept>
#include <string>

struct InventoryException  : std::runtime_error { using std::runtime_error::runtime_error; };
struct ValidationException : InventoryException { using InventoryException::InventoryException; };
struct FileException       : InventoryException { using InventoryException::InventoryException; };
struct NotFoundException   : InventoryException { using InventoryException::InventoryException; };
