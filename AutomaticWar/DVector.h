#pragma once

#include <cmath>

class DVector
{
public:
	double X, Y;

	static const DVector ZeroVector;

	DVector()
		: X(0.0), Y(0.0)
	{
	}

	DVector(double x, double y)
		: X(x), Y(y)
	{
	}

	//DVector operator^(const DVector& V) const
	//{
	//	return DVector
	//	(
	//		Y * V.Z - Z * V.Y,
	//		Z * V.X - X * V.Z,
	//		X * V.Y - Y * V.X
	//	);
	//}

	//static DVector CrossProduct( const DVector& A, const DVector& B )
	//{
	//	return A ^ B;
	//}

	double operator|(const DVector& v) const
	{
		return X*v.X + Y*v.Y;
	}

	static double DotProduct(const DVector& a, const DVector& b)
	{
		return a | b;
	}

	DVector operator+(const DVector& v) const
	{
		return DVector(X + v.X, Y + v.Y);
	}

	DVector operator-(const DVector& v) const
	{
		return DVector(X - v.X, Y - v.Y);
	}

	DVector operator-(double bias) const
	{
		return DVector(X - bias, Y - bias);
	}

	DVector operator+(double bias) const
	{
		return DVector(X + bias, Y + bias);
	}

	DVector operator*(double scale) const
	{
		return DVector(X * scale, Y * scale);
	}

	DVector operator/(double scale) const
	{
		const double rScale = 1.0 / scale;
		return DVector(X * rScale, Y * rScale);
	}

	DVector operator*(const DVector& v) const
	{
		return DVector(X * v.X, Y * v.Y);
	}

	DVector operator/(const DVector& v) const
	{
		return DVector(X / v.X, Y / v.Y);
	}

	bool operator==(const DVector& v) const
	{
		return (X == v.X && Y == v.Y);
	}

	bool operator!=(const DVector& v) const
	{
		return (X != v.X || Y != v.Y);
	}

	//bool Equals(const DVector& V, double Tolerance=KINDA_SMALL_NUMBER) const
	//{
	//	return FMath::Abs(X - V.X) <= Tolerance && FMath::Abs(Y - V.Y) <= Tolerance && FMath::Abs(Z - V.Z) <= Tolerance;
	//}

	DVector operator-() const
	{
		return DVector(-X, -Y);
	}

	DVector operator+=(const DVector& v)
	{
		X += v.X; Y += v.Y;
		//DiagnosticCheckNaN();
		return *this;
	}

	DVector operator-=(const DVector& v)
	{
		X -= v.X; Y -= v.Y;
		//DiagnosticCheckNaN();
		return *this;
	}

	DVector operator*=(double scale)
	{
		X *= scale; Y *= scale;
		//DiagnosticCheckNaN();
		return *this;
	}

	DVector operator/=(double v)
	{
		const double rv = 1.0 / v;
		X *= rv; Y *= rv;
		//DiagnosticCheckNaN();
		return *this;
	}

	//DVector operator*=(const DVector& V)
	//{
	//	X *= V.X; Y *= V.Y; Z *= V.Z;
	//	//DiagnosticCheckNaN();
	//	return *this;
	//}

	//DVector operator/=(const DVector& V)
	//{
	//	X /= V.X; Y /= V.Y; Z /= V.Z;
	//	//DiagnosticCheckNaN();
	//	return *this;
	//}

	// Simple functions.

	void Set(double x, double y)
	{
		X = x;
		Y = y;
		//DiagnosticCheckNaN();
	}

	double Size() const
	{
		return sqrt(X*X + Y*Y);
	}

	double SizeSquared() const
	{
		return X*X + Y*Y;
	}

	bool IsNearlyZero(double tolerance=1.e-8) const
	{
		return (abs(X) <= tolerance && abs(Y) <= tolerance);
	}

	bool IsZero() const
	{
		return (X == 0.0 && Y == 0.0);
	}

	void Normalize()
	{
		const double varSize = Size();
		if (varSize > 1.e-8)
		{
			X /= varSize;
			Y /= varSize;
		}
	}

	//bool IsNormalized() const
	//{
	//	return (FMath::Abs(1.f - SizeSquared()) < THRESH_VECTOR_NORMALIZED);
	//}

	//void ToDirectionAndLength(DVector &OutDir, double &OutLength) const
	//{
	//	OutLength = Size();
	//	if (OutLength > 1.e-8)
	//	{
	//		double OneOverLength = 1.0f / OutLength;
	//		OutDir = DVector(X*OneOverLength, Y*OneOverLength,
	//			Z*OneOverLength);
	//	}
	//	else
	//	{
	//		OutDir = DVector::ZeroVector;
	//	}
	//}

	//DVector GetSignVector() const
	//{
	//	return FVector
	//	(
	//		FMath::FloatSelect(X, 1.f, -1.f),
	//		FMath::FloatSelect(Y, 1.f, -1.f),
	//		FMath::FloatSelect(Z, 1.f, -1.f)
	//	);
	//}

	/**
	 * Projects 2D components of vector based on Z.
	 *
	 * @return Projected version of vector based on Z.
	 */
	//DVector Projection() const
	//{
	//	const double RZ = 1.0 / Z;
	//	return DVector(X*RZ, Y*RZ, 1);
	//}

	//DVector ProjectOnTo(const DVector& A) const
	//{
	//	return (A * ((*this | A) / (A | A)));
	//}

	DVector GetNormal() const
	{
		DVector tempVec(X, Y);
		if(tempVec.Size() > 1.e-8)
			tempVec.Normalize();
		return tempVec;
	}

	static double Dist(const DVector &v1, const DVector &v2)
	{
		double xDiff = (v2.X - v1.X);
		double yDiff = (v2.Y - v1.Y);
		return sqrt(xDiff*xDiff + yDiff*yDiff);
	}
};
