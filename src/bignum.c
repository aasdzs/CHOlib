/**
 * @file bignum.c
 * @brief 
 * @details 
 * - Initialize
 * - Zeroize
 * - Randomize
 * - Free
 * - Reaalocation
 * - Compare
 * - Copy
 * @date 2017. 04. 04.
 * @author YoungJin CHO
 * @version 1.01
 */

#include "config.h"
#include "bignum.h"

/**
 * @brief Initialize to BIGNUM
 * @details 
 * - �޸� �Լ� ���ο��� �Ҵ� \n
 * - Get BIGNUM struct (Num, Length, Sign, Flag) -> Generate BIGNUM \n
 * @param[in,out] BIGNUM *A 
 * @param[in] UNWORD len (const) ����
 * @param[in] SNWORD sign (const) ��ȣ
 * @param[in] SNWORD flag (const) ����ȭ �ɼ�
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 30. v1.01 (Flag) \n
 */
void BN_Init(BIGNUM *A, const UNWORD len, const SNWORD sign, const SNWORD flag)
{
	A->Num = (UNWORD *)calloc(len, sizeof(UNWORD)); 
	A->Length = len; 
	A->Sign = sign;
	A->Flag = flag;
}

/**
 * @brief Initialize to Zero BIGNUM
 * @details
 * - �޸� �Լ� ���ο��� �Ҵ� \n
 * - Generate Zero BIGNUM \n
 * - Length = 1, Sign = ZERO, Flag = DEFAULT \n
 * @param[in,out] BIGNUM *A  
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 30. v1.01 (Flag) \n
 */
void BN_Init_Zero(BIGNUM *A)
{	
    A->Num = (UNWORD *)calloc(1, sizeof(UNWORD));
	A->Length = 1; 
	A->Sign = ZERO;
	A->Flag = DEFAULT;
}

/**
 * @brief Initialize to Random BIGNUM
 * @details
 * - �޸� �Լ� ���ο��� �Ҵ� \n
 * - ���� BIGNUM ���� \n
 * - Num, Length, Sign => ��� ���� \n
 * - �ܼ� TEST��, ������ ���� X \n
 * @param[in,out] BIGNUM *A 
 * @param UNWORD maxsize (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Init_Rand(BIGNUM *A, const UNWORD maxsize)
{
	UNWORD rlen;
		
	rlen = (rand() % maxsize) + 1; // ���� 0 ����
	BN_Init(A, rlen, ZERO, DEFAULT); 
	BN_Randomize(A);

	if((rlen == 1) && (A->Num[0] == 0))
		A->Sign = ZERO; 
	else if(rlen & 1)
		A->Sign = MINUS;
	else
        A->Sign = PLUS;
}

/**
 * @brief Initialize and Copy BIGNUM *A to *R
 * @details
 * - �޸� �Լ� ���ο��� �Ҵ� \n
 * - BIGNUM �����ؼ� (R <- A) COPY \n
 * @param[in,out] BIGNUM *R
 * @param[in] BIGNUM *A
 * @date 2017. 03. 30. v1.00 \n
 */
void BN_Init_Copy(BIGNUM *R, const BIGNUM *A)
{
	// A �� ũ�� ���� BIGNUM ����
	BN_Init(R, A->Length, A->Sign, A->Flag);
	// �� ����
	BN_Copy(R, A);
}

/**
 * @brief Zeroize BIGNUM
 * @details 
 * - BIGNUM ���� ZERO �� ���� \n
 * - flag(Optimize) : �޸� ���Ҵ� (ũ�� ����), Length = 1 \n
 * - flag(Dafault) : ���� 0 ���� ���� (ũ�� ���� X) \n 
 * @param[in,out] BIGNUM *A 

 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 29. v1.01 (Flag) \n 
 */
void BN_Zeroize(BIGNUM *A)
{	
	UNWORD i;

	for(i = 0 ; i < A->Length ; i++)
			A->Num[i] = 0;
	A->Sign = ZERO;			

	// ũ�⵵ ���Ҵ�
	if(A->Flag == OPTIMIZE)
	{
		A->Num = (UNWORD *)realloc(A->Num, sizeof(UNWORD));
    	A->Length = 1;
	}
}

