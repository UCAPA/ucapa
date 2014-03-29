/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 UCAPA Team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef UCAPA_MATRIX_H
#define UCAPA_MATRIX_H

#include <assert.h>
#include <stdexcept>

#include <vector3.h>

namespace ucapa{
    template<typename T>
    /**
     * @brief The Matrix class provide the use of template matrix
     */
    class UCAPA_API Matrix
    {
    protected:
        int m_nRow; ///< Number of lines
        int m_nCol; ///< Number of columns
        T** m_pdData; ///< Array of matrix values.

        /**
         * @brief Allocate memory for a matrix
         * @param lig Number of line of the matrix
         * @param col Number of columns of the matrix
         */
        void alloc(int lig, int col);
        template<typename C>
        /**
         * @brief Copy the content of the matrix passed in parameter into the current matrix
         * @param mat Matrix to copy
         */
        void copy(const Matrix<C>& mat);
        /**
         * @brief Release the memory used by the matrix
         */
        void destroy();

    public:
        /**
         * @brief Construct a Matrix with the specified number of lines and columns, by default set to 0
         * @param nbRow Number of lines of the matrix
         * @param nbCol Number of columns of the matrix
         */
        Matrix(int nbRow = 0, int nbCol = 0);
        /**
         * @brief Copy Constructors
         * @param mat Matrix to copy
         */
        Matrix(const Matrix& mat);
        ~Matrix(void);

        /**
         * @brief Set matrix to zero
         * @return Matrix set to 0
         */
        Matrix& Zero();
        /**
         * @brief Set the matrix to the identity (or to the nearest matrix for a non-square matrix)
         * @return Matrix set to identity
         */
        Matrix& setToIdentity();

        /**
         * @brief Get the number of lines
         * @return number of lines
         */
        int getNbRows() const { return m_nRow; }
        /**
         * @brief Get the number of columns
         * @return number of colums
         */
        int getNbCols() const { return m_nCol; }

        //Operators
        Matrix& operator= (const Matrix &mat);
        template<typename C> Matrix& operator= (const Matrix<C> &mat);
        // Acces operators
        T& operator() (int i, int j);
        const T& operator() (int i, int j) const;
        // Get the column i of the matrix in an other matrix (m_nRow, 1)
        Matrix<T> operator[] (int i) const;
        // Math operators
        Matrix operator-() const;
        Matrix& operator+= (const Matrix &mat);
        Matrix& operator-= (const Matrix &mat);
        Matrix operator+ (const Matrix &mat) const;
        Matrix operator- (const Matrix &mat) const;
        Matrix operator* (const Matrix &mat) const;
        // Conparison operators
        bool operator== (const Matrix &mat) const;
        bool operator!= (const Matrix &mat) const;

        /**
         * @brief Compute the transpose of the matrix
         * @return The transpose of the matrix
         */
        Matrix transponate() const;

        /**
         * @brief Swap two rows
         * @param row1 index of the first row to swap
         * @param row2 index of the second row to swap
         */
        void swapRows(int row1, int row2);

        /**
         * @brief Compute the reduce of the matrix
         * @return The reduce of the current matrix
         */
        Matrix reduce();

        template<typename T2>
        friend Vector3 operator* (const Matrix<T2>& mat, const Vector3& vec);
    };




    template<typename T>
    Matrix<T>::Matrix(int lig, int col)
        : m_pdData(NULL)
    {
        alloc(lig, col);
    }


    template<typename T>
    Matrix<T>::Matrix(const Matrix& mat)
        : m_pdData(NULL)
    {
        alloc(mat.m_nRow, mat.m_nCol);
        copy(mat);
    }


    template<typename T>
    Matrix<T>::~Matrix(void)
    {
        destroy();
    }


    template<typename T>
    void Matrix<T>::alloc(int lig, int col)
    {
        assert(m_pdData == NULL);
        m_nRow = lig > 0 ? lig : 0;
        m_nCol = col > 0 ? col : 0;

        m_pdData = new T*[m_nRow];
        for(int i=0; i<m_nRow; ++i)
            m_pdData[i] = new T[m_nCol];
    }


    template<typename T>
    template<typename C>
    void Matrix<T>::copy(const Matrix<C>& mat)
    {
        assert(m_nRow == mat.getNbRows() && m_nCol == mat.getNbCols());
        for(int i=0; i<m_nRow; ++i)
            for(int j=0; j<m_nCol; ++j)
                m_pdData[i][j] = (T) mat(i, j);
    }


