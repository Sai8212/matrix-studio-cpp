#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <string>

// Encapsulation: the grid (rows, cols, data) is private.
// Other code can only interact with a Matrix through its public methods.
class Matrix {
private:
    int rows;
    int cols;
    std::vector<std::vector<int> > data;

public:
    Matrix(int r, int c);

    int getRows() const;
    int getCols() const;
    int get(int i, int j) const;
    void set(int i, int j, int value);

    // Operator overloading (OOP requirement)
    Matrix operator+(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix transpose() const;

    // Formats the matrix as text with CRLF line endings, ready to
    // drop straight into a Windows edit control.
    std::string toString() const;
};

// Parses text typed into a text box, shaped like:
//   rows cols
//   e1 e2 e3 ...
// into a Matrix. Throws std::invalid_argument with a clear, labelled
// message ("Matrix A: ...") on any malformed input.
Matrix parseMatrixFromText(const std::string& text, const std::string& label);

#endif
