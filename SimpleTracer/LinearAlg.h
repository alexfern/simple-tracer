#pragma once

#include <cmath>
#include <ostream>
#include <algorithm>

constexpr float PI = (float) 3.14159265358979323846;

//Forward Declarations
template<size_t Rows, size_t Columns, typename Type>
struct MatrixData {
	union {
		Type data[Rows * Columns];
		Type data2d[Rows][Columns];
	};

	MatrixData() = default;
};

template<size_t Rows, size_t Columns, typename Type>
struct Matrix;

template<size_t Size, typename Type>
struct VectorData {
	union {
		Type data[Size];
	};

	VectorData() = default;
};

template<size_t Size, typename Type>
struct Vector;

template<size_t Size, typename Type>
struct PointData {
	union {
		Type data[Size];
	};

	PointData() = default;
};

template<size_t Size, typename Type>
struct Point;

template<size_t Size, typename Type>
struct NormalData {
	union {
		Type data[Size];
	};

	NormalData() = default;
};

template<size_t Size, typename Type>
struct Normal;

//Matrices
template<size_t Rows, size_t Columns, typename Type>
struct Matrix : public MatrixData<Rows, Columns, Type> {
	Matrix() = default;

	Matrix(const Type data[Rows * Columns]) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] = data[i];
	}

	Matrix(const Type data2d[Rows][Columns]) {
		for (size_t j = 0; j < Rows; j++)
			for (size_t i = 0; i < Columns; i++)
				this->data2d[j][i] = data2d[j][i];
	}

	template<typename C>
	explicit Matrix(const Matrix<Rows, Columns, C>& other) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] = (Type)other.data[i];
	}

	template<size_t Size, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Matrix(const Vector<Size, Type> & other) :
		Matrix(other.data)
	{}

	template<size_t Size, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Matrix(const Point<Size, Type> & other) :
		Matrix(other.data)
	{}

	template<size_t Size, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Matrix(const Normal<Size, Type> & other) :
		Matrix(other.data)
	{}

	Matrix(std::initializer_list<Type> list) {
		std::copy(list.begin(), list.begin() + std::min((Rows * Columns), list.size()), this->data);
	}

	template<typename... Args, typename E = typename std::enable_if<Rows * Columns == sizeof...(Args)>::type>
	Matrix(Args... args) :
		Matrix({ Type(args)... })
	{}

	Matrix& operator+=(const Matrix& rhs) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] += rhs.data[i];
		return *this;
	}

	friend Matrix operator+(const Matrix & lhs, const Matrix & rhs) {
		return (Matrix(lhs) += rhs);
	}

	Matrix& operator-=(const Matrix & rhs) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] -= rhs.data[i];
		return *this;
	}

	friend Matrix operator-(const Matrix & lhs, const Matrix & rhs) {
		return (Matrix(lhs) -= rhs);
	}

	Matrix& operator*=(Type rhs) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] *= rhs;
		return *this;
	}

	friend Matrix operator*(const Matrix & lhs, Type rhs) {
		return (Matrix(lhs) *= rhs);
	}

	friend Matrix operator*(Type lhs, const Matrix & rhs) {
		return (rhs * lhs);
	}

	friend Matrix operator/(const Matrix & lhs, Type rhs) {
		return (Matrix(lhs) /= rhs);
	}

	Matrix& operator/=(Type rhs) {
		Type inv = 1 / rhs;
		return (*this *= inv);
	}

	Matrix operator-() const {
		return (Matrix() -= *this);
	}

	friend bool operator==(const Matrix & lhs, const Matrix & rhs) {
		for (size_t i = 0; i < Rows * Columns; i++)
			if (lhs.data[i] != rhs.data[i])
				return false;
		return true;
	}

	friend bool operator!=(const Matrix & lhs, const Matrix & rhs) {
		return !(lhs == rhs);
	}

	Matrix& operator=(const Matrix & rhs) {
		for (size_t i = 0; i < Rows * Columns; i++)
			this->data[i] = rhs.data[i];
		return *this;
	}

	//Matrix Multiplication
	template<size_t C>
	friend Matrix<Rows, C, Type> operator*(const Matrix& lhs, const Matrix<Columns, C, Type>& rhs) {
		Matrix<Rows, C, Type> result = Matrix<Rows, C, Type>();
		for (size_t i = 0; i < Rows; i++) {
			for (size_t j = 0; j < C; j++) {
				Type sum = (Type)0;
				for (size_t k = 0; k < Columns; k++) {
					sum += lhs.data2d[i][k] * rhs.data2d[k][j];
				}
				result.data2d[i][j] = sum;
			}
		}
		return result;
	}

	friend Matrix abs(const Matrix& mat) {
		Matrix<Rows, Columns, Type> result = Matrix<Rows, Columns, Type>();
		for (size_t i = 0; i < Rows * Columns; i++)
			result.data[i] = std::abs(mat.data[i]);
		return result;
	}

	template<size_t C>
	friend Matrix<Rows, C, Type> mul(const Matrix& lhs, const Matrix<Columns, C, Type>& rhs) {
		return lhs * rhs;
	}

	Type* operator[](size_t row) {
		return this->data2d[row];
	}

	const Type* operator[](size_t row) const {
		return this->data2d[row];
	}

	//Copies the data from the specified matrix into this matrix starting at the specified row and column (returning itself for chaining)
	template<size_t R, size_t C, typename E = typename std::enable_if<R <= Rows && C <= Columns>::type>
	Matrix& put(size_t row, size_t column, const Matrix<R, C, Type>& src) {
		for (size_t rowI = 0; rowI < R; rowI++)
			for (size_t columnJ; columnJ < C; columnJ++)
				this->data2d[rowI + row][columnJ + column] = src.data2d[rowI][columnJ];
		return *this;
	}

	template<size_t R, size_t C, typename E = typename std::enable_if<R <= Rows && C <= Columns>::type>
	Matrix<R, C, Type>& pull(size_t row, size_t column, Matrix<R, C, Type>& dest) const {
		for (size_t rowI = 0; rowI < R; rowI++)
			for (size_t columnJ; columnJ < C; columnJ++)
				dest.data2d[rowI][columnJ] = this->data2d[rowI + row][columnJ + column];
		return dest;
	}
	template<size_t R, size_t C, typename E = typename std::enable_if<R <= Rows && C <= Columns>::type>
	Matrix<R, C, Type> pull(size_t row, size_t column) const {
		Matrix<R, C, Type> dest = Matrix<R, C, Type>();
		return pull(row, column, dest);
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Matrix& rhs) {
		if (Rows == 0 || Columns == 0) {
			lhs << "[[]]";
			return lhs;
		}
		lhs << "[";
		lhs << "[" << rhs.data2d[0][0];
		for (size_t j = 1; j < Columns; j++)
			lhs << ", " << rhs.data2d[0][j];
		lhs << "]";
		for (size_t i = 1; i < Rows; i++) {
			lhs << "," << std::endl << " [" << rhs.data2d[i][0];
			for (size_t j = 1; j < Columns; j++)
				lhs << ", " << rhs.data2d[i][j];
			lhs << "]";
		}
		lhs << "]";
		return lhs;
	}

	friend Matrix<Columns, Rows, Type> transpose(const Matrix& mat) {
		Matrix<Columns, Rows, Type> transposed = Matrix<Columns, Rows, Type>();
		for (size_t j = 0; j < Rows; j++)
			for (size_t i = 0; i < Columns; i++)
				transposed.data2d[i][j] = mat.data2d[j][i];
		return transposed;
	}

	friend Matrix<Columns, Rows, Type> T(const Matrix& mat) {
		return transpose(mat);
	}

	template<typename E = typename std::enable_if<Rows == Columns>::type>
	static Matrix identity() {
		Matrix mat = Matrix();
		for (size_t i = 0; i < Rows; i++)
			mat.data2d[i][i] = 1;
		return mat;
	}

	template<typename E = typename std::enable_if<Rows == Columns>::type>
	static Matrix I() {
		return identity();
	}
};

