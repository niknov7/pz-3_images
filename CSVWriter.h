#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

class CSVWriter {
public:
    CSVWriter(const std::string &filename, const std::vector<std::string> &headers);

    void addRow(const std::vector<std::string> &row);

    void save();

    static std::string toCsvString(double value);

private:
    std::string filename;
    std::vector<std::string> headers;
    std::vector<std::vector<std::string> > rows;

    static std::string joinRow(const std::vector<std::string> &row);

    static std::string escapeCell(const std::string &cell);
};

#endif //CSVWRITER_H
