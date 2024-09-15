#include "Matrix.h"

using std::cout;
using std::endl;
using std::invalid_argument;
using std::exception;
using std::string;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::swap;

namespace SimulationEngine {

    Matrix::Matrix() :
        m_iNumRows(0),
        m_iNumColumns(0),
        m_pMatrix(new double* [0])
    { ; }

    Matrix::Matrix(const size_t iRows, const size_t iColumns) {
        size_t iRow;

        if ((iRows < 1) || (iColumns < 1)) {
            cout << "Matrix dimensions must be positive and non-zero!" << endl;
            throw invalid_argument("Matrix dimensions must be positive and non-zero!");
        }

        m_iNumRows = iRows;
        m_iNumColumns = iColumns;
        m_pMatrix = new double* [m_iNumRows];
        for(iRow = 0; iRow < m_iNumRows; iRow++) {
            m_pMatrix[iRow] = new double[m_iNumColumns];
        }
        clear();
    }

    Matrix::Matrix(const Matrix& oMatrix) {
        size_t iRow;
        size_t iColumn;
        m_iNumRows = oMatrix.getNumRows();
        m_iNumColumns = oMatrix.getNumColumns();
        m_pMatrix = new double* [m_iNumRows];
        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            m_pMatrix[iRow] = new double[m_iNumColumns];
        }

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            for (iColumn = 0; iColumn < m_iNumColumns; iColumn++) {
                m_pMatrix[iRow][iColumn] = oMatrix(iRow, iColumn);
            }
        }
    }

    double Matrix::getValue(const size_t iRow, const size_t iColumn) const {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        return m_pMatrix[iRow][iColumn];
    }

    void Matrix::setValue(const size_t iRow, const size_t iColumn, const double dValue) {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        m_pMatrix[iRow][iColumn] = dValue;
    }

    void Matrix::swapRows(const size_t iRow1, const size_t iRow2) {
        swap(m_pMatrix[iRow1], m_pMatrix[iRow2]);
    }

    void Matrix::swapValues(const size_t iRow1, const size_t iColumn1, const size_t iRow2, const size_t iColumn2) {
        swap(m_pMatrix[iRow1][iColumn1], m_pMatrix[iRow2][iColumn2]);
    }

    void Matrix::clear() {
        size_t iRow;
        size_t iColumn;

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            for (iColumn = 0; iColumn < m_iNumColumns; iColumn++) {
                m_pMatrix[iRow][iColumn] = 0.0;
            }
        }
    }

    const string Matrix::printMatrix() const {
        size_t iRow;
        size_t iColumn;
        string sMatrix = "";

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            sMatrix += "[" + to_string(m_pMatrix[iRow][0]);
            for (iColumn = 1; iColumn < m_iNumColumns; iColumn++) {
                sMatrix += "\t" + to_string(m_pMatrix[iRow][iColumn]);
            }
            sMatrix += "]\n";
        }
        sMatrix += '\n';

        return sMatrix;
    }

    PLU_Factorization::PLU_Factorization() :
        m_pL(make_unique<Matrix>()),
        m_pU(make_unique<Matrix>()),
        m_pP(make_unique<Matrix>()),
        m_pQ(make_unique<Matrix>())
    { ; }

    PLU_Factorization::PLU_Factorization(const Matrix& oMatrix) {
        runPLU_Factorization(oMatrix);
    }

    // Function to perform PLU factorization with row and column pivoting
    // Factors a matrix into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    void PLU_Factorization::runPLU_Factorization(const Matrix& oMatrix) {
        size_t iRows = oMatrix.getNumRows();
        size_t iColumns = oMatrix.getNumColumns();
        double dMaxVal;
        size_t iMaxRow;
        size_t iMaxCol;
        size_t iI1;
        size_t iI2;
        size_t iI3;

        if ((iRows == 0) || (iColumns == 0)) {
            cout << "Cannot factor a matrix with dimensions of zero!" << endl;
            throw invalid_argument("Cannot factor a matrix with dimensions of zero!");
        }

        // Initialize P and Q as identity permutations (single column matrices)
        m_pP = make_unique<Matrix>(iRows, 1);
        m_pQ = make_unique<Matrix>(iRows, 1);
        m_pL = make_unique<Matrix>(iRows, iRows);
        m_pU = make_unique<Matrix>(oMatrix); // Initialize U to be a copy of A

        // Initialize L to an idenity matrix
        for (iI1 = 0; iI1 < iRows; ++iI1) {
            (*m_pL)(iI1, iI1) = 1.0;
        }

        for (iI1 = 0; iI1 < iRows; ++iI1) {
            (*m_pP)(iI1, 0) = (double)iI1;
            (*m_pQ)(iI1, 0) = (double)iI1;
        }

        for (iI3 = 0; iI3 < iRows; ++iI3) {
            // Find the pivot (maximum element) in the submatrix U[iI3:m_iNumRows][iI3:m_iNumRows]
            dMaxVal = 0.0;
            iMaxRow = iI3;
            iMaxCol = iI3;
            for (iI1 = iI3; iI1 < iRows; ++iI1) {
                for (iI2 = iI3; iI2 < iRows; ++iI2) {
                    if (fabs((*m_pU)(iI1, iI2)) > dMaxVal) {
                        dMaxVal = fabs((*m_pU)(iI1, iI2));
                        iMaxRow = iI1;
                        iMaxCol = iI2;
                    }
                }
            }

            // Swap rows in U to move the pivot to position (iI3, iI3)
            m_pU->swapRows(iI3, iMaxRow);
            m_pP->swapRows(iI3, iMaxRow);

            // Swap columns in U to move the pivot to position (iI3, iI3)
            for (iI1 = 0; iI1 < iRows; ++iI1) {
                m_pU->swapValues(iI1, iI3, iI1, iMaxCol);
            }
            m_pQ->swapRows(iI3, iMaxCol);

            // Update L for the row swaps
            for (iI1 = 0; iI1 < iI3; ++iI1) {
                m_pL->swapValues(iI3, iI1, iMaxRow, iI1);
            }

            // Compute multipliers and update U
            for (iI1 = iI3 + 1; iI1 < iRows; ++iI1) {
                (*m_pL)(iI1, iI3) = (*m_pU)(iI1, iI3) / (*m_pU)(iI3, iI3);
                for (iI2 = iI3 + 1; iI2 < iRows; ++iI2) {
                    (*m_pU)(iI1, iI2) = (*m_pU)(iI1, iI2) - (*m_pL)(iI1, iI3) * (*m_pU)(iI3, iI2);
                }
                (*m_pU)(iI1, iI3) = 0.0;
            }
        }
    }

}