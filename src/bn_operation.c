/**
 * @file bn_operation.c
 * @brief BIGNUM �̿��� ����
 * @details 
 * - Addition
 * - Subtraction
 * - Multiplication
 * - Modular
 * - inversion
 * �ڼ��� ����
 * @date 2017. 03 ~ 07.
 * @author YoungJin CHO
 * @version 1.00
 */

#include "config.h"
#include "bignum.h"
#include "bn_operation.h"
#include "wd_operation.h"

/**
 * @brief Absolute Add BIGNUM *A and BIGNUM *B
 * @details
 * - BIGNUM *A �� BIGNUM *B �� ���ؼ� BIGNUM *R ���
 * - BN_Abs_Add() �Լ� ����(Length) : A >= B  
 * - Sign ���� ���� WORD ���� -> R = |A| + |B|
 * - *R �� �ִ� ũ�� : A.Length + 1
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Abs_Add(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{	
	UNWORD i;
	UNWORD carry = 0;		// �ʱ� Carry = 0
	UNWORD tmp1, tmp2;
	
	//// R ũ�Ⱑ |A| + |B| ������� ������ ���Ҵ� 
	//if(R->Length < (A->Length + 1))
	//	BN_Realloc_Mem(R, (A->Length + 1));	

	for(i = 0 ; i < B->Length ; i++) 
	{
		tmp1 = (A->Num[i] + carry) & WORD_MASK;
		carry = (tmp1 < carry); // A + c < c �� ��� carry �߻� 
		tmp2 = (tmp1 + B->Num[i]) & WORD_MASK;
		carry += (tmp2 < tmp1); // A + B + c < A + c �� ��� carry �߻�
		R->Num[i] = tmp2;					
	}

	// A > B �� ��� ������ A �κ� ����
	if(carry)
	{
		for( ; i < A->Length ; i++)
		{
			tmp1 = (A->Num[i] + carry) & WORD_MASK;
			carry = (tmp1 < carry);
			R->Num[i] = tmp1;
		}

		if(carry)
		{
			R->Num[i] = carry;
			R->Length = A->Length + 1;
		}
	}
	else // carry = 0 �̸� ������ �κ� �׳� ����
		for( ; i < A->Length ; i++)
			R->Num[i] = A->Num[i];
	
	R->Sign = PLUS;
	//// BIGNUM ����ȭ
	//BN_Optimize_Out(R);	
}

/**
 * @brief Absolute Subtract BIGNUM *B from BIGNUM *A
 * @details
 * - BIGNUM *A ���� BIGNUM *B �� ���� BIGNUM *R ���
 * - BN_Abs_Sub() �Լ� ����(Length) : A > B  
 * - Sign ���� ���� WORD ���� -> R = |A| - |B|
 * - *R �� �ִ� ũ�� : A.Length
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Abs_Sub(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{	
	UNWORD i;
	UNWORD borrow = 0;	// �ʱ� borrow = 0
	UNWORD tmp1, tmp2;	
	
	//// R ũ�Ⱑ |A| - |B| ������� ������ ���Ҵ�
	//if(R->Length < A->Length)
	//	BN_Realloc_Mem(R, (A->Length));	

	for(i = 0 ; i < B->Length ; i++) 
	{
		tmp1 = (A->Num[i] - borrow) & WORD_MASK;
		borrow = (tmp1 > A->Num[i]); // A - b > A �� ��� borrow �߻�
		tmp2 = (tmp1 - B->Num[i]) & WORD_MASK;
		borrow += (tmp1 < B->Num[i]); // A - b < B �� ��� borrow �߻�
		R->Num[i] = tmp2;					
	}
	// A > B �� ��� ������ A �κ� ����
	if(borrow)
	{
		for( ; i < A->Length ; i++)
		{
			tmp1 = (A->Num[i] - borrow) & WORD_MASK;
			if(A->Num[i] > 0) // A->Num[i] > 0 �̸� ���� borrow �߻� X
				borrow = 0;
			
			R->Num[i] = tmp1;	
		}
	}
	else // borrow = 0 �̸� ������ �κ� �׳� 
		for( ; i < A->Length ; i++)
			R->Num[i] = A->Num[i];	

	R->Sign = PLUS;
	// BIGNUM ����ȭ
	BN_Optimize_Out(R);	
}

/**
 * @brief Add BIGNUM *A and BIGNUM *B
 * @details
 * - BIGNUM *A �� BIGNUM *B �� ���ؼ� BIGNUM *R ���
 * - ��ȣ ���� ����
 * - WORD ���� -> R = A + B
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Add(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{		
	// A or B �� 0 �� ��� �ܼ� �迭 ����
	if(A->Length == 0)
		BN_Copy(R, B);
	else if(B->Length == 0)
		BN_Copy(R, A);
	else // A, B ��� 0 �� �ƴ� ���
	{
		if(A->Sign == B->Sign) 
		{
			//  a +  b =  (a + b)
			// -a + -b = -(a + b)
			R->Sign = A->Sign; // ��� �� R ��ȣ�� A, B ��ȣ�� ����

			if(A->Length >= B->Length)
				BN_Abs_Add(R, A, B);
			else // (A->Length < B->Length)
				BN_Abs_Add(R, B, A);
		}
		else // A->Sign != B->Sign �� ���
		{
			if(BN_Abs_Cmp(A, B) == LARGE) // A > B �� ���
			{
				//  a + -b =  (a - b)
				BN_Abs_Sub(R, A, B);
				R->Sign = A->Sign; // ����� �� ū �� ��ȣ ����
			}
			else // // A <= B �� ��� 
			{
				// -a +  b =  (b - a)
				BN_Abs_Sub(R, B, A);
				R->Sign = B->Sign; // ����� �� ū �� ��ȣ ����
			}
		}		
	}
}

/**
 * @brief Subtract BIGNUM *B from BIGNUM *A
 * @details
 * - BIGNUM *A ���� BIGNUM *B �� ���� BIGNUM *R ���
 * - ��ȣ ���� ����
 * - WORD ���� -> R = A - B
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Sub(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{	
	if(A->Sign != B->Sign) 
	{	
		if(BN_Abs_Cmp(A, B) == LARGE) // A > B �� ���
			BN_Abs_Add(R, A, B);
		else // // A <= B �� ��� 
			BN_Abs_Add(R, B, A);
	
		// ( a) - (-b) =  (a + b)
		// (-a) - ( b) = -(a + b)
		R->Sign = A->Sign; // ����� ũ�� ��� ���� A ��ȣ ����
	}
	else // A->Sign == B->Sign �� ���
	{
		// ( a) - ( b) =  a - b)
		// (-a) - (-b) = -a + b)
		if(BN_Abs_Cmp(A, B) == LARGE)
		{
			BN_Abs_Sub(R, A, B);
			R->Sign = A->Sign; // ����� �� ū �� ��ȣ ����
		}
		else // (A->Length < B->Length)
		{
			BN_Abs_Sub(R, B, A);
			R->Sign = -(B->Sign); // - ������ ����� �� ū �� �ݴ� ��ȣ ����
		}
	}	
}

/**
 * @brief Multiply BIGNUM *A and BIGNUM *B
 * @details
 * - BIGNUM *A �� BIGNUM *B �� ���� ��� BIGNUM *R ���
 * - �⺻ ���� ��� ����
 * - ��ȣ ���� ����
 * - WORD ���� -> R = A * B
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Basic_Mul(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{	
	UNWORD i, j, n;
	UNWORD carry = 0;
	UNWORD tmp[2];
		
	if((A->Length == 0) || (B->Length == 0)) // A or B �� 0 �� ���
		BN_Zeroize(R); 
	else if((A->Length == 1) && (A->Num[0] == 1)) // A = 1 or -1 �� ���
		BN_Copy(R, B);
	else if((B->Length == 1) && (B->Num[0] == 1)) // B = 1 or -1 �� ��� 
		BN_Copy(R, A);
	else // ���� ���� ����
	{
		for(i = 0 ; i < A->Length ; i++)
		{
			for(j = 0 ; j < B->Length ; j++)
			{
				// WORD * WORD ���� , tmp[1]|tmp[0] ���
				UNWORD_Mul(tmp, A->Num[i], B->Num[j]);
				
				// ��� �迭�� tmp[0] �߰�
				R->Num[i + j] += tmp[0];
				carry = (R->Num[i + j] < tmp[0]); // carry 
				tmp[1] = (tmp[1] + carry);
				
				// carry ����� ����
				carry = (tmp[1] < carry); // tmp[1] + c < c �� ��� carry 
				R->Num[i + j + 1] += tmp[1];
				carry += (R->Num[i + j + 1] < tmp[1]); // A + B + c < A + c �� ��� carry �߻�
				
				// ���� ���� �迭�� ���� carry ���, �ִ� �迭 : R->Num[A->Length + B->Length - 1]
				if((carry == 1) && ((i + j + 1) < (A->Length + B->Length)))
				{
					n = (i + j + 1);
					while (carry) // ���� �迭�� �ö󰡸鼭 carry ���
					{
						n++;
						R->Num[n] += carry;
						carry = (R->Num[n] < carry);
					}
				}
			}
		}
	}
	
	// ��ȣ ����
	R->Sign = (A->Sign * B->Sign);

	// BIGNUM ����ȭ
	BN_Optimize_Out(R);			
}

/**
 * @brief Multiply BIGNUM *A and BIGNUM *B by Karatsuba Multiplication
 * @details
 * - BIGNUM *A �� BIGNUM *B �� ���� ��� BIGNUM *R ���
 * - �⺻ ���� ��� ����
 * - ��ȣ ���� ����
 * - WORD ���� -> R = A * B
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Kara_Mul(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{
	BIGNUM u0, v0, u1, v1;			 // ����ü�� �̿� (A �� Num ���)
	BIGNUM us, vs, u0v0, u1v1, usvs, tmp; // BIGNUM �����ؼ� �̿�
	
	UNWORD d, q, p;
	UNWORD d0 = 1;
	
	// d = max(a,b) 
	d = (A->Length > B->Length) ? A->Length : B->Length;
	// Floor, Ceil ���� (p <= q)
	// d = p + q
	p = (d >> 1);
	q = (d & 1) ? (p + 1) : (p);	
		
	if(p > d0) // d > KARA_THRESHOLD(d0) �� ��� ī����� ���� 
	{		
		// u0, u1, v0, v1 ����
		u0.Num = A->Num;		// u0 = (q-1, q-2, ..., 0)		// u0.Length = q
		u1.Num = A->Num + q;	// u1 = (p+q-1, p+q-2, ..., q)	// u1.Length = p = (A->Length - q)
		v0.Num = B->Num;		// v0 = (q-1, q-2, ..., 0)		// v0.Length = q
		v1.Num = B->Num + q;	// v1 = (p+q-1, p+q-2, ..., q)	// v1.Length = p = (B->Length - q)
				
		// Positive Multiprecision Integers
		u0.Sign = u1.Sign = PLUS;
		v0.Sign = v1.Sign = PLUS;
				
		// p, q ���� ���ϱ� (�߿�)
		if(q > A->Length) // u1.Length = (A->Length - q) < 0
		{
			u0.Length = A->Length;
			u1.Length = 0;
		}
		else
		{
			u0.Length = q;
			u1.Length = (p > A->Length - q) ? (A->Length - q) : p;
		}
		if(q > B->Length)
		{
			v0.Length = B->Length;
			v1.Length = 0;
		}
		else
		{
			v0.Length = q;
			v1.Length = (p > B->Length - q) ? (B->Length - q) : p;
		}
		// ���� ��� BIGNUM ����
		BN_Init(&us, (u0.Length + 1), PLUS, DEFAULT);
		BN_Init(&vs, (v0.Length + 1), PLUS, DEFAULT);
		BN_Init(&u0v0, (u0.Length + v0.Length), PLUS, DEFAULT);
		BN_Init(&u1v1, (u1.Length + v1.Length), PLUS, DEFAULT);
		BN_Init(&usvs, (us.Length + vs.Length), PLUS, DEFAULT);
		BN_Init(&tmp, (us.Length + vs.Length), PLUS, DEFAULT);
		
		// u0 + u1
		BN_Abs_Add(&us, &u0, &u1);
		// v0 + v1
		BN_Abs_Add(&vs, &v0, &v1);
		// u0v0
		BN_Kara_Mul(&u0v0, &u0, &v0);
		// u1v1
		BN_Kara_Mul(&u1v1, &u1, &v1);
		// ((usvs - u1v1) - u0v0) = usvs > 0
		BN_Kara_Mul(&usvs, &us, &vs);
		BN_Abs_Sub(&tmp, &usvs, &u1v1);
		BN_Zeroize(&usvs);
		BN_Abs_Sub(&usvs, &tmp, &u0v0);

		// R = (u1v1)^2q + (usvs)^q + u0v0
		// R += u0v0
		R->Length = u0v0.Length + 1; // carry ����Ͽ� ���� + 1
		BN_Abs_Add(R, R, &u0v0);
		// R += (usvs)^q
		R->Num += q;
		R->Length = usvs.Length + 1; 
		BN_Abs_Add(R, R, &usvs);
		// R += (u1v1)^2q
		R->Num += q;
		R->Length = u1v1.Length + 1; 
		BN_Abs_Add(R, R, &u1v1);
		
		// R ���� ����
		R->Length = A->Length + B->Length; 
		R->Num -= (q << 1);

		// BIGNUM �Ҵ� ���� 
		BN_Zero_Free(&us);
		BN_Zero_Free(&vs);
		BN_Zero_Free(&u0v0);
		BN_Zero_Free(&u1v1);
		BN_Zero_Free(&usvs);
		BN_Zero_Free(&tmp);

	}
	else // d <= KARA_THRESHOLD(d0) �� ��� �⺻ Mul ����
		BN_Basic_Mul(R, A, B);
		
}

/**
 * @brief Multiplication (Basic or Karatsuba)
 * @details
 * - BIGNUM *A �� BIGNUM *B �� ���� ��� BIGNUM *R ���
 * - �⺻ ���� ��� ����
 * - ��ȣ ���� ����
 * - WORD ���� -> R = A * B
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Mul(BIGNUM *R, const BIGNUM *A, const BIGNUM *B)
{
	BN_Kara_Mul(R, A, B);
	//BN_Basic_Mul(R, A, B);
}

/**
 * @brief Square BIGNUM *A 
 * @details
 * - BIGNUM *A �� ���� ��� BIGNUM *R ���
 * - ���� ���� UNWORD ���� �� ����
 * - ��ȣ ���� ����
 * - WORD ���� -> R = (A)^2
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Sqr(BIGNUM *R, const BIGNUM *A)
{
	UNWORD i, j, n;
	UNWORD carry = 0;
	UNWORD tmp[3];
	UNWORD m0, m1;
			
	if(A->Length == 0) // A �� 0 �� ���
		BN_Zeroize(R); 
	else if((A->Length == 1) && (A->Num[0] == 1)) // A = 1 or -1 �� ���
		BN_Copy(R, A);
	else // ���� ���� ����
	{
		for(i = 0 ; i < A->Length ; i++)
		{
			m0 = i << 1; // (2 * i)
			m1 = m0 + 1; // (2 * i) + 1

			// (WORD)^2 ���� , tmp[1]|tmp[0] ���
			UNWORD_Sqr(tmp, A->Num[i]);

			// ��� �迭�� tmp[0] �߰�
			R->Num[m0] += tmp[0];
			carry = (R->Num[m0] < tmp[0]); // carry 

			// carry ����� ����
			tmp[1] = (tmp[1] + carry);
			carry = (tmp[1] < carry); // tmp[1] + c < c �� ��� carry 
			R->Num[m1] += tmp[1];
			if(R->Num[m1] < tmp[1]) // A + B + c < A + c �� ��� carry �߻�
				carry += 1;
						
			// ���� ���� �迭�� ���� carry ���, �ִ� �迭 : R->Num[(2 * A->Length) - 1]
			if((carry == 1) && (m1 < (A->Length << 1)))
			{
				while (carry) // ���� �迭�� �ö󰡸鼭 carry ���
				{
					m1++;
					R->Num[m1] += carry;
					carry = (R->Num[m1] < carry);
				}
			}
			
			for(j = i + 1 ; j < A->Length ; j++)
			{
				// (WORD * WORD) * 2 , tmp[2]|tmp[1]|tmp[0] ��� (�ִ� 3 �迭)
				UNWORD_2_Mul(tmp, A->Num[i], A->Num[j]);
				
				// ��� �迭�� tmp[0] �߰�
				R->Num[i + j] += tmp[0];
				carry = (R->Num[i + j] < tmp[0]); // carry 
				tmp[1] = (tmp[1] + carry);
				carry = (tmp[1] < carry); // tmp[1] + c < c �� ��� carry 
				R->Num[i + j + 1] += tmp[1];
				carry += (R->Num[i + j + 1] < tmp[1]) + tmp[2]; // A + B + c < A + c �� ��� carry �߻�
				
				// ���� ���� �迭�� ���� carry ���, �ִ� �迭 : (2 * R->Num[A->Length]) - 1]
				if((carry > 0) && ((i + j + 1) < (A->Length << 1)))
				{
					n = (i + j + 1);
					while (carry) // ���� �迭�� �ö󰡸鼭 carry ���
					{
						n++;
						R->Num[n] += carry;
						carry = (R->Num[n] < carry);
					}
				}
			}
		}
	}
	
	// ��ȣ ������ ���
	R->Sign = PLUS;
	
	// BIGNUM ����ȭ
	BN_Optimize_Out(R);	
}

/**
 * @brief Barret Reduction of BIGNUM
 * @details
 * - 
 * - Guide to ECC p.36 ���� \n
 * @param[out] BIGNUM *R 
 * @param[in] BIGNUM *Z (const)
 * @param[in] BIGNUM *P (const)
 * @param[in] BIGNUM *MU (const)
 * @date 2017. 03. 29. v1.00 \n
 * @todo ���� �̿ϼ�
 */
