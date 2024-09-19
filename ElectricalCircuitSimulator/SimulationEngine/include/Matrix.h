#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <string>
#include <memory>

namespace SimulationEngine {

    class PLU_Factorization;
    class Matrix;

    // All indexes in this class are zero-based
    class Matrix final
    {
        public:

            Matrix();
            Matrix(const size_t iRows, const size_t iColumns);
            Matrix(const Matrix& oMatrix);
            Matrix(Matrix&&) = delete;

            ~Matrix() {
                delete m_pMatrix;
            }

            Matrix& operator=(const Matrix&) = delete;
            Matrix& operator=(Matrix&&) = delete;
            const double& operator()(const size_t iRow, const size_t iColumn) const {
                return m_pMatrix[iRow][iColumn];
            }
            double& operator()(const size_t iRow, const size_t iColumn) {
                return m_pMatrix[iRow][iColumn];
            }

            size_t getNumRows() const {
                return m_iNumRows;
            }
            size_t getNumColumns() const {
                return m_iNumColumns;
            }
            double getValue(const size_t iRow, const size_t iColumn) const;
            void setValue(const size_t iRow, const size_t iColumn, const double dValue);
            void swapRows(const size_t iRow1, const size_t iRow2);
            void swapValues(const size_t iRow1, const size_t iColumn1, const size_t iRow2, const size_t iColumn2);
            void clear();
            const std::string printMatrix() const;

        private:

            size_t m_iNumRows;
            size_t m_iNumColumns;
            double** m_pMatrix; // Outer pointer is rows, inner pointer is columns
    };

    // Represents a matrix factored into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    class PLU_Factorization final
    {
        public:

            PLU_Factorization();
            PLU_Factorization(const Matrix& oMatrix);

            const Matrix& getL() const {
                return *m_pL;
            }
            const Matrix& getU() const {
                return *m_pU;
            }
            const Matrix& getP() const {
                return *m_pP;
            }
            const Matrix& getQ() const {
                return *m_pQ;
            }

        private:

            void runPLU_Factorization(const Matrix& oMatrix);

            std::unique_ptr<Matrix> m_pL; // Lower triangular matrix
            std::unique_ptr<Matrix> m_pU; // Upper triangular matrix
            std::unique_ptr<Matrix> m_pP; // Row permutation matrix
            std::unique_ptr<Matrix> m_pQ; // Column permutation matrix
    };

}

#endif