//Vectors
template<size_t Size, typename Type>
struct Vector : public VectorData<Size, Type> {
	Vector() = default;

	Vector(const Type data[3]) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = data[i];
	}

	template<typename C>
	explicit Vector(const Vector<Size, C> & other) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = (Type)other.data[i];
	}

	template<size_t Rows, size_t Columns, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Vector(const Matrix<Rows, Columns, Type> & other) :
		Vector(other.data)
	{}

	explicit Vector(const Point<Size, Type>& other) :
		Vector(other.data)
	{}

	explicit Vector(const Normal<Size, Type>& other) :
		Vector(other.data)
	{}

	Vector(std::initializer_list<Type> list) {
		std::copy(list.begin(), list.begin() + std::min((Size), list.size()), this->data);
	}

	template<typename... Args, typename E = typename std::enable_if<Size == sizeof...(Args)>::type>
	Vector(Args... args) :
		Vector({ Type(args)... })
	{}

	Vector& operator+=(const Vector& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] += rhs.data[i];
		return *this;
	}

	friend Vector operator+(const Vector& lhs, const Vector& rhs) {
		return (Vector(lhs) += rhs);
	}

	Vector& operator-=(const Vector& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] -= rhs.data[i];
		return *this;
	}

	friend Vector operator-(const Vector& lhs, const Vector& rhs) {
		return (Vector(lhs) -= rhs);
	}

	Vector& operator*=(Type rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] *= rhs;
		return *this;
	}

	friend Vector operator*(const Vector& lhs, Type rhs) {
		return (Vector(lhs) *= rhs);
	}

	friend Vector operator*(Type lhs, const Vector& rhs) {
		return (rhs * lhs);
	}

	friend Vector operator/(const Vector& lhs, Type rhs) {
		return (Vector(lhs) /= rhs);
	}

	Vector& operator/=(Type rhs) {
		Type inv = 1 / rhs;
		return (*this *= inv);
	}

	Vector operator-() const {
		return (Vector() -= *this);
	}

	friend bool operator==(const Vector& lhs, const Vector& rhs) {
		for (size_t i = 0; i < Size; i++)
			if (lhs.data[i] != rhs.data[i])
				return false;
		return true;
	}

	friend bool operator!=(const Vector& lhs, const Vector& rhs) {
		return !(lhs == rhs);
	}

	Vector& operator=(const Vector& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = rhs.data[i];
		return *this;
	}

	//Dot Product
	friend Type operator*(const Vector& rhs, const Vector& lhs) {
		Type sum = Type(0);
		for (size_t i = 0; i < Size; ++i)
			sum += rhs.data[i] * lhs.data[i];
		return sum;
	}

	//Matrix Multiplication
	template<size_t R>
	friend Vector operator*(const Matrix<R, Size, Type>& rhs, const Vector& lhs) {
		return Vector<Size, Type>(rhs * Matrix<Size, 1, Type>(lhs));
	}

	template<size_t R>
	friend Vector mul(const Matrix<R, Size, Type>& rhs, const Vector& lhs) {
		return rhs * lhs;
	}

	Type lengthSq() const {
		return dot(*this, *this);
	}

	template <typename R>
	R length() const {
		return (R)std::sqrt(lengthSq());
	}

	Type length() const {
		return length<Type>();
	}

	friend Type dot(const Vector& a, const Vector& b) {
		return a * b;
	}

	friend Type absDot(const Vector& a, const Vector& b) {
		return std::abs(dot(a, b));
	}

	friend Vector abs(const Vector& vec) {
		Vector<Size, Type> result = Vector<Size, Type>();
		for (size_t i = 0; i < Size; i++)
			result.data[i] = std::abs(vec.data[i]);
		return result;
	}

	friend Vector normalize(const Vector & vec) {
		const Type length = vec.length();
		if (length == 0) {
			return vec;
		}
		return (vec / length);
	}

	//[i] operator returns the i-th element
	const Type& operator[](size_t i) const {
		return this->data[i];
	}

	Type& operator[](size_t i) {
		return this->data[i];
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Vector<Size, Type>& rhs) {
		if (Size == 0) {
			lhs << "[]";
			return rhs;
		}
		lhs << "[" << rhs.data[0];
		for (size_t i = 1; i < Size; i++)
			lhs << ", " << rhs.data[i];
		lhs << "]";
		return lhs;
	}
};