void BN_Bar_Redc(BIGNUM *R, const BIGNUM *Z, const BIGNUM *P, const BIGNUM *MU)
{	
	BIGNUM qh, qh_p, s_tmp;	// BN_Init() �ʿ� 
	BIGNUM tmp;				// BIGNUM �ּҸ� Ȱ��
	UNWORD k;
	
	// Ȯ�λ���
	// Input ���� ������� �ϳ� ? -> P >= 3 
	// Z == P �� ���� ??
	// Reduction ��� ����ȭ ���� ?

	// BIGNUM ���� �ִ� ũ�� ���
	BN_Init(&qh, 0, ZERO); // ũ�� ����
	BN_Init(&qh_p, 0, ZERO);
	

	// Z < P �� ��� -> Reduction �ʿ� X
	if(BN_Abs_Cmp(Z, P) == SMALL)
		BN_Copy(R, Z); 
	else // Z > P �� ��� Reduction ����
	{
		// b = 2^(L), L : WORD ũ�� or ���μ��� ����
		k = P->Length;	// k = Ceil(log_b(P)) + 1 
		
		// Ceil(Z / b^(k-1)) ��� (���� ���� >> ����)
		tmp.Num = Z->Num + k - 1; // ���� ������ ��ġ ����
		tmp.Length = Z->Length - k + 1; 
		tmp.Sign = Z->Sign; // �׻� Z > 0
				
		BN_Mul(&qh, &tmp, MU);
		// Ceil(qh / b^(k+1)) ��� (���� ���� >> ����)
		qh.Num += (k + 1); 
		qh.Length -= (k + 1);

		// Z mod b^(k+1) -> k + 1 ���� ���� �迭�� �츲
		tmp.Num = Z->Num;
		tmp.Length = k;
		
		// (qh * P) mod b^(k+1)
		BN_Mul(&qh_p, &qh, P);
		if(qh_p.Length > k)  
			qh_p.Length = k;

		// Z mod b^(k+1) - (qh * P) mod b^(k+1)
		BN_Sub(R, &tmp, &qh_p);
		
		// R �� ���� -> R = R + b^(k+1)
		if(R->Sign == MINUS)
		{
			BN_Init(&s_tmp, k + 1, ZERO, DEFAULT); // (k+1) �� s_tmp.Num �迭 -> 0 �ʱ�ȭ
			s_tmp.Num[k + 1] = 1;	// b^(k+1) ex) 0x10..0
			s_tmp.Sign = PLUS;

			BN_Add(R, &s_tmp, R);
		}
		// R >= p �̸� -> R - p �ݺ�
		while(BN_Abs_Cmp(R, P) != MINUS)
			BN_Sub(R, R, P);		
	}

	// BN_Free() or BN_Zero_Free() ? 
	BN_Zero_Free(&qh);
	BN_Zero_Free(&qh_p);		
}

