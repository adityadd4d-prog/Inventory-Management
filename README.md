# Inventory Management System

A terminal-based Inventory Management System built in C using ncurses for the UI. Developed as a Student Semester Project (SSP) for the Data Structures course.

## Features

- **Hash Table Storage** — Inventory items are stored in a hash table (using the djb2 hashing algorithm) for fast barcode-based lookups.
- **Barcode Search** — Look up items instantly by entering their 13-digit barcode.
- **OCR Search** — Scan barcode images using Tesseract OCR to search for items without manual entry.
- **Item Management** — Add new items and update price, stock quantity, or active/discontinued status.
- **Report Generation** — Generate CSV reports for:
  - **Low Stock** — Active items with stock percentage below 34% of capacity.
  - **Bestsellers** — Items with negative net transactions (more sold than received) and stock below 50%.
  - **Slow Stock** — Items with positive net transactions (more received than sold) and stock above 67%.
- **File Operations** — Load inventory from CSV, save changes back to CSV, and purge discontinued items.
- **Admin Authentication** — File operations and status changes to discontinued items require admin credentials.
- **LibreOffice Integration** — Open generated reports directly in LibreOffice Calc from within the application.

## Tech Stack

| Component     | Technology            |
|---------------|-----------------------|
| Language      | C                     |
| Build System  | CMake                 |
| UI            | ncurses / menu        |
| OCR           | Tesseract             |
| Data Format   | CSV                   |

## Prerequisites

- **Linux** (developed and tested on Arch Linux)
- GCC or another C compiler
- CMake ≥ 3.10
- ncurses development libraries
- Tesseract OCR (optional, for barcode image search)

### Install dependencies

**Arch Linux:**

```bash
sudo pacman -S cmake gcc ncurses tesseract
```

**Debian / Ubuntu:**

```bash
sudo apt install cmake gcc libncurses-dev libmenu-dev tesseract-ocr
```

## Building

```bash
mkdir -p build && cd build
cmake ..
make
```

The compiled binary `Inventory` will be created in the `build/` directory.

## Usage

```bash
./build/Inventory
```

Navigate menus using the **Arrow Keys** and **Enter**. Press **F1** to select the last menu option (Back/Exit).

### Menu Structure

```
Main Menu
├── Items
│   ├── Search
│   │   ├── Barcode   — Search by entering a barcode
│   │   └── OCR       — Search by providing a barcode image path
│   ├── Update        — Update price, stock quantity, or status
│   └── Add Item      — Add a new inventory item
├── Report
│   ├── Low Stock Items
│   ├── Bestsellers
│   └── Slow Stock
├── File (requires admin login)
│   ├── Load File     — Load inventory CSV into memory
│   ├── Save File     — Save current inventory to CSV
│   └── Purge Discontinued Items
├── About
└── Exit
```

### CSV Format

The inventory CSV uses the following columns:

```
Index, Barcode, Name, Price, Stock, Transactions, Capacity, Percent, Status
```

- **Barcode** — 13-digit product barcode
- **Transactions** — Net stock transactions (negative = sold, positive = received)
- **Capacity** — Maximum stock capacity for the item
- **Percent** — Current stock as a percentage of capacity
- **Status** — `1` for Active, `0` for Discontinued

A sample dataset is provided at `data/inventory.csv`.

## Project Structure

```
.
├── CMakeLists.txt          # Build configuration
├── README.md
├── src/
│   ├── main.c              # Application entry point and main menu logic
│   ├── functions.h          # Shared header with structs, constants, and prototypes
│   ├── hashTable.c          # Hash table implementation, CSV I/O, reports, and OCR
│   └── ui.c                 # ncurses UI: menus, forms, item display, and admin auth
├── data/
│   └── inventory.csv        # Sample inventory dataset
├── Backup/
│   └── inventory.csv        # Backup copy of the dataset
├── Barcodes/                # Sample barcode images for OCR testing
├── Reports/                 # Generated report CSV files
│   ├── Low_Stock.csv
│   ├── Bestsellers.csv
│   └── Slow_Stock.csv
└── build/                   # Build output directory
```

## Author

**Aditya Patel** — [GitHub](https://github.com/adityadd4d-prog)

## License

This project is part of an academic course and does not currently include a license.