//Points
template<size_t Size, typename Type>
struct Point : public PointData<Size, Type> {
	Point() = default;

	Point(const Type data[Size]) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = data[i];
	}

	template<typename C>
	explicit Point(const Point<Size, C> & other) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = (Type)other.data[i];
	}

	template<size_t Rows, size_t Columns, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Point(const Matrix<Rows, Columns, Type> & other) :
		Point(other.data)
	{}

	explicit Point(const Vector<Size, Type>& other) :
		Point(other.data)
	{}

	explicit Point(const Normal<Size, Type>& other) :
		Point(other.data)
	{}

	Point(std::initializer_list<Type> list) {
		std::copy(list.begin(), list.begin() + std::min((Size), list.size()), this->data);
	}

	template<typename... Args, typename E = typename std::enable_if<Size == sizeof...(Args)>::type>
	Point(Args... args) :
		Point({ Type(args)... })
	{}

	Point& operator+=(const Point& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] += rhs.data[i];
		return *this;
	}

	Point& operator+=(const Vector<Size, Type>& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] += rhs.data[i];
		return *this;
	}

	friend Point operator+(const Point& lhs, const Point& rhs) {
		return (Point(lhs) += rhs);
	}

	friend Point operator+(const Point& lhs, const Vector<Size, Type>& rhs) {
		return (Point(lhs) += rhs);
	}

	friend Point operator+(const Vector<Size, Type>& lhs, const Point& rhs) {
		return (Point(rhs) += lhs);
	}

	Point& operator-=(const Vector<Size, Type>& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] -= rhs.data[i];
		return *this;
	}

	friend Point operator-(const Point& lhs, const Vector<Size, Type>& rhs) {
		return (Point(lhs) -= rhs);
	}

	friend Vector<Size, Type> operator-(const Point& lhs, const Point& rhs) {
		return (Vector<Size, Type>(lhs) -= Vector<Size, Type>(rhs));
	}

	Point& operator*=(Type rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] *= rhs;
		return *this;
	}

	friend Point operator*(const Point& lhs, Type rhs) {
		return (Point(lhs) *= rhs);
	}

	friend Point operator*(Type lhs, const Point& rhs) {
		return (rhs * lhs);
	}

	friend Point operator/(const Point& lhs, Type rhs) {
		return (Point(lhs) /= rhs);
	}

	Point& operator/=(Type rhs) {
		Type inv = 1 / rhs;
		return (*this *= inv);
	}

	Point operator-() const {
		return (Point() -= *this);
	}

	friend bool operator==(const Point& lhs, const Point& rhs) {
		for (size_t i = 0; i < Size; i++)
			if (lhs.data[i] != rhs.data[i])
				return false;
		return true;
	}

	friend bool operator!=(const Point& lhs, const Point& rhs) {
		return !(lhs == rhs);
	}

	Point& operator=(const Point& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = rhs.data[i];
		return *this;
	}

	//Matrix Multiplication
	template<size_t R>
	friend Point operator*(const Matrix<R, Size, Type>& rhs, const Point& lhs) {
		return Point<Size, Type>(rhs * Matrix<Size, 1, Type>(lhs));
	}

	template<size_t R>
	friend Point mul(const Matrix<R, Size, Type>& rhs, const Point& lhs) {
		return rhs * lhs;
	}

	friend Type distanceSq(const Point& a, const Point& b) {
		return (a - b).lengthSq();
	}

	template<typename R>
	friend R distance(const Point& a, const Point& b) {
		return (R)std::sqrt(distanceSq(a, b));
	}

	friend Type distance(const Point& a, const Point& b) {
		return (Type)std::sqrt(distanceSq(a, b));
	}

	friend Point abs(const Point& poi) {
		Point<Size, Type> result = Point<Size, Type>();
		for (size_t i = 0; i < Size; i++)
			result.data[i] = std::abs(poi.data[i]);
		return result;
	}

	//[i] operator returns the i-th element
	const Type& operator[](size_t i) const {
		return this->data[i];
	}

	Type& operator[](size_t i) {
		return this->data[i];
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Point<Size, Type>& rhs) {
		if (Size == 0) {
			lhs << "[]";
			return rhs;
		}
		lhs << "[" << rhs.data[0];
		for (size_t i = 1; i < Size; i++)
			lhs << ", " << rhs.data[i];
		lhs << "]";
		return lhs;
	}
};