    template<typename T>
    void Matrix<T>::destroy(void)
    {
        for(int i=0; i<m_nRow; ++i)
            delete[] m_pdData[i];
        delete[] m_pdData;
        m_nRow = m_nCol = 0;
        m_pdData = NULL;
    }


    template<typename T>
    Matrix<T>& Matrix<T>::Zero(void)
    {
        for(int i=0; i<m_nRow; ++i)
            for(int j=0; j<m_nCol; ++j)
                m_pdData[i][j] = 0;
        return *this;
    }


    template<typename T>
    Matrix<T>& Matrix<T>::setToIdentity()
    {
        int min = (m_nRow<m_nCol) ? m_nRow : m_nCol;
        Zero();
        for (int i=0; i<min; i++)
            m_pdData[i][i] = 1;
        return *this;
    }



    template<typename T>
    Matrix<T>& Matrix<T>::operator= (const Matrix &mat)
    {
        if(this != &mat)
        {
            destroy();
            alloc(mat.getNbRows(), mat.getNbCols());
            copy(mat);
        }
        return *this;
    }

    template<typename T>
    template<typename C>
    Matrix<T>& Matrix<T>::operator= (const Matrix<C> &mat)
    {
        if(this != dynamic_cast<Matrix<T>*>(&mat))
        {
            destroy();
            alloc(mat.getNbRows(), mat.getNbCols());
            copy(mat);
        }
        return *this;
    }



    template<typename T>
    T& Matrix<T>::operator() (int i, int j)
    {
        if(i>=0 && i<m_nRow && j>=0 && j<m_nCol)
            return m_pdData[i][j];
        else
            throw std::out_of_range("Access to an invalid value of the matrix");
    }


    template<typename T>
    const T& Matrix<T>::operator() (int i, int j) const
    {

        if(i>=0 && i<m_nRow && j>=0 && j<m_nCol)
            return m_pdData[i][j];
        else
            throw std::out_of_range("Access to an invalid value of the matrix");

    }

    template<typename T>
    Matrix<T> Matrix<T>::operator[] (int j) const
    {
        Matrix<T> m(m_nRow, 1);
        for (int i=0; i<m_nRow; i++)
            m(i, 0) = (*this)(i, j);
        return m;
    }



    template<typename T>
    Matrix<T> Matrix<T>::operator-() const
    {
        Matrix<T> tempMat(m_nRow, m_nCol);
        for(int i=0; i<m_nRow; ++i)
        {
            for(int j=0; j<m_nCol; ++j)
                tempMat(i,j) = -(*this)(i,j);
        }

        return tempMat;
    }


    template<typename T>
    Matrix<T>& Matrix<T>::operator+= (const Matrix& mat)
    {
        if (m_nRow == mat.m_nRow && m_nCol == mat.m_nCol)
        {
            for (int i=0 ; i<m_nRow ; ++i) {
                for (int j=0 ; j<m_nCol ; ++j) {
                    m_pdData[i][j] += mat(i,j);
                }
            }
            return *this;
        }
        else
            throw std::length_error("Matrix addition (+=) error. (The two operands haven't a compatible size.)");
    }


    template<typename T>
    Matrix<T> Matrix<T>::operator+ (const Matrix& mat) const
    {
        if (m_nRow == mat.m_nRow && m_nCol == mat.m_nCol)
        {
            Matrix<T> tempMat(mat.m_nRow, mat.m_nCol);
            tempMat.Zero();
            tempMat+=(*this);
            tempMat+=mat;
            return tempMat;
        }
        else
            throw std::length_error("Matrix addition error. (The two operands haven't a compatible size.)");
    }


    template<typename T>
    Matrix<T>& Matrix<T>::operator-= (const Matrix& mat)
    {
        if (m_nRow == mat.m_nRow && m_nCol == mat.m_nCol)
        {
            for (int i=0 ; i<m_nRow ; ++i) {
                for (int j=0 ; j<m_nCol ; ++j) {
                    m_pdData[i][j] -= mat(i,j);
                }
            }
            return *this;
        }
        else
            throw std::length_error("Matrix substraction (-=) error. (The two operands haven't a compatible size.)");
    }