/**
 * @brief Randomize BIGNUM
 * @details
 * - ������ ������ BIGNUM �� ���� �� �Է� \n
 * - Length ���� X \n
 * - �ܼ� TEST��, ������ ���� X \n
 * - flag(Optimize) : ��ȣ ���� \n
 * - flag(Dafault) : ���� ���� ���� ���� (ũ�� ���� X) \n 
 * @param[in,out] BIGNUM *A 
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 30. v1.01 (Flag) \n
 * @todo ���� ���
 */
void BN_Randomize(BIGNUM *A)
{
	UNWORD i, j;
	 
	for(i = 0 ; i < A->Length ; i++)
	{
		for(j = 0 ; j < BIT_LEN ; j++)
		{
			A->Num[i] <<= 8;
			A->Num[i] ^= rand() ;
		}		
	}
	
	// Flag(Optimize) : ��ȣ ����
	if(A->Flag == OPTIMIZE)
	{
		if((A->Length == 1) && (A->Num[0] == 0))
			A->Sign = ZERO; // ���� ���� ���� ���� 0 �̸� Sign = ZERO
		else if(A->Num[0] & 1)
			A->Sign = MINUS;
		else
        	A->Sign = PLUS;
	}
}

/**
 * @brief Free BIGNUM Memory
 * @details
 * - BIGNUM Num WORD �����Ҵ� ����
 * - 0 �ʱ�ȭ ���� �ܼ� �޸� �Ҵ� ����
 * - BIGNUM �� Free �� ���� => Length = 0 , Sign = 0, Flag = 0
 * @param[in,out] BIGNUM *A 
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 30. v1.01 (Flag) \n
 */
void BN_Free(BIGNUM *A)
{
	free(A->Num);
	A->Length = ZERO;
	A->Sign = ZERO;
	A->Flag = DEFAULT;
}

/**
 * @brief Zero Free BIGNUM Memory
 * @details
 * - BIGNUM Num WORD �����Ҵ� ����
 * - ���� 0 ���� �ʱ�ȭ �� �� �޸� �Ҵ� ����
 * - BIGNUM �� Free �� ���� => Length = 0 , Sign = 0, Flag = 0
 * @param[in,out] BIGNUM *A 
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 03. 30. v1.01 (Flag) \n 
 */
void BN_Zero_Free(BIGNUM *A)
{
	while(A->Length > 0)
	{
		A->Num[(A->Length - 1)] = 0;
		A->Length--;
	}
	free(A->Num);
	A->Sign = ZERO;
	A->Flag = DEFAULT;
}

/**
 * @brief Memory Reallocate BIGNUM *A
 * @details
 * - �Է� ���� ������ �̻��� ���� 0 �ʱ�ȭ
 * - ������ ���� ������ �� ����
 * - BIGNUM *A �޸𸮸� �Է¹��� ũ��� �����ϰ� ���Ҵ�
 * @param[out] BIGNUM *A
 * @param[in] UNWORD size (const)
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 04. 04. v1.01 \n
 */
void BN_Realloc_Mem(BIGNUM *A, const UNWORD size)
{	
	if(A->Length > size)
	{
		// �Է¹��� size ũ�� ���� ū �� 0 ���� ���� ��
		while(A->Length > size)
		{
			A->Num[(A->Length - 1)] = 0;
			A->Length--;
		}
		// �޸� ���Ҵ�
		A->Num = (UNWORD *)realloc(A->Num, A->Length * sizeof(UNWORD));
	}
	else if(A->Length < size)
	{
		// �޸𸮸� ���Ҵ�
		A->Num = (UNWORD *)realloc(A->Num, size * sizeof(UNWORD));
		A->Length = size;
	}	
}

/**
 * @brief Zero Data and Memory Reallocate BIGNUM *A
 * @details
 * - ��� ���� 0 ���� ���� �� 
 * - BIGNUM *A �޸𸮸� �Է¹��� ũ��� �����ϰ� ���Ҵ�
 * @param[out] BIGNUM *A
 * @param[in] UNWORD size (const)
 * @date 2017. 03. 30. v1.00 \n
 */
void BN_Zero_Realloc_Mem(BIGNUM *A, const UNWORD size)
{	
	// �Է� ���� ũ�� ��ŭ �޸� ���Ҵ�
	BN_Realloc_Mem(A, size);
	// ���Ҵ�� �κ� 0 ���� �ʱ�ȭ
	BN_Zeroize(A);	
}

