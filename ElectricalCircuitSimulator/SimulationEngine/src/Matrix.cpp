#include "Matrix.h"

using std::cout;
using std::endl;
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
        m_pMatrix = new double*[m_iNumRows];
        for(iRow = 0; iRow < m_iNumRows; iRow++) {
            m_pMatrix[iRow] = new double[m_iNumColumns];
        }
        clear();
    }

    Matrix::Matrix(const Matrix& oMatrix) {
        int iRow;
        int iColumn;
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

    double Matrix::getValue(const int iRow, int const iColumn) const {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        return m_pMatrix[iRow][iColumn];
    }

    void Matrix::setValue(const int iRow, const int iColumn, const double dValue) {
        if ((iRow >= m_iNumRows) || (iColumn >= m_iNumColumns)) {
            cout << "Location beyond dimensions of matrix!" << endl;
            throw invalid_argument("Location beyond dimensions of matrix!");
        }
        m_pMatrix[iRow][iColumn] = dValue;
    }

    void Matrix::clear() {
        int iRow;
        int iColumn;

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            for (iColumn = 0; iColumn < m_iNumColumns; iColumn++) {
                m_pMatrix[iRow][iColumn] = 0.0;
            }
        }
    }

    void Matrix::printMatrix() const {
        int iRow;
        int iColumn;

        for (iRow = 0; iRow < m_iNumRows; iRow++) {
            cout << "[" << m_pMatrix[iRow][0];
            for (iColumn = 1; iColumn < m_iNumColumns; iColumn++) {
                cout << "\t" << m_pMatrix[iRow][iColumn];
            }
            cout << "]\n";
        }
        cout << endl;
    }

    void Matrix::linearSystemSolver(const Matrix& oSourceVector, const PLU_Factorization& oPLU, Matrix& oVoltageVector) {
        int iI1;
        int iI2;
        int iIndex;
        int iNumRows = oSourceVector.m_iNumRows;
        Matrix oX(iNumRows, 1); // UX = Y
        Matrix oY(iNumRows, 1); // LY = B_Permuted
        Matrix oB_Permuted(iNumRows, 1);

        // Apply row permutations to B to get B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (int)round(oPLU.getP()(iI1, 0));
            oB_Permuted(iI1, 0) = oSourceVector(iIndex, 0);
        }

        // Forward substitution to solve LY = B_Permuted
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            oY(iI1, 0) = oB_Permuted(iI1, 0);
            for (iI2 = 0; iI2 < iI1; ++iI2) {
                oY(iI1, 0) -= oPLU.getL()(iI1, iI2) * oY(iI2, 0);
            }
        }

        // Backward substitution to solve UX = Y
        for (iI1 = iNumRows - 1; iI1 >= 0; --iI1) {
            oX(iI1, 0) = oY(iI1, 0);
            for (iI2 = iI1 + 1; iI2 < iNumRows; ++iI2) {
                oX(iI1, 0) -= oPLU.getU()(iI1, iI2) * oX(iI2, 0);
            }
            if (fabs(oPLU.getU()(iI1, iI1)) > ZERO) {
                oX(iI1, 0) /= oPLU.getU()(iI1, iI1);
            } else {
                oX(iI1, 0) = 0; // If a diagonal on the U matrix is 0, it's due to the ground node being included in the matrix, and is effectively infinity, resulting in oX = 0 for this row.
            }
        }

        // Apply column permutations to X using Q to get X_Final
        for (iI1 = 0; iI1 < iNumRows; ++iI1) {
            iIndex = (int)round(oPLU.getQ()(iI1, 0));
            oVoltageVector(iIndex, 0) = oX(iI1, 0);
        }

#ifdef MATRIX_PRINT
        cout << "X Vector:" << endl;
        oX.printMatrix();
        cout << "Y Vector:" << endl;
        oY.printMatrix();
        cout << "B Permuted Vector:" << endl;
        oB_Permuted.printMatrix();
#endif
    }

    // Function to perform PLU factorization with row and column pivoting
    // Factors a matrix into P*A*Q = L*U, where P = Row Permutation Matrix, and Q = Column Permutation Matrix
    void PLU_Factorization::runPLU_Factorization(const Matrix& oConductanceMatrix) {
        int iRows = oConductanceMatrix.getNumRows();
        int iColumns = oConductanceMatrix.getNumColumns();
        double dMaxVal;
        int iMaxRow;
        int iMaxCol;
        int iI1;
        int iI2;
        int iI3;

        if ((iRows == 0) || (iColumns == 0)) {
            cout << "Cannot factor a matrix with dimensions of zero!" << endl;
            throw invalid_argument("Cannot factor a matrix with dimensions of zero!");
        }

        // Initialize P and Q as identity permutations (single column matrices)
        m_pP = make_unique<Matrix>(iRows, 1);
        m_pQ = make_unique<Matrix>(iRows, 1);
        m_pL = make_unique<Matrix>(iRows, iRows);
        m_pU = make_unique<Matrix>(oConductanceMatrix); // Initialize U to be a copy of A

        // Initialize L to an idenity matrix
        for (iI1 = 0; iI1 < iRows; ++iI1) {
            (*m_pL)(iI1, iI1) = 1.0;
        }

        for (iI1 = 0; iI1 < iRows; ++iI1) {
            (*m_pP)(iI1, 0) = iI1;
            (*m_pQ)(iI1, 0) = iI1;
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