    template<typename T>
    Matrix<T> Matrix<T>::operator- (const Matrix& mat) const
    {
        if (m_nRow == mat.m_nRow && m_nCol == mat.m_nCol)
        {
            Matrix<T> tempMat(mat.m_nRow, mat.m_nCol);
            tempMat.Zero();
            tempMat+=(*this);
            tempMat-=mat;
            return tempMat;
        }
        else
            throw std::length_error("Matrix substraction error. (The two operands haven't a compatible size.)");
    }


    template<typename T>
    Matrix<T> Matrix<T>::operator* (const Matrix &mat) const
    {
        if (mat.m_nRow == m_nCol)
        {
            Matrix<T> tempMat(m_nRow, mat.m_nCol);
            tempMat.Zero();
            for(int i = 0 ; i< m_nRow ; ++i) {
                for (int j=0; j < mat.m_nCol; ++j) {
                    for (int k = 0; k < m_nCol; ++k) {
                        tempMat(i, j) += (*this)(i, k) * mat(k, j);
                    }
                }
            }
            return tempMat;
        }
        else
            throw std::length_error("Matrix multiplication error. (The two operands haven't a compatible size.)");
    }


    template<typename T>
    bool Matrix<T>::operator== (const Matrix &mat) const
    {
        if (m_nRow == mat.m_nRow && m_nCol == mat.m_nCol)
        {
            for (int i=0 ; i<m_nRow ; ++i) {
                for (int j=0 ; j<m_nCol ; ++j) {
                    if ( mat(i,j) != m_pdData[i][j] )
                        return false;
                }
            }
            return true;
        }
        else
            return false;
    }

    template<typename T>
    bool Matrix<T>::operator!= (const Matrix &mat) const
    {
        return !( (*this) == mat );
    }


    template<typename T>
    Matrix<T> Matrix<T>::transponate() const
    {
        Matrix<T> mat(m_nCol, m_nRow);
        for(int i = 0 ; i<m_nRow ; ++i) {
                for (int j=0; j < m_nCol; ++j) {
                    mat(j, i)=(*this)(i, j);
                }
        }
        return mat;
    }


    template<typename T>
    void Matrix<T>::swapRows(int row1, int row2)
    {
        if (row1 == row2)
            return;

        T tmp;
        for (int i=0; i < m_nCol; i++)
        {
            tmp = (*this)(i, row1);
            (*this)(i, row1) = (*this)(i, row2);
            (*this)(i, row2) = tmp;
        }
    }

    template<typename T>
    Matrix<T> Matrix<T>::reduce()
    {
        Matrix<T> reduced(*this);

        int lead = 0;
        for (int row = 0; row < reduced.m_nRow; row++)
        {
            if (reduced.m_nCol <= lead)
                break;

            int i = row;
            while (reduced(lead, i) == 0)
            {
                i++;
                if (i == reduced.m_nRow)
                {
                    i = row;
                    lead++;
                    if (lead == reduced.m_nCol)
                        return reduced;
                }
            }

            reduced.swapRows(i, row);

            T divFactor = reduced(lead, row);
            for (int k = 0; k < reduced.m_nCol; k++)
            {
                reduced(k, row) = reduced(k, row) / divFactor;
            }

            for (int j = 0; j < reduced.m_nRow; j++)
            {
                if (j == row) continue;

                T factor = reduced(lead, j);
                for (int k = 0; k < reduced.m_nCol; k++)
                {
                    reduced(k, j) = reduced(k, j) - reduced(k, row) * factor;
                }
            }

        }
        return reduced;
    }


    template<typename T2>
    Vector3 operator* (const Matrix<T2>& mat, const Vector3& vec)
    {
        if (mat.m_nRow != 4 || mat.m_nCol != 4)
            throw std::invalid_argument("Matrix and Vector3 multiplication error. Matrix must have the size (4, 4)");

        return Vector3(mat(0, 0)*vec.x + mat(0, 1)*vec.y + mat(0, 2)*vec.z,
                       mat(1, 0)*vec.x + mat(1, 1)*vec.y + mat(1, 2)*vec.z,
                       mat(2, 0)*vec.x + mat(2, 1)*vec.y + mat(2, 2)*vec.z);
    }
}


#endif // UCAPA_MATRIX_H