//Normals
template<size_t Size, typename Type>
struct Normal : public NormalData<Size, Type> {
	Normal() = default;

	Normal(const Type data[Size]) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = data[i];
	}

	template<typename C>
	explicit Normal(const Normal<Size, C> & other) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = (Type)other.data[i];
	}

	template<size_t Rows, size_t Columns, typename E = typename std::enable_if<Size == Rows * Columns>::type>
	explicit Normal(const Matrix<Rows, Columns, Type> & other) :
		Normal(other.data)
	{}

	explicit Normal(const Vector<Size, Type>& other) :
		Normal(other.data)
	{}

	explicit Normal(const Point<Size, Type>& other) :
		Normal(other.data)
	{}

	Normal(std::initializer_list<Type> list) {
		std::copy(list.begin(), list.begin() + std::min((Size), list.size()), this->data);
	}

	template<typename... Args, typename E = typename std::enable_if<Size == sizeof...(Args)>::type>
	Normal(Args... args) :
		Normal({ Type(args)... })
	{}

	Normal& operator+=(const Normal& rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] += rhs.data[i];
		return *this;
	}

	friend Normal operator+(const Normal & lhs, const Normal & rhs) {
		return (Normal(lhs) += rhs);
	}

	Normal& operator-=(const Normal & rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] -= rhs.data[i];
		return *this;
	}

	friend Normal operator-(const Normal & lhs, const Normal & rhs) {
		return (Normal(lhs) -= rhs);
	}

	Normal& operator*=(Type rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] *= rhs;
		return *this;
	}

	friend Normal operator*(const Normal & lhs, Type rhs) {
		return (Normal(lhs) *= rhs);
	}

	friend Normal operator*(Type lhs, const Normal & rhs) {
		return (rhs * lhs);
	}

	friend Normal operator/(const Normal & lhs, Type rhs) {
		return (Normal(lhs) /= rhs);
	}

	Normal& operator/=(Type rhs) {
		Type inv = 1 / rhs;
		return (*this *= inv);
	}

	Normal operator-() const {
		return (Normal() -= *this);
	}

	friend bool operator==(const Normal & lhs, const Normal & rhs) {
		for (size_t i = 0; i < Size; i++)
			if (lhs.data[i] != rhs.data[i])
				return false;
		return true;
	}

	friend bool operator!=(const Normal & lhs, const Normal & rhs) {
		return !(lhs == rhs);
	}

	Normal& operator=(const Normal & rhs) {
		for (size_t i = 0; i < Size; i++)
			this->data[i] = rhs.data[i];
		return *this;
	}

	//Dot Product
	friend Type operator*(const Normal & rhs, const Normal& lhs) {
		Type sum = Type(0);
		for (size_t i = 0; i < Size; ++i)
			sum += rhs.data[i] * lhs.data[i];
		return sum;
	}

	//Matrix Multiplication
	template<size_t R>
	friend Normal operator*(const Matrix<R, Size, Type>& rhs, const Normal& lhs) {
		return Normal<Size, Type>(rhs * Matrix<Size, 1, Type>(lhs));
	}

	template<size_t R>
	friend Normal mul(const Matrix<R, Size, Type>& rhs, const Normal& lhs) {
		return rhs * lhs;
	}

	Type lengthSq() const {
		return dot(*this, *this);
	}

	template <typename R>
	R length() const {
		return (R)std::sqrt(lengthSq());
	}

	Type length() const {
		return length<Type>();
	}

	friend Type dot(const Normal& a, const Normal& b) {
		return a * b;
	}

	friend Type absDot(const Normal& a, const Normal& b) {
		return std::abs(dot(a, b));
	}

	friend Type dot(const Normal& a, const Vector<Size, Type>& b) {
		return Vector<Size, Type>(a) * b;
	}

	friend Type absDot(const Normal& a, const Vector<Size, Type>& b) {
		return std::abs(dot(a, b));
	}

	friend Type dot(const Vector<Size, Type>& a, const Normal& b) {
		return a * Vector<Size, Type>(b);
	}

	friend Type absDot(const Vector<Size, Type>& a, const Normal& b) {
		return std::abs(dot(a, b));
	}

	friend Normal abs(const Normal& norm) {
		Normal<Size, Type> result = Normal<Size, Type>();
		for (size_t i = 0; i < Size; i++)
			result.data[i] = std::abs(norm.data[i]);
		return result;
	}

	friend Normal normalize(const Normal& norm) {
		const Type length = norm.length();
		if (length == 0) {
			return norm;
		}
		return (norm / length);
	}

	//[i] operator returns the i-th element
	const Type& operator[](size_t i) const {
		return this->data[i];
	}

	Type& operator[](size_t i) {
		return this->data[i];
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Normal<Size, Type>& rhs) {
		if (Size == 0) {
			lhs << "[]";
			return rhs;
		}
		lhs << "[" << rhs.data[0];
		for (size_t i = 1; i < Size; i++)
			lhs << ", " << rhs.data[i];
		lhs << "]";
		return lhs;
	}
};