/**
 * @brief Montgomery Reduction of BIGNUM
 * @details
 * - 
 * - Handbook of E&HC p.181 ���� \n
 * @param[out] BIGNUM *T 
 * @param[in] BIGNUM *N (const)
 * @param[in] BIGNUM *R (const)
 * @param[in] UNWORD Np (const)
 * @param[in] BIGNUM *U (const)
 * @date 2017. 03. 29. v1.00 \n
 * @todo ���� �̿ϼ�
 */
void BN_Mont_Redc(BIGNUM *T, const BIGNUM *N, const BIGNUM *R, const UNWORD Np, const BIGNUM *U)
{	
	/*
	BIGNUM t, knb;
	UNWORD i, k;
	UNWORD tmp[2];
		
	BN_Init_Copy(&t, U);
	BN_Init(knb, N->Length + 1, PLUS, DEFAULT);

	for(i = 0 ; i < N->Length ; i++)	
	{
		UNWORD_Mul(tmp, t.Num[i], Np);
		k = tmp[0]; // k = (ti * Np) mod b
		BN_UNWORD_Mul(knb, N, k);

		// �����ϱ� ���� ��ġ�� �̵�
		t.Num += i;
		t.Length -= i;

		BN_ADD(&t, &t, knb);
		
		// ���� ��ġ�� ����
		t.Num -= i;
		t.Length += i;
	}
	
	// t = t/R
	t.Num += N->Length;
	t.Length -= N->Length;
	
	if(BN_Abs_Cmp(t, N) != SMALL) // => if(t >= N)
		BN_Abs_Sub(t, t, N);

	BN_COPY(T, &t);

	BN_Zero_Free(t);
	BN_Zero_Free(knb);
	*/
}