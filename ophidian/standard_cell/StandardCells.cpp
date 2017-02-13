#include "StandardCells.h"

namespace ophidian {
namespace standard_cell {


StandardCells::StandardCells() :
    cellNames_(cells_), pinNames_(pins_), pinDirections_(pins_),
    cellPins_(cells_, pins_)
{

}

StandardCells::StandardCells(StandardCells && stdCell) :
	cells_(std::move(stdCell.cells_)),
	pins_(std::move(stdCell.pins_)),
    cellNames_(std::move(stdCell.cellNames_)),
    pinNames_(std::move(stdCell.pinNames_)),
    pinDirections_(std::move(stdCell.pinDirections_)),
    cellPins_(std::move(stdCell.cellPins_))
{

}

StandardCells::~StandardCells() = default;

//--------------------------- Cells -------------------------------//

Cell StandardCells::add(Cell, std::string name)
{
    auto cell = cells_.add();
    cellNames_[cell] = name;
    return cell;
}

void StandardCells::erase(const Cell & cell)
{
	cells_.erase(cell);
}

void StandardCells::reserve(Cell, uint32_t size)
{
	cells_.reserve(size);
}

uint32_t StandardCells::size(Cell) const
{
	return cells_.size();
}

uint32_t StandardCells::capacity(Cell) const
{
	return cells_.capacity();
}

std::string StandardCells::name(const Cell & cell) const
{
    return cellNames_[cell];
}

entity_system::Association<Cell, Pin>::Parts StandardCells::pins(const Cell &cell) const
{
    return cellPins_.parts(cell);
}

ophidian::util::Range<StandardCells::CellsIterator> StandardCells::range(Cell) const
{
	return ophidian::util::Range<StandardCells::CellsIterator>(cells_.begin(), cells_.end());
}

//--------------------------- Pins -------------------------------//

Pin StandardCells::add(Pin, std::string name, PinDirection direction)
{
    auto pin = pins_.add();
    pinNames_[pin] = name;
    pinDirections_[pin] = direction;
    return pin;
}

void StandardCells::erase(const Pin & pin)
{
	pins_.erase(pin);
}

void StandardCells::reserve(Pin, uint32_t size)
{
	pins_.reserve(size);
}

uint32_t StandardCells::size(Pin) const
{
	return pins_.size();
}

uint32_t StandardCells::capacity(Pin) const
{
	return pins_.capacity();
}

std::string StandardCells::name(const Pin & pin) const
{
	return pinNames_[pin];
}

PinDirection StandardCells::direction(const Pin & pin)
{
    return pinDirections_[pin];
}

Cell StandardCells::owner(const Pin & pin)
{
    return cellPins_.whole(pin);
}

ophidian::util::Range<StandardCells::PinsIterator> StandardCells::range(Pin) const
{
	return ophidian::util::Range<StandardCells::PinsIterator>(pins_.begin(), pins_.end());
}

//--------------------------- Association -------------------------------//

//! Add Pin into Cell
/*!
   \brief Adds a Pin to a given Cell.
   \param cell A handler for the Cell we want to add a Pin.
   \param pin A handler for the Pin we want to add in \p cell.
 */
void StandardCells::add(const Cell& cell, const Pin& pin)
{
	cellPins_.addAssociation(cell, pin);
}

} //namespace ophidian

} //namespace standard_cell