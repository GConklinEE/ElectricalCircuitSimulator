#pragma once
#include <complex>
#include <memory>
#include <sstream>
#include <string>

namespace SimulationEngine
{
  template<typename T>
  concept Numeric = std::is_arithmetic_v<T> ||
                    requires(T t)
                    {
                      typename T::value_type;
                      requires std::same_as< T, std::complex<typename T::value_type> >;
                      requires std::is_arithmetic_v<typename T::value_type>;
                    };

  template<Numeric T>
  class Matrix final
  {
  public:
    #pragma region Constructors and Destructors
    Matrix(size_t numRows = 1, size_t numColumns = 1)
      : _numRows(numRows),
        _numColumns(numColumns),
        _data(std::make_unique< std::unique_ptr< T[] >[] >(_numRows))
    {
      for (size_t rowIndex = 0; rowIndex < _numRows; ++rowIndex)
        _data[rowIndex] = make_unique<T[]>(_numColumns);
    }

    Matrix(const Matrix& original)
      : Matrix(original._numRows, original._numColumns)
    {
      for (auto rowIndex = 0; rowIndex < _numRows; ++rowIndex)
      {
        for (auto columnIndex = 0; columnIndex < _numColumns; ++columnIndex)
          _data[rowIndex][columnIndex] = original._data[rowIndex][columnIndex];
      }
    }

    Matrix(Matrix&&) = default;
    ~Matrix() = default;
    #pragma endregion

    #pragma region Modifiers
    void swapRows(size_t row1, size_t row2)
    {
      std::swap(_data[row1], _data[row2]);
    }

    void swapValues(size_t row1, size_t column1, size_t row2, size_t column2)
    {
      std::swap(_data[row1][column1], _data[row2][column2]);
    }

    void clear()
    {
      for (size_t rowIndex = 0; rowIndex < _numRows; ++rowIndex)
        for (size_t columnIndex = 0; columnIndex < _numColumns; ++columnIndex)
          _data[rowIndex][columnIndex] = T{};
    }

    std::string getMatrixString() const
    {
      std::stringstream stream;
      for(size_t rowIndex = 0; rowIndex < _numRows; ++rowIndex)
      {
        stream << '[';
        for(size_t columnIndex = 0; columnIndex < _numColumns; ++columnIndex)
          stream << '\t' << _data[rowIndex][columnIndex];
        stream << "\t]";
      }
      return stream.str();
    }
    #pragma endregion

    #pragma region Observers
    size_t getNumRows() const
    {
      return _numRows;
    }

    size_t getNumColumns() const
    {
      return _numColumns;
    }
    #pragma endregion

    #pragma region Operators
    #pragma region Modifiers
    T& operator()(size_t row, size_t column = 0)
    {
      return _data[row][column];
    }

    Matrix& operator=(const Matrix& original)
    {
      _numRows(original._numRows);
      _numColumns(original._numColumns);
      _data(std::make_unique< std::unique_ptr<std::unique_ptr<T[]>[]> >(_numRows));

      for (auto rowIndex = 0; rowIndex < _numRows; ++rowIndex)
      {
        auto row = make_unique<T[]>(_numColumns);
        for (auto columnIndex = 0; columnIndex < _numColumns; ++columnIndex)
          row[columnIndex] = original._data[rowIndex][columnIndex];
        _data[rowIndex] = row;
      }
    }

    Matrix& operator=(Matrix&&) = default;
    #pragma endregion

    #pragma region Observers
    const T& operator()(size_t row, size_t column = 0) const
    {
      return _data[row][column];
    }
    #pragma endregion
    #pragma endregion
  private:
    #pragma region Members
    size_t _numRows;
    size_t _numColumns;
    std::unique_ptr< std::unique_ptr< T[] >[] > _data; // Outer pointer is rows, inner pointer is columns
    #pragma endregion
  };
}