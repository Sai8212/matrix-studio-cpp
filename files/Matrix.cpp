#include "Matrix.h"
#include <sstream>
#include <stdexcept>

using namespace std;

Matrix::Matrix(int r, int c) : rows(r), cols(c), data(r, vector<int>(c, 0)) {}

int Matrix::getRows() const { return rows; }
int Matrix::getCols() const { return cols; }
int Matrix::get(int i, int j) const { return data[i][j]; }
void Matrix::set(int i, int j, int value) { data[i][j] = value; }

// ----- Matrix Addition -----
// Rule: order(A) must equal order(B).
Matrix Matrix::operator+(const Matrix& other) const {
    if (rows != other.rows || cols != other.cols) {
        throw invalid_argument(
            "Addition error: matrices must have the same order. "
            "Matrix A is " + to_string(rows) + "x" + to_string(cols) +
            ", Matrix B is " + to_string(other.rows) + "x" + to_string(other.cols) + "."
        );
    }
    Matrix result(rows, cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result.data[i][j] = data[i][j] + other.data[i][j];
    return result;
}

// ----- Matrix Multiplication -----
// Rule: cols(A) must equal rows(B).
Matrix Matrix::operator*(const Matrix& other) const {
    if (cols != other.rows) {
        throw invalid_argument(
            "Multiplication error: columns of Matrix A (" + to_string(cols) +
            ") must equal rows of Matrix B (" + to_string(other.rows) + ")."
        );
    }
    Matrix result(rows, other.cols);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < other.cols; ++j)
            for (int k = 0; k < cols; ++k)
                result.data[i][j] += data[i][k] * other.data[k][j];
    return result;
}

// ----- Matrix Transpose -----
// No dimension restriction: works on any matrix.
Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            result.set(j, i, data[i][j]);
    return result;
}

string Matrix::toString() const {
    ostringstream out;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            out << data[i][j];
            if (j != cols - 1) out << "\t";
        }
        out << "\r\n";
    }
    return out.str();
}

Matrix parseMatrixFromText(const string& text, const string& label) {
    istringstream ss(text);
    int r, c;

    if (!(ss >> r >> c)) {
        throw invalid_argument(
            "Matrix " + label + ": the first line must contain two whole "
            "numbers - rows and columns."
        );
    }
    if (r <= 0 || c <= 0) {
        throw invalid_argument(
            "Matrix " + label + ": rows and columns must be positive integers."
        );
    }

    Matrix m(r, c);
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            int value;
            if (!(ss >> value)) {
                throw invalid_argument(
                    "Matrix " + label + ": expected " + to_string(r * c) +
                    " elements (a " + to_string(r) + "x" + to_string(c) +
                    " matrix) but not enough numbers were entered."
                );
            }
            m.set(i, j, value);
        }
    }
    return m;
}
