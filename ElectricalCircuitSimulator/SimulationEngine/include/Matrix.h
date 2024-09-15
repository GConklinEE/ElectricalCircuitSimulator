#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

namespace SimulationEngine {

    class PLU_Factorization;
    class Matrix;

    // All indexes in this class are zero-based
    class Matrix
    {
        public:

            Matrix() {
                m_iNumRows = 0;
                m_iNumColumns = 0;
                m_oMatrix = new double* [0];
            };
            Matrix(const int iRows, const int iColumns);

            ~Matrix() {
                delete m_oMatrix;
            }

            int getNumRows() const { 
                return m_iNumRows;
            }
            int getNumColumns() const {
                return m_iNumColumns;
            }
            double getValue(const int iRow, const int iColumn) const;
            void setValue(const int iRow, const int iColumn, const double dValue);
            void clear();
            Matrix* clone() const;
            void printMatrix() const;
            PLU_Factorization* runPLU_Factorization() const;
            static Matrix* linearSystemSolver(const Matrix& oSourceVector, const PLU_Factorization& oPLU_Factorization);

        private:

            int m_iNumRows;
            int m_iNumColumns;
            double** m_oMatrix; // Outer pointer is rows, inner pointer is columns
    };

    // Represents a matrix factored into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    class PLU_Factorization
    {
        public:

            PLU_Factorization() {
                m_pL = new Matrix();
                m_pU = new Matrix();
                m_pP = new Matrix();
                m_pQ = new Matrix();
            };

            ~PLU_Factorization() {
                delete m_pL;
                delete m_pU;
                delete m_pP;
                delete m_pQ;
            };

            Matrix* m_pL; // Lower triangular matrix
            Matrix* m_pU; // Upper triangular matrix
            Matrix* m_pP; // Row permutation matrix
            Matrix* m_pQ; // Column permutation matrix
    };

}

#endif