/**
 * @brief Optimize Output of BIGNUM *A
 * @details
 * - BIGNUM *A ���� ũ�� ����ȭ ��� (Memory, Length)
 * - BIGNUM *A �� 0 �� ��� *A �� Sign = ZERO
 * - �ֻ��� Num WORD ���� ���� 0 ���� üũ
 * @param[in, out] BIGNUM *A
 * @date 2017. 03. 28. v1.00 \n
 * @date 2017. 04. 04. v1.01 \n
 */
void BN_Optimize_Out(BIGNUM *A)
{
	// Length�� 1 �̻��̰�, �ֻ��� WORD ���� 0 �� ��츸 ���Ҵ�
	if((A->Length > 1) && (A->Num[A->Length - 1] == 0))
	{
		//  �ּ� A->Length �� : 1
		while((A->Length > 1) && (A->Num[A->Length - 1] == 0))
			A->Length--;
			
		A->Num = (UNWORD *)realloc(A->Num, A->Length * sizeof(UNWORD));
	}
	
	// A->Length = 1 , A-Num[0] = 0 �� ��� Sign = ZERO ����
	if((A->Length == 1) && (A->Num[0] == 0))
		A->Sign = ZERO; 
}

/**
 * @brief BIGNUM Copy A to R
 * @details
 * - BIGNUM *A ���� BIGNUM *R �� ���� (*A �� ���� ũ�� *R ���) \n
 * - ���ο��� BIGNUM *A �� ������ ũ�� BIGNUM *R ���Ҵ� \n
 * @param[in,out] BIGNUM *R
 * @param[in] BIGNUM *A (const)
 * @date 2017. 03. 28. v1.00 \n
 */
void BN_Copy(BIGNUM *R, const BIGNUM *A)
{
	SNWORD i;
		
	// *R ũ��� *A �� �ٸ��� ������ ũ���� 0 �迭�� ���Ҵ�
	if(R->Length != A->Length)
		BN_Zero_Realloc_Mem(R, A->Length);
	
	R->Sign = A->Sign;
	R->Flag = A->Flag;

	for(i = R->Length - 1 ; i >= 0 ; i--) 
		R->Num[i] = A->Num[i];
}

/**
 * @brief Absolute Value Compare BIGNUM *A to BIGNUM *B
 * @details
 * - BIGNUM *A ���� BIGNUM *B �� ���밪 ũ�� �� (Length, WORD Num ��)\n
 * - BIGNUM *A �������� A > B -> return LARGE(1) \n
 * - 1) Length �� �� \n
 * - 2) �ֻ��� Num WORD���� �� �� \n
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @return LARGE(1), EQUAL(0), SMALL(-1)
 * @date 2017. 03. 28. v1.00 \n
 */
SNWORD BN_Abs_Cmp(const BIGNUM *A, const BIGNUM *B)
{
	SNWORD i;
	
	// 1) A �� B �� WORD Length(����) ���� ��
	if(A->Length == B->Length)
	{	
		// �ֻ��� WORD ���� ũ�� ��
		for(i = A->Length - 1 ; i >= 0 ; i--)
			if(A->Num[i] != B->Num[i]) // A == B �� ��� Skip
				return (A->Num[i] > B->Num[i]) ? LARGE : SMALL;
		
		// ��� WORD�� ���ؼ� A == B �̸� �� BIGNUM ����
		return EQUAL;
	}
	else // 2) A �� B �� WORD Length(����) �ٸ� ��
		return (A->Length > B->Length) ? LARGE : SMALL; // A > B �̸� LARGE ����
}

/**
 * @brief Compare BIGNUM *A to BIGNUM *B
 * @details
 * - BIGNUM *A ���� BIGNUM *B �� ũ�� �� (Sign, Length, WORD Num ��)
 * - BIGNUM *A �������� A > B -> return LARGE(1) 
 * - Sign �� �� BN_Abs_Cmp() ��
 * @param[in] BIGNUM *A (const)
 * @param[in] BIGNUM *B (const)
 * @return LARGE(1), EQUAL(0), SMALL(-1)
 * @date 2017. 03. 28. v1.00 \n
 */
