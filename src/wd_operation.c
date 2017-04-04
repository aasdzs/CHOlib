/**
 * @file wd_operation.c
 * @brief WORD ���� ����
 * @details 
 * - Addition
 * - Square
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
#include "wd_operation.h"

/**
 * @brief Multiply UNWORD A from UNWORD B
 * @details
 * - UNWORD A ���� UNWORD B �� ���ؼ� UNWORD *R ��� (�ִ� 2 WORD ũ��)
 * - UNWORD ���� -> R = A * B
 * @param[out] UNWORD *R
 * @param[in] UNWORD A (const)
 * @param[in] UNWORD B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void UNWORD_Mul(UNWORD *r, const UNWORD a, const UNWORD b)
{	
	UNWORD half_len = BIT_LEN / 2;
	
	UNWORD a_H = (a >> half_len); // WORD ���� ���� bit
	UNWORD a_L = (a & WORD_MASK_L); // WORD ���� ���� bit
	UNWORD b_H = (b >> half_len);
	UNWORD b_L = (b & WORD_MASK_L);
	
	UNWORD aH_bL = (a_H * b_L);
	UNWORD aL_bH = (a_L * b_H);

	r[1] = (a_H * b_H); 
	r[0] = (a_L * b_L); 
	
	r[1] += (aH_bL >> half_len);
	r[0] += (aH_bL << half_len);
	if(r[0] < (aH_bL << half_len)) // carry
		r[1] += 1;
	
	r[1] += (aL_bH >> half_len);
	r[0] += (aL_bH << half_len);
	if(r[0] < (aL_bH << half_len)) // carry
		r[1] += 1;
}

/**
 * @brief Multiply BIGNUM A from UNWORD b
 * @details
 * - BIGNUM A ���� UNWORD B �� ���ؼ� BIGNUM *R ��� (�ִ� 2 WORD ũ��)
 * - BIGNUM * UNWORD ���� -> R = A * b
 * @param[out] BIGNUM *R
 * @param[in] BIGNUM A (const)
 * @param[in] UNWORD b (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_UNWORD_Mul(BIGNUM *R, const BIGNUM *A, const UNWORD b)
{	
	UNWORD i, n;
	UNWORD carry = 0;
	UNWORD tmp[2];
		
	if((A->Length == 0) || (b == 0)) // BIGNUM *A or UNWORD b �� 0 �� ���
		BN_Zeroize(R); 
	else if(b == 1) // UNWORD b == 1 �� ���
		BN_Copy(R, A);
	else if((A->Length == 1) && (A->Num[0] == 1)) // B = 1 or -1 �� ��� 
		R->Num[0] = b;
	else // ���� ���� ����
	{
		for(i = 0 ; i < A->Length ; i++)
		{
			// WORD * WORD ���� , tmp[1]|tmp[0] ���
			UNWORD_Mul(tmp, A->Num[i], b);
			
			// ��� �迭�� tmp[0] �߰�
			R->Num[i] += tmp[0];
			carry = (R->Num[i] < tmp[0]); // carry 
			tmp[1] = (tmp[1] + carry);
			
			// carry ����� ����
			carry = (tmp[1] < carry); // tmp[1] + c < c �� ��� carry 
			R->Num[i + 1] += tmp[1];
			carry += (R->Num[i + 1] < tmp[1]); // A + B + c < A + c �� ��� carry �߻�
			
			// ���� ���� �迭�� ���� carry ���, �ִ� �迭 : R->Num[A->Length + B->Length - 1]
			if((carry == 1) && ((i + 1) < (A->Length)))
			{
				n = (i + 1);
				while (carry) // ���� �迭�� �ö󰡸鼭 carry ���
				{
					n++;
					R->Num[n] += carry;
					carry = (R->Num[n] < carry);
				}
			}
		}
	}
	
	// ��ȣ ����
	R->Sign = A->Sign;

	// BIGNUM ����ȭ
	BN_Optimize_Out(R);		
}

/**
 * @brief Multiply UNWORD A and B and 2
 * @details
 * - UNWORD A * B * 2 => UNWORD *R ��� (�ִ� 3 WORD ũ��)
 * - UNWORD ���� -> R = 2 * (A * B)
 * @param[out] UNWORD *R
 * @param[in] UNWORD A (const)
 * @param[in] UNWORD B (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void UNWORD_2_Mul(UNWORD *r, const UNWORD a, const UNWORD b)
{	
	UNWORD tmp_h, tmp_l;
	
	// WORD ���� ����
	UNWORD_Mul(r, a, b);

	tmp_h = (r[1] << 1); // r[1] * 2
	tmp_l = (r[0] << 1); // r[0] * 2
	
	r[2] = (tmp_h < r[1]); // carry of r[1] (1 or 0)
	r[1] = tmp_h + (tmp_l < r[0]); // r[1] + carry of r[0]
	r[0] = tmp_l;
}

/**
 * @brief Square UNWORD A
 * @details
 * - UNWORD A �� �����ؼ� UNWORD *R ��� (�ִ� 2 WORD ũ��)
 * - UNWORD ���� -> R = (A)^2
 * - �ٸ� ������̶� �ӵ� ���� ���� ����..
 * @param[out] UNWORD *R
 * @param[in] UNWORD A (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void UNWORD_Sqr(UNWORD *r, const UNWORD a)
{	
	UNWORD half_len = BIT_LEN / 2;
	
	UNWORD a_H = (a >> half_len);	// WORD ���� ���� bit
	UNWORD a_L = (a & WORD_MASK_L); // WORD ���� ���� bit
	
	UNWORD aH_bL = (a_H * a_L);
	
	r[1] = (a_H * a_H); 
	r[0] = (a_L * a_L); 
		
	// 2�� �ݺ�
	r[1] += (aH_bL >> half_len);
	r[0] += (aH_bL << half_len);
	if(r[0] < (aH_bL << half_len)) // carry 
		r[1] += 1;

	r[1] += (aH_bL >> half_len);
	r[0] += (aH_bL << half_len);
	if(r[0] < (aH_bL << half_len)) // carry 
		r[1] += 1;
}
