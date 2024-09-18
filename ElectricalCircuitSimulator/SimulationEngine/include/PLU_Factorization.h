#pragma once
#include "Matrix.h"

namespace SimulationEngine
{
  // Represents a matrix factored into P*A*Q = L*U, where P = Row Permutation Matrix,
  // and Q = Column Permutation Matrix
  template<Numeric T>
  class PLU_Factorization final
  {
  public:
    #pragma region Constructors and Destructors
    PLU_Factorization(const Matrix<T>& A = Matrix<T>{})
      : _A(A),
        _L(_A.getNumRows(), _A.getNumRows()),
        _U(_A),
        _P(_A.getNumRows(), 1),
        _Q(_A.getNumRows(), 1)
    {
      runPLU_Factorization();
    }
    #pragma endregion

    #pragma region Observers
    const Matrix<T>& getL() const
    {
      return _L;
    }

    const Matrix<T>& getU() const
    {
      return _U;
    }

    const Matrix<size_t>& getP() const
    {
      return _P;
    }

    const Matrix<size_t>& getQ() const
    {
      return _Q;
    }

    Matrix<T> solve(const Matrix<T>& b)
    {
      static const T EPSILON = 1e-9;

      auto numRows = b.getNumRows();
      Matrix<T> X(numRows);
      Matrix<T> Y(numRows);
      Matrix<T> B_Permuted(numRows);
      Matrix<T> solution(numRows);

      // Apply row permutations to B to get B_Permuted
      for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
        B_Permuted(rowIndex) = b(_P(rowIndex));

      // Forward substitution to solve LY = B_Permuted
      for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
      {
        Y(rowIndex) = B_Permuted(rowIndex);
        for (size_t rowIndex2 = 0; rowIndex2 < rowIndex; ++rowIndex2)
          Y(rowIndex) -= _L(rowIndex, rowIndex2)* Y(rowIndex2);
      }

      // Backward substitution to solve UX = Y
      for (size_t rowIndex = numRows - 1; rowIndex-->0;)
      {
        X(rowIndex) = Y(rowIndex);
        for (size_t rowIndex2 = rowIndex + 1; rowIndex2 < numRows; ++rowIndex2)
          X(rowIndex) -= _U(rowIndex, rowIndex2) * X(rowIndex2);
        
        auto uDiagonal = _U(rowIndex, rowIndex);
        // If a diagonal on the U matrix is 0, it's due to the ground node being included in the matrix,
        // and is effectively infinity, resulting in oX = 0 for this row.
        X(rowIndex) = uDiagonal > EPSILON || uDiagonal < -EPSILON ? X(rowIndex) / uDiagonal
                                                                  : T{};
      }

      // Apply column permutations to X using Q to get X_Final
      for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
        solution(_Q(rowIndex)) = X(rowIndex);

      return solution;
    }
    #pragma endregion

  private:
    Matrix<T> _A; //Matrix to decompose
    Matrix<T> _L; //Lower triangular matrix
    Matrix<T> _U; //Upper triangular matrix
    Matrix<size_t> _P; //Row permuation matrix
    Matrix<size_t> _Q; //Column permutation matrix

    void runPLU_Factorization()
    {
      auto numRows = _A.getNumRows();
      //Initialize L to an identity matrix
      for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
        _L(rowIndex, rowIndex) = 1;

      for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
        _P(rowIndex, 0) = _Q(rowIndex, 0) = rowIndex;

      for (size_t rowIndex3 = 0; rowIndex3 < numRows; ++rowIndex3)
      {
        // Find the pivot (maximum element) in the submatrix U[rowIndex3:numRows][rowIndex3:numRows]
        T maxValue = T{};
        auto maxRow = rowIndex3;
        auto maxColumn = rowIndex3;
        for (size_t rowIndex1 = rowIndex3; rowIndex1 < numRows; ++rowIndex1)
        {
          for (size_t rowIndex2 = rowIndex3; rowIndex2 < numRows; ++rowIndex2)
          {
            auto absoluteValue = _U(rowIndex1, rowIndex2) < 0 ? -_U(rowIndex1, rowIndex2) : _U(rowIndex1, rowIndex2);
            if (absoluteValue > maxValue)
            {
              maxValue = absoluteValue;
              maxRow = rowIndex1;
              maxColumn = rowIndex2;
            }
          }
        }

        // Swap rows in U to move the pivot to position (iI3, iI3)
        _U.swapRows(rowIndex3, maxRow);
        _P.swapRows(rowIndex3, maxRow);

        // Swap columns in U to move the pivot to position (iI3, iI3)
        for (size_t rowIndex1 = 0; rowIndex1 < numRows; ++rowIndex1)
          _U.swapValues(rowIndex1, rowIndex3, rowIndex1, maxColumn);

        _Q.swapRows(rowIndex3, maxColumn);

        // Update L for the row swaps
        for (size_t rowIndex1 = 0; rowIndex1 < rowIndex3; ++rowIndex1)
          _L.swapValues(rowIndex3, rowIndex1, maxRow, rowIndex1);

        // Compute multipliers and update U
        for (auto rowIndex1 = rowIndex3 + 1; rowIndex1 < numRows; ++rowIndex1)
        {
          _L(rowIndex1, rowIndex3) = _U(rowIndex1, rowIndex3) / _U(rowIndex3, rowIndex3);
          for (auto rowIndex2 = rowIndex3 + 1; rowIndex2 < numRows; ++rowIndex2)
            _U(rowIndex1, rowIndex2) = _U(rowIndex1, rowIndex2) - _L(rowIndex1, rowIndex3) * _U(rowIndex3, rowIndex2);
          _U(rowIndex1, rowIndex3) = T{};
        }
      }
    }
  };
}
