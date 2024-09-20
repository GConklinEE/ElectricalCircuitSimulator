#pragma once

#include <complex>
#include <memory>
#include <sstream>
#include <string>

namespace SimulationEngine {

    template<typename T>
    concept Numeric = std::is_arithmetic_v<T> ||
        requires(T t)
    {
        typename T::value_type;
        requires std::same_as<T, std::complex<typename T::value_type>>;
        requires std::is_arithmetic_v<typename T::value_type>;
    };

    template<Numeric T>
    class Matrix final {

        public:

            #pragma region Constructors and Destructors

            Matrix(const size_t iNumRows = 1, const size_t iNumColumns = 1) :
                m_iNumRows(iNumRows),
                m_iNumColumns(iNumColumns),
                m_pData(std::make_unique< std::unique_ptr<T[]>[]>(m_iNumRows))
            {
                size_t iRowIndex;

                if (iNumRows == 0 || iNumColumns == 0)
                    throw std::invalid_argument("Matrix dimensions must be positive and non-zero!");

                for (iRowIndex = 0; iRowIndex < m_iNumRows; ++iRowIndex)
                    m_pData[iRowIndex] = make_unique<T[]>(m_iNumColumns);
            }

            Matrix(const Matrix& oOriginal) :
                Matrix(oOriginal.m_iNumRows, oOriginal.m_iNumColumns)
            {
                size_t iRowIndex;
                size_t iColumnIndex;

                for (iRowIndex = 0; iRowIndex < m_iNumRows; ++iRowIndex) {
                    for (iColumnIndex = 0; iColumnIndex < m_iNumColumns; ++iColumnIndex) {
                        m_pData[iRowIndex][iColumnIndex] = oOriginal.m_pData[iRowIndex][iColumnIndex];
                    }
                }
            }

            Matrix(Matrix&&) = default;
            ~Matrix() = default;

            #pragma endregion

            #pragma region Modifiers

            void swapRows(const size_t iRow1, const size_t iRow2) {
                checkBounds(iRow1, 0);
                checkBounds(iRow2, 0);
                std::swap(m_pData[iRow1], m_pData[iRow2]);
            }

            void swapValues(const size_t iRow1, const size_t iColumn1, const size_t iRow2, const size_t iColumn2) {
                checkBounds(iRow1, iColumn1);
                checkBounds(iRow2, iColumn2);
                std::swap(m_pData[iRow1][iColumn1], m_pData[iRow2][iColumn2]);
            }

            void clear() {
                size_t iRowIndex;
                size_t iColumnIndex;

                for (iRowIndex = 0; iRowIndex < m_iNumRows; ++iRowIndex) {
                    for (iColumnIndex = 0; iColumnIndex < m_iNumColumns; ++iColumnIndex) {
                        m_pData[iRowIndex][iColumnIndex] = T{};
                    }
                }
            }

            std::string getMatrixString() const {
                size_t iRowIndex;
                size_t iColumnIndex;
                std::stringstream stream;

                for (iRowIndex = 0; iRowIndex < m_iNumRows; ++iRowIndex) {
                    stream << '[';
                    for (iColumnIndex = 0; iColumnIndex < m_iNumColumns; ++iColumnIndex) {
                        stream << '\t' << m_pData[iRowIndex][iColumnIndex];
                    }
                    stream << "\t]\n";
                }
                stream << std::endl;

                return stream.str();
            }

            #pragma endregion

            #pragma region Observers

            size_t getNumRows() const {
                return m_iNumRows;
            }

            size_t getNumColumns() const {
                return m_iNumColumns;
            }

            #pragma endregion

            #pragma region Operators

            #pragma region Modifiers

            T& operator()(const size_t iRow = 0, const size_t iColumn = 0) {
                checkBounds(iRow, iColumn);
                return m_pData[iRow][iColumn];
            }

            Matrix& operator=(const Matrix& oOriginal) {
                size_t iRowIndex;
                size_t iColumnIndex;
                std::unique_ptr<T[]> pRow;
                m_iNumRows(oOriginal.m_iNumRows);
                m_iNumColumns(oOriginal.m_iNumColumns);
                m_pData(std::make_unique< std::unique_ptr<std::unique_ptr<T[]>[]>>(m_iNumRows));

                for (iRowIndex = 0; iRowIndex < m_iNumRows; ++iRowIndex) {
                    pRow = make_unique<T[]>(m_iNumColumns);
                    for (iColumnIndex = 0; iColumnIndex < m_iNumColumns; ++iColumnIndex) {
                        pRow[iColumnIndex] = oOriginal.m_pData[iRowIndex][iColumnIndex];
                    }
                    m_pData[iRowIndex] = pRow;
                }
            }

            Matrix& operator=(Matrix&&) = default;

            #pragma endregion

            #pragma region Observers

            const T& operator()(const size_t iRow = 0, const size_t iColumn = 0) const {
                checkBounds(iRow, iColumn);
                return m_pData[iRow][iColumn];
            }

            #pragma endregion

            #pragma endregion

        private:

            #pragma region Members

            size_t m_iNumRows;
            size_t m_iNumColumns;
            std::unique_ptr<std::unique_ptr<T[]>[]> m_pData; // Outer pointer is rows, inner pointer is columns

            #pragma endregion

            #pragma region Observers

            inline void checkBounds(size_t iRow, size_t iColumn) const {
                if (iRow >= m_iNumRows || iColumn >= m_iNumColumns) {
                    throw std::invalid_argument("Location beyond dimensions of matrix!");
                }
            }

            #pragma endregion
    };

}