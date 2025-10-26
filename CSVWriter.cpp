#include "CSVWriter.h"
#include <sstream>
#include <fstream>
#include <iomanip>

CSVWriter::CSVWriter(const std::string &filename, const std::vector<std::string> &headers)
    : filename(filename), headers(headers) {
}

void CSVWriter::addRow(const std::vector<std::string> &row) {
    if (row.size() != headers.size()) {
        throw std::invalid_argument("Row size must match headers size");
    }
    rows.push_back(row);
}

std::string CSVWriter::escapeCell(const std::string &cell) {
    bool needQuotes = cell.find(';') != std::string::npos || cell.find('"') != std::string::npos ||
                      cell.front() == ' ' || cell.back() == ' ';
    if (!needQuotes) return cell;

    std::string escaped = "\"";
    for (char c: cell) {
        if (c == '"') escaped += "\"\"";
        else escaped += c;
    }
    escaped += "\"";
    return escaped;
}

std::string CSVWriter::joinRow(const std::vector<std::string> &row) {
    std::ostringstream oss;
    for (size_t i = 0; i < row.size(); ++i) {
        oss << escapeCell(row[i]);
        if (i + 1 < row.size()) oss << ";";
    }
    return oss.str();
}

void CSVWriter::save() {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open CSV file for writing");
    }

    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write(reinterpret_cast<char *>(bom), 3);

    file << joinRow(headers) << "\n";

    for (const auto &row: rows) {
        file << joinRow(row) << "\n";
    }

    file.close();
}

std::string CSVWriter::toCsvString(const double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    std::string s = oss.str();
    for (auto &c : s) {
        if (c == '.') c = ',';
    }
    return s;
}
