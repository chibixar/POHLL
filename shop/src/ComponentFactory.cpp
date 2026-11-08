#include "ComponentFactory.h"
#include "constants.h"
#include "exceptions.h"
#include "Internals.h"
#include "Peripherals.h"

std::unique_ptr<Component> ComponentFactory::create(int category_id)
{
    switch (category_id)
    {
    case CAT_MOUSE:     return std::make_unique<Mouse>();
    case CAT_KEYBOARD:  return std::make_unique<Keyboard>();
    case CAT_MONITOR:   return std::make_unique<Monitor>();
    case CAT_PROCESSOR: return std::make_unique<Processor>();
    case CAT_RAM:       return std::make_unique<RAM>();
    default: throw NotFoundException("Unknown product category ID: " + std::to_string(category_id));
    }
}
