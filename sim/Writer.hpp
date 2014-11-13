#ifndef WRITER_HPP
#define WRITER_HPP
#include <fstream>
#include <exception>
#include <string>

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

        template<typename Iter>
        void WriteCSV(Iter begin, Iter end)
        {
            if (fout.fail()) {
                throw std::runtime_error("Can't write to file \'" + filename + "\'.\n");
            }
            if (begin == end) {
                return;
            }
            fout << (*begin).getCSVHeaderString() << '\n';
            for (auto i = begin; i != end; i++) {
                fout << (*i).getCSVRowString() << '\n';
            }
            return;
        }
};

#endif //WRITER_HPP
