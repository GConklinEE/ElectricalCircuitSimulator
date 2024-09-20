#pragma once

#include "Matrix.h"
#include <iostream>

namespace SimulationEngine {

    // Represents a matrix factored into P*A*Q = L*U, where P = Row Permutation Matrix,
    // and Q = Column Permutation Matrix
    template<Numeric T>
    class PLU_Factorization final {

        public:

            #pragma region Constructors and Destructors

            PLU_Factorization(const Matrix<T>& oA = Matrix<T>{}) :
                m_oA(oA),
                m_oL(m_oA.getNumRows(), m_oA.getNumRows()),
                m_oU(m_oA),
                m_oP(m_oA.getNumRows(), 1),
                m_oQ(m_oA.getNumRows(), 1)
            {
              runPLU_Factorization();
            }

            #pragma endregion

            #pragma region Observers

            const Matrix<T>& getL() const {
                return m_oL;
            }

            const Matrix<T>& getU() const {
                return m_oU;
            }

            const Matrix<size_t>& getP() const {
                return m_oP;
            }

            const Matrix<size_t>& getQ() const {
                return m_oQ;
            }

            Matrix<T> solve(const Matrix<T>& oB) const {
                static const T uEPSILON = 1e-9;

                size_t iNumRows = oB.getNumRows();
                size_t iRowIndex1;
                size_t iRowIndex2;
                T uDiagonal;
                Matrix<T> oX(iNumRows);
                Matrix<T> oY(iNumRows);
                Matrix<T> oB_Permuted(iNumRows);
                Matrix<T> oSolution(iNumRows);

                // Apply row permutations to B to get B_Permuted
                for (iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                    oB_Permuted(iRowIndex1) = oB(m_oP(iRowIndex1));
                }

                // Forward substitution to solve LY = B_Permuted
                for (iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                    oY(iRowIndex1) = oB_Permuted(iRowIndex1);
                    for (iRowIndex2 = 0; iRowIndex2 < iRowIndex1; ++iRowIndex2) {
                        oY(iRowIndex1) -= m_oL(iRowIndex1, iRowIndex2)* oY(iRowIndex2);
                    }
                }

                // Backward substitution to solve UX = Y
                for (iRowIndex1 = iNumRows - 1; iRowIndex1-->0;) {
                    oX(iRowIndex1) = oY(iRowIndex1);
                    for (iRowIndex2 = iRowIndex1 + 1; iRowIndex2 < iNumRows; ++iRowIndex2) {
                        oX(iRowIndex1) -= m_oU(iRowIndex1, iRowIndex2) * oX(iRowIndex2);
                    }

                    uDiagonal = m_oU(iRowIndex1, iRowIndex1);
                    // If a diagonal on the U matrix is 0, it's due to the ground node being included in the matrix,
                    // and is effectively infinity, resulting in X = 0 for this row.
                    oX(iRowIndex1) = (uDiagonal > uEPSILON) || (uDiagonal < -uEPSILON) ? oX(iRowIndex1) / uDiagonal : T{};
                }

                // Apply column permutations to X using Q to get Solution
                for (iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                    oSolution(m_oQ(iRowIndex1)) = oX(iRowIndex1);
                }

#ifdef MATRIX_PRINT
                std::cout << "X Vector:" << std::endl;
                std::cout << oX.getMatrixString();
                std::cout << "Y Vector:" << std::endl;
                std::cout << oY.getMatrixString();
                std::cout << "B Permuted Vector:" << std::endl;
                std::cout << oB_Permuted.getMatrixString();
#endif

                return oSolution;
            }

            #pragma endregion

        private:

            #pragma region Members

            Matrix<T> m_oA; //Matrix to decompose
            Matrix<T> m_oL; //Lower triangular matrix
            Matrix<T> m_oU; //Upper triangular matrix
            Matrix<size_t> m_oP; //Row permuation matrix
            Matrix<size_t> m_oQ; //Column permutation matrix

            #pragma endregion

            #pragma region Functions

            void runPLU_Factorization() {
                size_t iNumRows = m_oA.getNumRows();
                size_t iRowIndex1;
                size_t iRowIndex2;
                size_t iRowIndex3;
                size_t iMaxRow;
                size_t imaxColumn;
                T uAbsoluteValue;
                T uMaxValue;

                //Initialize L to an identity matrix
                for (size_t iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                    m_oL(iRowIndex1, iRowIndex1) = 1;
                }

                for (iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                    m_oP(iRowIndex1, 0) = m_oQ(iRowIndex1, 0) = iRowIndex1;
                }

                for (iRowIndex3 = 0; iRowIndex3 < iNumRows; ++iRowIndex3) {
                    // Find the pivot (maximum element) in the submatrix U[iRowIndex3:iNumRows][iRowIndex3:iNumRows]
                    uMaxValue = T{};
                    iMaxRow = iRowIndex3;
                    imaxColumn = iRowIndex3;
                    for (iRowIndex1 = iRowIndex3; iRowIndex1 < iNumRows; ++iRowIndex1) {
                        for (iRowIndex2 = iRowIndex3; iRowIndex2 < iNumRows; ++iRowIndex2) {
                            uAbsoluteValue = (m_oU(iRowIndex1, iRowIndex2) < 0) ? -m_oU(iRowIndex1, iRowIndex2) : m_oU(iRowIndex1, iRowIndex2);
                            if (uAbsoluteValue > uMaxValue) {
                                uMaxValue = uAbsoluteValue;
                                iMaxRow = iRowIndex1;
                                imaxColumn = iRowIndex2;
                            }
                        }
                    }

                    // Swap rows in U to move the pivot to position (iRowIndex3, iRowIndex3)
                    m_oU.swapRows(iRowIndex3, iMaxRow);
                    m_oP.swapRows(iRowIndex3, iMaxRow);

                    // Swap columns in U to move the pivot to position (iRowIndex3, iRowIndex3)
                    for (iRowIndex1 = 0; iRowIndex1 < iNumRows; ++iRowIndex1) {
                        m_oU.swapValues(iRowIndex1, iRowIndex3, iRowIndex1, imaxColumn);
                    }

                    m_oQ.swapRows(iRowIndex3, imaxColumn);

                    // Update L for the row swaps
                    for (iRowIndex1 = 0; iRowIndex1 < iRowIndex3; ++iRowIndex1) {
                        m_oL.swapValues(iRowIndex3, iRowIndex1, iMaxRow, iRowIndex1);
                    }

                    // Compute multipliers and update U
                    for (iRowIndex1 = iRowIndex3 + 1; iRowIndex1 < iNumRows; ++iRowIndex1) {
                        m_oL(iRowIndex1, iRowIndex3) = m_oU(iRowIndex1, iRowIndex3) / m_oU(iRowIndex3, iRowIndex3);
                        for (iRowIndex2 = iRowIndex3 + 1; iRowIndex2 < iNumRows; ++iRowIndex2) {
                            m_oU(iRowIndex1, iRowIndex2) = m_oU(iRowIndex1, iRowIndex2) - m_oL(iRowIndex1, iRowIndex3) * m_oU(iRowIndex3, iRowIndex2);
                        }
                        m_oU(iRowIndex1, iRowIndex3) = T{};
                    }
                }
            }

            #pragma endregion
    };

}