template<size_t Size, typename Type>
inline Matrix<Size, 1, Type> asColumnMatrix(const Vector<Size, Type>& vec) {
	return Matrix<Size, 1, Type>(vec);
}

template<size_t Size, typename Type>
inline Matrix<1, Size, Type> asRowMatrix(const Vector<Size, Type>& vec) {
	return Matrix<1, Size, Type>(vec);
}

template<size_t Size, typename Type>
inline Matrix<Size, 1, Type> asColumnMatrix(const Point<Size, Type>& poi) {
	return Matrix<Size, 1, Type>(poi);
}

template<size_t Size, typename Type>
inline Matrix<1, Size, Type> asRowMatrix(const Point<Size, Type>& poi) {
	return Matrix<1, Size, Type>(poi);
}

template<size_t Size, typename Type>
inline Matrix<Size, 1, Type> asColumnMatrix(const Normal<Size, Type>& norm) {
	return Matrix<Size, 1, Type>(norm);
}

template<size_t Size, typename Type>
inline Matrix<1, Size, Type> asRowMatrix(const Normal<Size, Type>& norm) {
	return Matrix<1, Size, Type>(norm);
}

//Inline/Static Functions
template<size_t Size, typename Type>
inline Type det(const Matrix<Size, Size, Type>& mat);