SNWORD BN_Cmp(const BIGNUM *A, const BIGNUM *B)
{	
	// A, B Sign �ٸ� ��
	if(A->Sign != B->Sign)
		return (A->Sign > B->Sign) ? LARGE : SMALL;
	else // A, B Sign ���� �� BN_Abs_Cmp() ���
	{
		if(A->Sign == PLUS)
			return BN_Abs_Cmp(A, B); // |A > B| => (A > B) : retun LARGE(1)
		else // ������ ���� ���� �� ���� ���� �� ŭ
			return BN_Abs_Cmp(B, A); // |A < B| => (-A > -B) : retun LARGE(1)
	}
}

/**
 * @brief BIGNUM n-bit Right Shift  
 * @details
 * - BIGNUM *A ���������� n ��Ʈ ����Ʈ \n
 * - BN_RShift_Bit(R, R, s_bit) ���� ���� ���� (���� ���� ������ ����)\n
 * @param[out] BIGNUM *R 
 * @param[in] BIGNUM *A (const)
 * @param[in] s_bit (const)
 * @date 2017. 03. 29. v1.00 \n
 * @todo test �ʿ�
 */
void BN_RShift_Bit(BIGNUM *R, const BIGNUM *A, const UNWORD s_bit)
{
	UNWORD i;
	UNWORD tmp_word = s_bit / BIT_LEN;
	UNWORD tmp_bit = s_bit % BIT_LEN;

	if(s_bit < BIT_LEN)	
	{
		for(i = 0 ; i < (A->Length - 1) ; i++)
			R->Num[i] = (A->Num[i + 1] << (BIT_LEN - s_bit)) ^ (A->Num[i] >> s_bit);
		// �ֻ��� WORD ó��
		R->Num[i] = (A->Num[i] >> s_bit);
	}
	else // s_bit >= BIT_LEN (���� �̵�)
	{
		for(i = 0 ; (i + tmp_word) < A->Length ; i++)
			R->Num[i] = A->Num[i + tmp_word];
		
		for( ; i < (A->Length - 1) ; i++)
			R->Num[i] = 0;
				
		for(i = 0 ; i < ((A->Length - tmp_word) - 1) ; i++)
			R->Num[i] = (A->Num[i + 1] << (BIT_LEN - s_bit)) ^ (A->Num[i] >> s_bit);
		// �ֻ��� WORD ó��
		R->Num[i] = (A->Num[i] >> s_bit);
	}

	if(A->Flag == OPTIMIZE)
		BN_Optimize_Out(R);
	
}

/**
 * @brief BIGNUM n-bit Left Shift  
 * @details
 * - BIGNUM *A �������� n ��Ʈ ����Ʈ \n
 * - BN_RShift_Bit(R, R, s_bit) ���� ���� ���� (���� ���� ������ ����)\n
 * @param[out] BIGNUM *R 
 * @param[in] BIGNUM *A (const)
 * @param[in] s_bit (const)
 * @date 2017. 03. 29. v1.00 \n
 * @todo test �ʿ�
 */
void BN_LShift_Bit(BIGNUM *R, const BIGNUM *A, const UNWORD s_bit)
{
	SNWORD i;
	//UNWORD bit_cnt;
	
	
	UNWORD tmp_word;
	UNWORD tmp_bit;
	UNWORD word_len = A->Length - 1;
	UNWORD zero_cnt = 0;

	for(i = BIT_LEN - 1 ; i >= 0 ; i--)
	{
		// ���� WORD ���� 0 �� bit üũ
		if((A->Num[word_len] >> i) == 0)
			zero_cnt++;
		else
			break;
		
		// ���� WORD ���� ��ü WORD �� 0 �̸� A ���� ���� & for �� �ٽ� ����
		if((i == 0) && ((A->Num[word_len] >> i) == 0))
		{
			word_len--;
			i = (BIT_LEN - 1);
		}
	}

	// zero_cnt >= s_bit �̸� �迭 ���� ���� ����� �ʿ� X
	if(zero_cnt < s_bit)
	{
		tmp_word = (s_bit - zero_cnt) / BIT_LEN;
		tmp_bit = (s_bit - zero_cnt) % BIT_LEN;
		BN_Realloc_Mem(&A, (A->Length + tmp_word));
		
		for(i = (A->Length - 1) ; (i - tmp_word) >= 0 ; i--)
			R->Num[i] = A->Num[i - tmp_word];
		
		for( ; i < (A->Length - 1) ; i++)
			R->Num[i] = 0;
				
		for(i = 0 ; i < ((A->Length - tmp_word) - 1) ; i++)
			R->Num[i] = (A->Num[i + 1] << (BIT_LEN - s_bit)) ^ (A->Num[i] >> s_bit);
		// �ֻ��� WORD ó��
		R->Num[i] = (A->Num[i] >> s_bit);
	}








	if(s_bit < BIT_LEN)	
	{
		for(i = 0 ; i < (A->Length - 1) ; i++)
			R->Num[i] = (A->Num[i + 1] << (BIT_LEN - s_bit)) ^ (A->Num[i] >> s_bit);
		// �ֻ��� WORD ó��
		R->Num[i] = (A->Num[i] >> s_bit);
	}
	else // s_bit >= BIT_LEN (���� �̵�)
	{
		tmp_word = s_bit / BIT_LEN;
		tmp_bit = s_bit % BIT_LEN;

		for(i = 0 ; i < (A->Length - tmp_word) ; i++)
			R->Num[i] = R->Num[i + tmp_word];
		
		for( ; i < (A->Length - 1) ; i++)
			R->Num[i] = 0;
				
		for(i = 0 ; i < ((A->Length - tmp_word) - 1) ; i++)
			R->Num[i] = (A->Num[i + 1] << (BIT_LEN - s_bit)) ^ (A->Num[i] >> s_bit);
		// �ֻ��� WORD ó��
		R->Num[i] = (A->Num[i] >> s_bit);
	}

	if(A->Flag == OPTIMIZE)
		BN_Optimize_Out(R);
	
}


