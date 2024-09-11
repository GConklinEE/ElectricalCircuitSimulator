#include "Matrix.h"

using std::cout;
using std::endl;
using std::swap;
using std::invalid_argument;
using std::exception;

#define ZERO (1e-9) // If a diagonal on the U matrix is less than or equal to this, it is considered a zero

namespace SimulationEngine {

    Matrix::Matrix(const int iRows, const int iColumns) {
        int iRow;

        if ((iRows < 1) || (iColumns < 1)) {
            cout << "Matrix dimensions must be positive and non-zero!" << endl;
            throw invalid_argument("Matrix dimensions must be positive and non-zero!");
        }

        m_iNumRows = iRows;
        m_iNumColumns = iColumns;
        m_oMatrix = new double*[m_iNumRows];
        for(iRow = 0; iRow < m_iNumRows; iRow++) {
            m_oMatrix[iRow] = new double[m_iNumColumns];
        }
        clear();
    }

    double Matrix::getValue(const int iRow, int const iColumn) const {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        return m_oMatrix[iRow][iColumn];
    }

    void Matrix::setValue(const int iRow, const int iColumn, const double dValue) {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        m_oMatrix[iRow][iColumn] = dValue;
    }

    void Matrix::clear() {
        int iRow;
        int iColumn;

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            for (iColumn = 0; iColumn < m_iNumColumns; iColumn++) {
                m_oMatrix[iRow][iColumn] = 0.0;
            }
        }
    }

    Matrix* Matrix::clone() const {
        int iRow;
        int iColumn;
        Matrix* pNewMatrix = new Matrix(m_iNumRows, m_iNumColumns);

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            for (iColumn = 0; iColumn < m_iNumColumns; iColumn++) {
                pNewMatrix->m_oMatrix[iRow][iColumn] = m_oMatrix[iRow][iColumn];
            }
        }

        return pNewMatrix;
    }

    void Matrix::printMatrix() const {
        int iRow;
        int iColumn;

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            cout << "[" << m_oMatrix[iRow][0];
            for (iColumn = 1; iColumn < m_iNumColumns; iColumn++) {
                cout << "\t" << m_oMatrix[iRow][iColumn];
            }
            cout << "]\n";
        }
        cout << endl;
    }

    // Function to perform PLU factorization with row and column pivoting
    // Factors a matrix into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    PLU_Factorization* Matrix::runPLU_Factorization() const {
        PLU_Factorization* pResults = new PLU_Factorization();
        double dMaxVal;
        int iMaxRow;
        int iMaxCol;
        int iI1;
        int iI2;
        int iI3;

        if ((m_iNumRows == 0) || (m_iNumColumns == 0)) {
            cout << "Cannot factor a matrix with dimensions of zero!" << endl;
            throw invalid_argument("Cannot factor a matrix with dimensions of zero!");
        }

        // Initialize P and Q as identity permutations (single column matrices)
        pResults->m_pP = new Matrix(m_iNumRows, 1);
        pResults->m_pQ = new Matrix(m_iNumRows, 1);
        pResults->m_pL = new Matrix(m_iNumRows, m_iNumRows);
        pResults->m_pU = clone(); // Initialize U to be a copy of A

        // Initialize L to an idenity matrix
        for (iI1 = 0; iI1 < m_iNumRows; ++iI1) {
            pResults->m_pL->m_oMatrix[iI1][iI1] = 1.0;
        }

        for (iI1 = 0; iI1 < m_iNumRows; ++iI1) {
            pResults->m_pP->m_oMatrix[iI1][0] = iI1;
            pResults->m_pQ->m_oMatrix[iI1][0] = iI1;
        }

        for (iI3 = 0; iI3 < m_iNumRows; ++iI3) {
            // Find the pivot (maximum element) in the submatrix U[iI3:m_iNumRows][iI3:m_iNumRows]
            dMaxVal = 0.0;
            iMaxRow = iI3;
            iMaxCol = iI3;
            for (iI1 = iI3; iI1 < m_iNumRows; ++iI1) {
                for (iI2 = iI3; iI2 < m_iNumRows; ++iI2) {
                    if (fabs(pResults->m_pU->m_oMatrix[iI1][iI2]) > dMaxVal) {
                        dMaxVal = fabs(pResults->m_pU->m_oMatrix[iI1][iI2]);
                        iMaxRow = iI1;
                        iMaxCol = iI2;
                    }
                }
            }

            // Swap rows in U to move the pivot to position (iI3, iI3)
            swap(pResults->m_pU->m_oMatrix[iI3], pResults->m_pU->m_oMatrix[iMaxRow]);
            swap(pResults->m_pP->m_oMatrix[iI3], pResults->m_pP->m_oMatrix[iMaxRow]);

            // Swap columns in U to move the pivot to position (iI3, iI3)
            for (iI1 = 0; iI1 < m_iNumRows; ++iI1) {
                swap(pResults->m_pU->m_oMatrix[iI1][iI3], pResults->m_pU->m_oMatrix[iI1][iMaxCol]);
            }
            swap(pResults->m_pQ->m_oMatrix[iI3], pResults->m_pQ->m_oMatrix[iMaxCol]);

            // Update L for the row swaps
            for (iI1 = 0; iI1 < iI3; ++iI1) {
                swap(pResults->m_pL->m_oMatrix[iI3][iI1], pResults->m_pL->m_oMatrix[iMaxRow][iI1]);
            }

            // Compute multipliers and update U
            for (iI1 = iI3 + 1; iI1 < m_iNumRows; ++iI1) {
                pResults->m_pL->m_oMatrix[iI1][iI3] = pResults->m_pU->m_oMatrix[iI1][iI3] / pResults->m_pU->m_oMatrix[iI3][iI3];
                for (iI2 = iI3 + 1; iI2 < m_iNumRows; ++iI2) {
                    pResults->m_pU->m_oMatrix[iI1][iI2] -= pResults->m_pL->m_oMatrix[iI1][iI3] * pResults->m_pU->m_oMatrix[iI3][iI2];
                }
                pResults->m_pU->m_oMatrix[iI1][iI3] = 0.0;
            }
        }

        return pResults;
    }

    Matrix* Matrix::linearSystemSolver(const Matrix& oB, const PLU_Factorization& oPLU) {
        int iI1;
        int iI2;
        int iIndex;
        int iNumRows = oB.m_iNumRows;
        Matrix oX(iNumRows, 1); // UX = Y
        Matrix oY(iNumRows, 1); // LY = B_Permuted
        Matrix oB_Permuted(iNumRows, 1);
        Matrix* pX_Final = new Matrix(iNumRows, 1);

        // Apply row permutations to B to get B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (int)round(oPLU.m_pP->m_oMatrix[iI1][0]);
            oB_Permuted.m_oMatrix[iI1][0] = oB.m_oMatrix[iIndex][0];
        }

        // Forward substitution to solve LY = B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            oY.m_oMatrix[iI1][0] = oB_Permuted.m_oMatrix[iI1][0];
            for (iI2 = 0; iI2 < iI1; ++iI2) {
                oY.m_oMatrix[iI1][0] -= oPLU.m_pL->m_oMatrix[iI1][iI2] * oY.m_oMatrix[iI2][0];
            }
        }

        // Backward substitution to solve UX = Y
        for (iI1 = iNumRows - 1; iI1 >= 0; --iI1) {
            oX.m_oMatrix[iI1][0] = oY.m_oMatrix[iI1][0];
            for (iI2 = iI1 + 1; iI2 < iNumRows; ++iI2) {
                oX.m_oMatrix[iI1][0] -= oPLU.m_pU->m_oMatrix[iI1][iI2] * oX.m_oMatrix[iI2][0];
            }
            if (fabs(oPLU.m_pU->m_oMatrix[iI1][iI1]) > ZERO) {
                oX.m_oMatrix[iI1][0] /= oPLU.m_pU->m_oMatrix[iI1][iI1];
            } else {
                oX.m_oMatrix[iI1][0] = 0; // If a diagonal on the U matrix is 0, it's due to the ground node being included in the matrix, and is effectively infinity, resulting in oX = 0 for this row.
            }
        }

        // Apply column permutations to X using Q to get X_Final
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (int)round(oPLU.m_pQ->m_oMatrix[iI1][0]);
            pX_Final->m_oMatrix[iIndex][0] = oX.m_oMatrix[iI1][0];
        }

#ifdef MATRIX_PRINT
        cout << "X Vector:" << endl;
        oX.printMatrix();
        cout << "Y Vector:" << endl;
        oY.printMatrix();
        cout << "B Permuted Vector:" << endl;
        oB_Permuted.printMatrix();
#endif

        return pX_Final;
    }

}