template<size_t Size, typename Type>
inline Type cofactor(const Matrix<Size, Size, Type>& mat, size_t row, size_t column) {
	Matrix<Size - 1, Size - 1, Type> subMat = Matrix<Size - 1, Size - 1, Type>();
	for (size_t i = 0; i < Size - 1; i++) {
		for (size_t j = 0; j < Size - 1; j++) {
			size_t r = i < row ? i : j + 1;
			size_t c = j < column ? j : j + 1;
			subMat[i][j] = mat[r][c];
		}
	}
	size_t sign = (row % 2 ? 1 : -1) * (column % 2 ? 1 : -1);
	return sign * det(subMat);
}

template<size_t Size, typename Type>
struct DetImpl {
	static Type det(const Matrix<Size, Size, Type>& mat) {
		Type sum = (Type)0;
		for (size_t i = 0; i < Size; i++)
			sum += mat[0][i] * cofactor(mat, 0, i);
		return sum;
	}
};

template<typename Type>
struct DetImpl<1, Type> {
	static Type det(const Matrix<1, 1, Type>& mat) {
		return mat[0][0];
	}
};

template<size_t Size, typename Type>
inline Type det(const Matrix<Size, Size, Type>& mat) {
	return DetImpl<Size, Type>::det(mat);
}

