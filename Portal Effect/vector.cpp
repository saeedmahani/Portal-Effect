
#include "vector.h"
#include "matrix.h"

#define VTYPE	float

Vector3DF &Vector3DF::operator*= (const MatrixF &op)
{
	double *m = op.GetDataF ();
	float xa, ya, za;
	xa = x * float(*m++);	ya = x * float(*m++);	za = x * float(*m++);	m++;
	xa += y * float(*m++);	ya += y * float(*m++);	za += y * float(*m++);	m++;
	xa += z * float(*m++);	ya += z * float(*m++);	za += z * float(*m++);	m++;
	xa += float(*m++);		ya += float(*m++);		za += float(*m++);
	x = xa; y = ya; z = za;
	return *this;
}

Vector3DF &Vector3DF::operator*= (const Matrix4F &op)
{
	float xa, ya, za;
	xa = x * op.data[0] + y * op.data[4] + z * op.data[8] + op.data[12];
	ya = x * op.data[1] + y * op.data[5] + z * op.data[9] + op.data[13];
	za = x * op.data[2] + y * op.data[6] + z * op.data[10] + op.data[14];
	x = xa; y = ya; z = za;
	return *this;
}

Vector4DF &Vector4DF::operator*= (const MatrixF &op)
{
	double *m = op.GetDataF ();
	VTYPE xa, ya, za, wa;
	xa = x * float(*m++);	ya = x * float(*m++);	za = x * float(*m++);	wa = x * float(*m++);
	xa += y * float(*m++);	ya += y * float(*m++);	za += y * float(*m++);	wa += y * float(*m++);
	xa += z * float(*m++);	ya += z * float(*m++);	za += z * float(*m++);	wa += z * float(*m++);
	xa += w * float(*m++);	ya += w * float(*m++);	za += w * float(*m++);	wa += w * float(*m++);
	x = xa; y = ya; z = za; w = wa;
	return *this;
}

Vector4DF &Vector4DF::operator*= (const Matrix4F &op)
{
	float xa, ya, za, wa;
	xa = x * op.data[0] + y * op.data[4] + z * op.data[8] + w * op.data[12];
	ya = x * op.data[1] + y * op.data[5] + z * op.data[9] + w * op.data[13];
	za = x * op.data[2] + y * op.data[6] + z * op.data[10] + w * op.data[14];
	wa = x * op.data[3] + y * op.data[7] + z * op.data[11] + w * op.data[15];
	x = xa; y = ya; z = za; w = wa;
	return *this;
}

