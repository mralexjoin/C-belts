#include <istream>
#include <ostream>
#include <stdexcept>
#include <vector>

using namespace std;

class Matrix {
public:
  Matrix() {}
  Matrix(const int& num_rows, const int& num_cols) {
    ChangeSize(num_rows, num_cols);
  }
  void Reset(const int& num_rows, const int& num_cols) {
    ChangeSize(num_rows, num_cols);
  }
  int At(const int& row, const int& col) const {
    CheckRowAndCol(row, col);
    return matrix[row][col];
  }
  int& At(const int& row, const int& col) {
    CheckRowAndCol(row, col);
    return matrix[row][col];
  }
  size_t GetNumRows() const {
    return rows;
  }
  size_t GetNumColumns() const {
    return cols;
  }
private:
  void CheckRowAndCol(const int& row, const int& col) const {
    if (0 > row || row >= GetNumRows() || 0 > col || col >= GetNumColumns())
      throw std::out_of_range("");
  }
  void ChangeSize(const int& num_rows, const int& num_cols) {
    if (num_rows < 0 || num_cols < 0)
      throw std::out_of_range("");
    rows = num_rows;
    cols = num_cols;
    matrix.assign(rows, std::vector<int>(cols));
    for (auto& row : matrix)
      row.assign(cols, 0);
  }
  size_t rows = 0;
  size_t cols = 0;
  std::vector<std::vector<int>> matrix;
};

std::istream& operator>>(std::istream& stream, Matrix& matrix) {
  int rows, cols;
  stream >> rows >> cols;
  matrix = {rows, cols};
  for (size_t row = 0; row < matrix.GetNumRows(); row++)
    for (size_t col = 0; col < matrix.GetNumColumns(); col++)
      stream >> matrix.At(row, col);
  return stream;
}

std::ostream& operator<<(std::ostream& stream, const Matrix& matrix) {
  stream << matrix.GetNumRows() << ' ' << matrix.GetNumColumns() << std::endl;
  for (size_t row = 0; row < matrix.GetNumRows(); row++) {
    for (size_t col = 0; col < matrix.GetNumColumns(); col++)
      stream << matrix.At(row, col) << ' ';
    stream << std::endl;
  }
  return stream;
}

bool IsSizeEqual(const Matrix& lhs, const Matrix& rhs) {
  return (lhs.GetNumRows() == rhs.GetNumRows()
          && lhs.GetNumColumns() == rhs.GetNumColumns());
}

bool operator==(const Matrix& lhs, const Matrix& rhs) {
  if (IsSizeEqual(lhs, rhs)) {
    for (size_t row = 0; row < lhs.GetNumRows(); row++)
      for (size_t col = 0; col < lhs.GetNumColumns(); col++)
        if (lhs.At(row, col) != rhs.At(row, col))
          return false;
    return true;
  }
  return false;
}

Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
  if (!IsSizeEqual(lhs, rhs))
    throw std::invalid_argument("Matrices are not equal");

  Matrix result(lhs.GetNumRows(), lhs.GetNumColumns());
  for (size_t row = 0; row < lhs.GetNumRows(); row++)
    for (size_t col = 0; col < lhs.GetNumColumns(); col++)
      result.At(row, col) = lhs.At(row, col) + rhs.At(row, col);
  return result;
}