template<size_t Size, typename Type>
inline Matrix<Size, Size, Type> adj(const Matrix<Size, Size, Type>& mat) {
	Matrix<Size, Size, Type> result = Matrix<Size, Size, Type>();
	for (size_t i = 0; i < Size; i++)
		for (size_t j = 0; j < Size; j++)
			result[i][j] = cofactor(mat, i, j);
	return transpose(result);
}

template<size_t Size, typename Type>
inline Matrix<Size, Size, Type> inv(const Matrix<Size, Size, Type>& mat) {
	return adj(mat) / det(mat);
}

//Cross Product for n-th dimensional vectors using n-1 vectors/Matrixes
template<size_t Size, typename Type, typename... Args, typename E = typename std::enable_if<Size - 2 == sizeof...(Args)>::type>
inline Vector<Size, Type> cross(Vector<Size, Type> vec, Args... args) {
	Matrix<Size, Size, Type> mat = Matrix<Size, Size, Type>();
	populateCrossMat(mat, 0, vec, args...);
	Vector<Size, Type> result = Vector<Size, Type>();
	for (size_t i = 0; i < Size; ++i)
		result.data[i] = cofactor(mat, 0, i);
	return result;
}

template<size_t Size, typename Type, typename... Args, typename E = typename std::enable_if<Size - 2 == sizeof...(Args)>::type>
inline Vector<Size, Type> cross(Normal<Size, Type> norm, Args... args) {
	return cross(Vector<Size, Type>(norm), args...);
}

template<size_t Size, typename Type>
inline void populateCrossMat(Matrix<Size, Size, Type>& mat, size_t row, Vector<Size, Type> vec) {
	for (size_t i = 0; i < Size; ++i)
		mat.data2d[row][i] = vec.data[i];
}

template<size_t Size, typename Type>
inline void populateCrossMat(Matrix<Size, Size, Type>& mat, size_t row, Normal<Size, Type> norm) {
	for (size_t i = 0; i < Size; ++i)
		mat.data2d[row][i] = norm.data[i];
}

template<size_t Size, typename Type, typename... Args>
inline void populateCrossMat(Matrix<Size, Size, Type>& mat, size_t row, Normal<Size, Type> norm, Args... args);

template<size_t Size, typename Type, typename... Args>
inline void populateCrossMat(Matrix<Size, Size, Type>& mat, size_t row, Vector<Size, Type> vec, Args... args) {
	populateCrossMat(mat, row, vec);
	populateCrossMat(mat, row + 1, args...);
}

template<size_t Size, typename Type, typename... Args>
inline void populateCrossMat(Matrix<Size, Size, Type>& mat, size_t row, Normal<Size, Type> norm, Args... args) {
	populateCrossMat(mat, row, norm);
	populateCrossMat(mat, row + 1, args...);
}

//Size Specific Data Unions
template<typename Type>
struct MatrixData<2, 2, Type> {
	union {
		Type data[2 * 2];
		Type data2d[2][2];
		struct { Vector<2, Type> a, b; };
	};

	MatrixData() = default;
};

template<typename Type>
struct MatrixData<3, 3, Type> {
	union {
		Type data[3 * 3];
		Type data2d[3][3];
		struct { Vector<3, Type> a, b, c; };
	};

	MatrixData() = default;
};

template<typename Type>
struct MatrixData<4, 4, Type> {
	union {
		Type data[4 * 4];
		Type data2d[4][4];
		struct { Vector<4, Type> a, b, c, d; };
	};

	MatrixData() = default;
};

template<typename Type>
struct VectorData<2, Type> {
	union {
		Type data[2];
		struct { Type x, y; };
	};

	VectorData() = default;
};

template<typename Type>
struct VectorData<3, Type> {
	union {
		Type data[3];
		struct { Type x, y, z; };
		struct { Type x, y, w; };
		Vector<2, Type> xy;
		struct { Type r, g, b; };
	};

	VectorData() = default;
};