/**
 * @brief BIGNUM Binary GCD algorithm 
 * @details
 * - BIGNUM *a, *b : Positive Integer
 * - BIGNUM *a �� *b �� �ִ����� BIGNUM *R ���
 * - Guide to ECC p.41 ����
 * @param[in] BIGNUM *a (const)
 * @param[in] BIGNUM *b (const)
 * @param[out] BIGNUM *R 
 * @date 2017. 03. 28. v1.00 \n
 * @todo ���� �̿ϼ�
 */
void BN_Bin_GCD(BIGNUM *R, const BIGNUM *a, const BIGNUM *b)
{	

	BIGNUM u, v, e;

	BN_Init_Copy(&u, a);
	BN_Init_Copy(&v, b);
	BN_Init(&e, 1, PLUS, DEFAULT);
	e.Num[0] = 1;
	
	while(((u.Num[0] & 1) == 0) && ((v.Num[0] & 1) == 0)) // u[0] and v[0] �� ¦���̸� �ݺ�
	{
		BN_RShift_Bit(&u, &u, 1); // u <- u/2
		BN_RShift_Bit(&v, &v, 1); // v <- v/2
		e <<= 1;				  // e <- 2e
	}

	while(u != 0)
	{
		while((u[0] & 1) == 0)
			BN_RShift_Bit(&u, &u, 1); // u <- u/2		
		while((v[0] & 1) == 0)		
			BN_RShift_Bit(&v, &v, 1); // v <- v/2	
		
		if(BN_Cmp(&u, &v))
			BN_Sub(&u, &u, &v);
		else
			BN_Sub(&v, &v, &u);
	}

	BN_UNWORD_Mul(R, v, e);
	
}

/**
 * @brief BIGNUM Extended Binary GCD algorithm 
 * @details
 * - BIGNUM *x, *y : Positive Integer
 * - BIGNUM *v = gcd(x, y), BIGNUM *a and *b s.t. ax + by = gcd(x, y) 
 * - Handbook of Applied Cryptography p.608 ����
 * @param[out] BIGNUM *v \n
 * @param[out] BIGNUM *a \n
 * @param[out] BIGNUM *b \n
 * @param[in] BIGNUM *x (const)
 * @param[in] BIGNUM *y (const)
 * @date 2017. 03. 28. v1.00 \n
 * @todo ���� �̿ϼ�
 */
void BN_Ex_Bin_GCD(BIGNUM *v, BIGNUM *a, BIGNUM *b, const BIGNUM *x, const BIGNUM *y)
{	
	/*
	BIGNUM a;

	while(((a[0] & 1) == 0) && ((b[0] & 1) == 0)) // a[0] and b[0] �� ¦���̸� �ݺ�
	{
		BN_RShift_Bit(&u, &u, 1); // u <- u/2
		BN_RShift_Bit(&v, &v, 1); // v <- v/2
		e <<= 1;				  // e <- 2e
	}

	*/

}