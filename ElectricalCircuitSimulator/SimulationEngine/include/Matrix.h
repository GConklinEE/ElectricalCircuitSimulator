#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
using std::swap;
using std::unique_ptr;
using std::make_unique;

namespace SimulationEngine {

    class PLU_Factorization;
    class Matrix;

    // All indexes in this class are zero-based
    class Matrix final
    {
        public:

            Matrix() {
                m_iNumRows = 0;
                m_iNumColumns = 0;
                m_pMatrix = new double* [0];
            };
            Matrix(const int iRows, const int iColumns);
            Matrix(const Matrix& oMatrix);
            Matrix(Matrix&&) = delete;

            ~Matrix() {
                delete m_pMatrix;
            }

            Matrix& operator=(const Matrix&) = delete;
            Matrix& operator=(Matrix&&) = delete;

            double& operator()(const int iRow, const int iColumn) const {
                return m_pMatrix[iRow][iColumn];
            }

            int getNumRows() const {
                return m_iNumRows;
            }
            int getNumColumns() const {
                return m_iNumColumns;
            }
            double getValue(const int iRow, const int iColumn) const;
            void setValue(const int iRow, const int iColumn, const double dValue);
            void swapRows(const int iRow1, const int iRow2) {
                swap(m_pMatrix[iRow1], m_pMatrix[iRow2]);
            }
            void swapValues(const int iRow1, const int iColumn1, const int iRow2, const int iColumn2) {
                swap(m_pMatrix[iRow1][iColumn1], m_pMatrix[iRow2][iColumn2]);
            }
            void clear();
            void printMatrix() const;
            static void linearSystemSolver(const Matrix& oSourceVector, const PLU_Factorization& oPLU, Matrix& oVoltageVector);

        private:

            int m_iNumRows;
            int m_iNumColumns;
            double** m_pMatrix; // Outer pointer is rows, inner pointer is columns
    };

    // Represents a matrix factored into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    class PLU_Factorization final
    {
        public:

            PLU_Factorization() {
                m_pL = make_unique<Matrix>();
                m_pU = make_unique<Matrix>();
                m_pP = make_unique<Matrix>();
                m_pQ = make_unique<Matrix>();
            }
            PLU_Factorization(const Matrix& oConductanceMatrix) {
                runPLU_Factorization(oConductanceMatrix);
            }

            Matrix& getL() const {
                return *m_pL;
            }
            Matrix& getU() const {
                return *m_pU;
            }
            Matrix& getP() const {
                return *m_pP;
            }
            Matrix& getQ() const {
                return *m_pQ;
            }

        private:

            void runPLU_Factorization(const Matrix& oConductanceMatrix);

            unique_ptr<Matrix> m_pL; // Lower triangular matrix
            unique_ptr<Matrix> m_pU; // Upper triangular matrix
            unique_ptr<Matrix> m_pP; // Row permutation matrix
            unique_ptr<Matrix> m_pQ; // Column permutation matrix
    };

}

#endif