template<typename Type>
struct VectorData<4, Type> {
	union {
		Type data[4];
		struct { Type x, y, z, w; };
		Vector<3, Type> xyz;
		Vector<2, Type> xy;
		struct { Type r, g, b, a; };
		Vector<3, Type> rgb;
	};

	VectorData() = default;
};

template<typename Type>
struct PointData<2, Type> {
	union {
		Type data[2];
		struct { Type x, y; };
	};

	PointData() = default;
};

template<typename Type>
struct PointData<3, Type> {
	union {
		Type data[3];
		struct { Type x, y, z; };
		struct { Type x, y, w; };
		Point<2, Type> xy;
		struct { Type r, g, b; };
	};

	PointData() = default;
};

template<typename Type>
struct PointData<4, Type> {
	union {
		Type data[4];
		struct { Type x, y, z, w; };
		Point<3, Type> xyz;
		Point<2, Type> xy;
		struct { Type r, g, b, a; };
		Point<3, Type> rgb;
	};

	PointData() = default;
};

template<typename Type>
struct NormalData<2, Type> {
	union {
		Type data[2];
		struct { Type x, y; };
	};

	NormalData() = default;
};

template<typename Type>
struct NormalData<3, Type> {
	union {
		Type data[3];
		struct { Type x, y, z; };
		struct { Type x, y, w; };
		Normal<2, Type> xy;
		struct { Type r, g, b; };
	};

	NormalData() = default;
};

template<typename Type>
struct NormalData<4, Type> {
	union {
		Type data[4];
		struct { Type x, y, z, w; };
		Normal<3, Type> xyz;
		Normal<2, Type> xy;
		struct { Type r, g, b, a; };
		Normal<3, Type> rgb;
	};

	NormalData() = default;
};

//Typedefs/Aliases
template<size_t Rows, size_t Columns, typename Type>
using Mat = Matrix<Rows, Columns, Type>;

template<typename Type>
using Mat22 = Mat<2, 2, Type>;
using Mat22i = Mat22<int>;
using Mat22f = Mat22<float>;
using Mat22d = Mat22<double>;

template<typename Type>
using Mat33 = Mat<3, 3, Type>;
using Mat33i = Mat33<int>;
using Mat33f = Mat33<float>;
using Mat33d = Mat33<double>;

template<typename Type>
using Mat44 = Mat<4, 4, Type>;
using Mat44i = Mat44<int>;
using Mat44f = Mat44<float>;
using Mat44d = Mat44<double>;

template<size_t Size, typename Type>
using Vec = Vector<Size, Type>;

template<typename Type>
using Vec2 = Vec<2, Type>;
using Vec2i = Vec2<int>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

template<typename Type>
using Vec3 = Vec<3, Type>;
using Vec3i = Vec3<int>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

template<typename Type>
using Vec4 = Vec<4, Type>;
using Vec4i = Vec4<int>;
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

template<size_t Size, typename Type>
using Poi = Point<Size, Type>;

template<typename Type>
using Poi2 = Poi<2, Type>;
using Poi2i = Poi2<int>;
using Poi2f = Poi2<float>;
using Poi2d = Poi2<double>;

template<typename Type>
using Poi3 = Poi<3, Type>;
using Poi3i = Poi3<int>;
using Poi3f = Poi3<float>;
using Poi3d = Poi3<double>;

template<typename Type>
using Poi4 = Poi<4, Type>;
using Poi4i = Poi4<int>;
using Poi4f = Poi4<float>;
using Poi4d = Poi4<double>;

template<size_t Size, typename Type>
using Norm = Normal<Size, Type>;

template<typename Type>
using Norm2 = Norm<2, Type>;
using Norm2i = Norm2<int>;
using Norm2f = Norm2<float>;
using Norm2d = Norm2<double>;

template<typename Type>
using Norm3 = Norm<3, Type>;
using Norm3i = Norm3<int>;
using Norm3f = Norm3<float>;
using Norm3d = Norm3<double>;

template<typename Type>
using Norm4 = Norm<4, Type>;
using Norm4i = Norm4<int>;
using Norm4f = Norm4<float>;
using Norm4d = Norm4<double>;
