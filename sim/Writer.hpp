#ifndef WRITER_HPP
#define WRITER_HPP
#include <fstream>
#include <exception>
#include <string>

template<int output_type, typename Iter>
class Writer{
    private:
        std::string filename;
        std::ofstream fout;

    public:
        Writer(const std::string &_filename) : filename(_filename)
        {
            fout.open(_filename);
            return;
        }
        ~Writer() { fout.close(); return; }

        void WriteCSV(std::string &filename, Iter begin, Iter end)
        {
            if (fout.fail()) {
                throw std::runtime_error("Can't write to file \'" + filename + "\'.\n");
            }
            if (begin == end) {
                return;
            }
            fout << (*begin).getCSVHeaderString();
            for (auto i = begin; i != end; i++) {
                fout << (*i).getCSVRowString();
            }
            return;
        }
};

#endif //WRITER_HPP
