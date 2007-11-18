#include "stdafx.h"
#include "CWordParse.h"
#include "charcode.h"


//@@@ 2001.06.23 N.Nakatani
/*!
	@brief ���݈ʒu�̒P��͈̔͂𒲂ׂ� static�����o
	@author N.Nakatani
	@retval true	���� ���݈ʒu�̃f�[�^�́u�P��v�ƔF������B
	@retval false	���s ���݈ʒu�̃f�[�^�́u�P��v�Ƃ͌�������Ȃ��C������B
*/
bool CWordParse::WhereCurrentWord_2(
	const wchar_t*	pLine,			//[in]  ���ׂ郁�����S�̂̐擪�A�h���X
	CLogicInt		nLineLen,		//[in]  ���ׂ郁�����S�̗̂L����
	CLogicInt		nIdx,			//[out] �����J�n�n�_:pLine����̑��ΓI�Ȉʒu
	CLogicInt*		pnIdxFrom,		//[out] �P�ꂪ���������ꍇ�́A�P��̐擪�C���f�b�N�X��Ԃ��B
	CLogicInt*		pnIdxTo,		//[out] �P�ꂪ���������ꍇ�́A�P��̏I�[�̎��̃o�C�g�̐擪�C���f�b�N�X��Ԃ��B
	CNativeW*		pcmcmWord,		//[out] �P�ꂪ���������ꍇ�́A���ݒP���؂�o���Ďw�肳�ꂽCMemory�I�u�W�F�N�g�Ɋi�[����B��񂪕s�v�ȏꍇ��NULL���w�肷��B
	CNativeW*		pcmcmWordLeft	//[out] �P�ꂪ���������ꍇ�́A���ݒP��̍��Ɉʒu����P���؂�o���Ďw�肳�ꂽCMemory�I�u�W�F�N�g�Ɋi�[����B��񂪕s�v�ȏꍇ��NULL���w�肷��B
)
{
	using namespace WCODE;

	*pnIdxFrom = nIdx;
	*pnIdxTo = nIdx;

	if( NULL == pLine ){
		return false;
	}
	if( nIdx >= nLineLen ){
		return false;
	}

	// ���݈ʒu�̕����̎�ނɂ���Ă͑I��s�\
	if( pLine[nIdx] == CR || pLine[nIdx] == LF ){
		return false;
	}

	// ���݈ʒu�̕����̎�ނ𒲂ׂ�
	ECharKind nCharKind = WhatKindOfChar( pLine, nLineLen, nIdx );

	// ������ނ��ς��܂őO���փT�[�`
	CLogicInt	nIdxNext = nIdx;
	CLogicInt	nCharChars = CLogicInt(&pLine[nIdxNext] - CNativeW::GetCharPrev( pLine, nLineLen, &pLine[nIdxNext] ));
	while( nCharChars > 0 ){
		CLogicInt	nIdxNextPrev = nIdxNext;
		nIdxNext -= nCharChars;
		ECharKind	nCharKindNext = WhatKindOfChar( pLine, nLineLen, nIdxNext );
		if( nCharKind == CK_ZEN_NOBASU ){
			if( nCharKindNext == CK_HIRA ||
				nCharKindNext == CK_ZEN_KATA ){
				nCharKind = nCharKindNext;
			}
		}
		else if( nCharKind == CK_HIRA ||
			nCharKind == CK_ZEN_KATA ){
			if( nCharKindNext == CK_ZEN_NOBASU ){
				nCharKindNext = nCharKind;
			}
		}

		if( nCharKind != nCharKindNext ){
			nIdxNext = nIdxNextPrev;
			break;
		}
		nCharChars = CLogicInt(&pLine[nIdxNext] - CNativeW::GetCharPrev( pLine, nLineLen, &pLine[nIdxNext] ));
	}
	*pnIdxFrom = nIdxNext;

	if( NULL != pcmcmWordLeft ){
		pcmcmWordLeft->SetString( &pLine[*pnIdxFrom], nIdx - *pnIdxFrom );
	}

	// ������ނ��ς��܂Ō���փT�[�`
	nIdxNext = nIdx;
	nCharChars = CNativeW::GetSizeOfChar( pLine, nLineLen, nIdxNext ); // 2005-09-02 D.S.Koba GetSizeOfChar
	while( nCharChars > 0 ){
		nIdxNext += nCharChars;
		ECharKind	nCharKindNext = WhatKindOfChar( pLine, nLineLen, nIdxNext );
		if( nCharKind == CK_ZEN_NOBASU ){
			if( nCharKindNext == CK_HIRA || nCharKindNext == CK_ZEN_KATA ){
				nCharKind = nCharKindNext;
			}
		}
		else if( nCharKind == CK_HIRA || nCharKind == CK_ZEN_KATA ){
			if( nCharKindNext == CK_ZEN_NOBASU ){
				nCharKindNext = nCharKind;
			}
		}

		if( nCharKind != nCharKindNext ){
			break;
		}
		nCharChars = CNativeW::GetSizeOfChar( pLine, nLineLen, nIdxNext ); // 2005-09-02 D.S.Koba GetSizeOfChar
	}
	*pnIdxTo = nIdxNext;

	if( NULL != pcmcmWord ){
		pcmcmWord->SetString( &pLine[*pnIdxFrom], *pnIdxTo - *pnIdxFrom );
	}
	return true;
}



//! ���ʎq�Ɏg�p�\�ȕ������ǂ���
inline bool isCSymbol(wchar_t c)
{
	return
		(c==L'_') ||
		(c>=L'0' && c<=L'9') ||
		(c>=L'A' && c<=L'Z') ||
		(c>=L'a' && c<=L'z');
}

//! �S�p�ŁA���ʎq�Ɏg�p�\�ȕ������ǂ���
inline bool isCSymbolZen(wchar_t c)
{
	return
		(c==L'�Q') ||
		(c>=L'�O' && c<=L'�X') ||
		(c>=L'�`' && c<=L'�y') ||
		(c>=L'��' && c<=L'��');
}



// ���݈ʒu�̕����̎�ނ𒲂ׂ�
ECharKind CWordParse::WhatKindOfChar(
	const wchar_t*	pData,
	int				pDataLen,
	int				nIdx
)
{
	int nCharChars = CNativeW::GetSizeOfChar( pData, pDataLen, nIdx );
	if( nCharChars == 0 ){
		return CK_NULL;	// NULL
	}
	else if( nCharChars == 1 ){
		using namespace WCODE;
		wchar_t c=pData[nIdx];

		//���܂ł̔��p
		if( c == CR              )return CK_CR;
		if( c == LF              )return CK_LF;
		if( c == TAB             )return CK_TAB;	// �^�u
		if( c == SPACE           )return CK_SPACE;	// ���p�X�y�[�X
		if( isCSymbol(c)         )return CK_CSYM;	// ���ʎq�Ɏg�p�\�ȕ��� (���p�p�����A���p�A���_�[�X�R�A)
		if( isHankakuKatakana(c) )return CK_KATA;	// ���p�̃J�^�J�i

		//���̑�
		if( isZenkakuSpace(c)    )return CK_ZEN_SPACE;	// �S�p�X�y�[�X
		if( c==L'�['             )return CK_ZEN_NOBASU;	// �L�΂��L�� '�['
		if( isCSymbolZen(c)      )return CK_ZEN_CSYM;	// �S�p�ŁA���ʎq�Ɏg�p�\�ȕ��� 
		if( isZenkakuKigou(c)    )return CK_ZEN_KIGO;	// �S�p�̋L��
		if( isHiragana(c)        )return CK_HIRA;		// �Ђ炪��
		if( isZenkakuKatakana(c) )return CK_ZEN_KATA;	// �S�p�J�^�J�i
		if( isGreek(c)           )return CK_GREEK;		// �M���V������
		if( isCyrillic(c)        )return CK_ZEN_ROS;	// ���V�A����
		if( isBoxDrawing(c)      )return CK_ZEN_SKIGO;	// �S�p�̓���L��

		//������
		if( isHankaku(c) )return CK_ETC;	// ���p�̂��̑�
		else return CK_ZEN_ETC;				// �S�p�̂��̑�(�����Ȃ�)
	}
	else{
		return CK_NULL;	// NULL
	}
}





/*!	���̒P��̐擪��T��
	pLine�i�����FnLineLen�j�̕����񂩂�P���T���B
	�T���n�߂�ʒu��nIdx�Ŏw��B�����͌���Ɍ���B�P��̗��[�Ŏ~�܂�Ȃ��i�֌W�Ȃ�����j
*/
bool CWordParse::SearchNextWordPosition(
	const wchar_t*	pLine,
	CLogicInt		nLineLen,
	CLogicInt		nIdx,		//	����
	CLogicInt*		pnColmNew,	//	���������ʒu
	BOOL			bStopsBothEnds	//	�P��̗��[�Ŏ~�܂�
)
{
	// ������ނ��ς��܂Ō���փT�[�`
	// �󔒂ƃ^�u�͖�������

	// ���݈ʒu�̕����̎�ނ𒲂ׂ�
	ECharKind nCharKind = WhatKindOfChar( pLine, nLineLen, nIdx );

	CLogicInt nIdxNext = nIdx;
	// 2005-09-02 D.S.Koba GetSizeOfChar
	CLogicInt nCharChars = CNativeW::GetSizeOfChar( pLine, nLineLen, nIdxNext );
	while( nCharChars > 0 ){
		nIdxNext += nCharChars;
		ECharKind nCharKindNext = WhatKindOfChar( pLine, nLineLen, nIdxNext );
		// �󔒂ƃ^�u�͖�������
		if( nCharKindNext == CK_TAB || nCharKindNext == CK_SPACE ){
			if ( bStopsBothEnds && nCharKind != nCharKindNext ){
				*pnColmNew = nIdxNext;
				return true;
			}
			nCharKind = nCharKindNext;
		}
		else {
			if( nCharKind == CK_ZEN_NOBASU ){
				if( nCharKindNext == CK_HIRA || nCharKindNext == CK_ZEN_KATA ){
					nCharKind = nCharKindNext;
				}
			}
			else if( nCharKind == CK_HIRA || nCharKind == CK_ZEN_KATA ){
				if( nCharKindNext == CK_ZEN_NOBASU ){
					nCharKindNext = nCharKind;
				}
			}

			if( nCharKind != nCharKindNext ){
				*pnColmNew = nIdxNext;
				return true;
			}
		}
		// 2005-09-02 D.S.Koba GetSizeOfChar
		nCharChars = CNativeW::GetSizeOfChar( pLine, nLineLen, nIdxNext );
	}
	return false;
}



//! wc��SJIS1�o�C�g�����Ȃ�char�ɕϊ�����0�`255��Ԃ��BSJIS2�o�C�g�����Ȃ�0��Ԃ��B
uchar_t wc_to_c(wchar_t wc)
{
	char buf[3]={0,0,0};
	int ret=wctomb(buf,wc);
	if(ret==-1)return 0;   //�G���[
	if(buf[1]!=0)return 0; //�G���[����
	return buf[0];         //1�o�C�g�ŕ\�����̂ŁA�����Ԃ�
}

//@@@ 2002.01.24 Start by MIK
/*!
	������URL���ǂ�������������B
	
	@param pszLine [in] ������
	@param nLineLen [in] ������̒���
	@param pnMatchLen [out] URL�̒���
	
	@retval TRUE URL�ł���
	@retval FALSE URL�łȂ�
	
	@note �֐����ɒ�`�����e�[�u���͕K�� static const �錾�ɂ��邱��(���\�ɉe�����܂�)�B
		url_char �̒l�� url_table �̔z��ԍ�+1 �ɂȂ��Ă��܂��B
		�V���� URL ��ǉ�����ꍇ�� #define �l���C�����Ă��������B
		url_table �͓��������A���t�@�x�b�g���ɂȂ�悤�ɕ��ׂĂ��������B

	2007.10.23 kobake UNICODE�Ή��B//$ wchar_t��p�̃e�[�u��(�܂��͔��胋�[�`��)��p�ӂ����ق��������͏オ��͂��ł��B
*/
BOOL IsURL( const wchar_t *pszLine, int nLineLen, int *pnMatchLen )
{
	struct _url_table_t {
		wchar_t	name[12];
		int		length;
		bool	is_mail;
	};
	static const struct _url_table_t	url_table[] = {
		/* �A���t�@�x�b�g�� */
		L"file://",		7,	false, /* 1 */
		L"ftp://",		6,	false, /* 2 */
		L"gopher://",	9,	false, /* 3 */
		L"http://",		7,	false, /* 4 */
		L"https://",	8,	false, /* 5 */
		L"mailto:",		7,	true,  /* 6 */
		L"news:",		5,	false, /* 7 */
		L"nntp://",		7,	false, /* 8 */
		L"prospero://",	11,	false, /* 9 */
		L"telnet://",	9,	false, /* 10 */
		L"tp://",		5,	false, /* 11 */	//2004.02.02
		L"ttp://",		6,	false, /* 12 */	//2004.02.02
		L"wais://",		7,	false, /* 13 */
		L"{",			0,	false  /* 14 */  /* '{' is 'z'+1 : terminate */
	};

/* �e�[�u���̕ێ琫�����߂邽�߂̒�` */
	const char urF = 1;
	const char urG = 3;
	const char urH = 4;
	const char urM = 6;
	const char urN = 7;
	const char urP = 9;
	const char urT = 10;
	const char urW = 13;	//2004.02.02

	static const char	url_char[] = {
	  /* +0  +1  +2  +3  +4  +5  +6  +7  +8  +9  +A  +B  +C  +D  +E  +F */
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,	/* +00: */
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,	/* +10: */
		  0, -1,  0, -1, -1, -1, -1,  0,  0,  0,  0, -1, -1, -1, -1, -1,	/* +20: " !"#$%&'()*+,-./" */
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1,  0, -1,	/* +30: "0123456789:;<=>?" */
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	/* +40: "@ABCDEFGHIJKLMNO" */
		 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1,  0,  0, -1,	/* +50: "PQRSTUVWXYZ[\]^_" */
		  0, -1, -1, -1, -1, -1,urF,urG,urH, -1, -1, -1, -1,urM,urN, -1,	/* +60: "`abcdefghijklmno" */
		urP, -1, -1, -1,urT, -1, -1,urW, -1, -1, -1,  0,  0,  0, -1,  0,	/* +70: "pqrstuvwxyz{|}~ " */
		/* ����128�o�C�g�]���ɂ����if����2�ӏ��폜�ł��� */
		/* 0    : not url char
		 * -1   : url char
		 * other: url head char --> url_table array number + 1
		 */
	};

	const wchar_t *p = pszLine;
	const struct _url_table_t	*urlp;
	int	i;

	if( wc_to_c(*p)==0 ) return FALSE;	/* 2�o�C�g���� */
	if( 0 < url_char[wc_to_c(*p)] ){	/* URL�J�n���� */
		for(urlp = &url_table[url_char[wc_to_c(*p)]-1]; urlp->name[0] == wc_to_c(*p); urlp++){	/* URL�e�[�u����T�� */
			if( (urlp->length <= nLineLen) && (auto_memcmp(urlp->name, pszLine, urlp->length) == 0) ){	/* URL�w�b�_�͈�v���� */
				p += urlp->length;	/* URL�w�b�_�����X�L�b�v���� */
				if( urlp->is_mail ){	/* ���[����p�̉�͂� */
					if( IsMailAddress(p, nLineLen - urlp->length, pnMatchLen) ){
						*pnMatchLen = *pnMatchLen + urlp->length;
						return TRUE;
					}
					return FALSE;
				}
				for(i = urlp->length; i < nLineLen; i++, p++){	/* �ʏ�̉�͂� */
					if( wc_to_c(*p)==0 || (!(url_char[wc_to_c(*p)])) ) break;	/* �I�[�ɒB���� */
				}
				if( i == urlp->length ) return FALSE;	/* URL�w�b�_���� */
				*pnMatchLen = i;
				return TRUE;
			}
		}
	}
	return IsMailAddress(pszLine, nLineLen, pnMatchLen);
}

/* ���݈ʒu�����[���A�h���X�Ȃ�΁ANULL�ȊO�ƁA���̒�����Ԃ� */
BOOL IsMailAddress( const wchar_t* pszBuf, int nBufLen, int* pnAddressLenfth )
{
	int		j;
	int		nDotCount;
	int		nBgn;


	j = 0;
	if( (pszBuf[j] >= L'a' && pszBuf[j] <= L'z')
	 || (pszBuf[j] >= L'A' && pszBuf[j] <= L'Z')
	 || (pszBuf[j] >= L'0' && pszBuf[j] <= L'9')
	){
		j++;
	}else{
		return FALSE;
	}
	while( j < nBufLen - 2 &&
		(
		(pszBuf[j] >= L'a' && pszBuf[j] <= L'z')
	 || (pszBuf[j] >= L'A' && pszBuf[j] <= L'Z')
	 || (pszBuf[j] >= L'0' && pszBuf[j] <= L'9')
	 || (pszBuf[j] == L'.')
	 || (pszBuf[j] == L'-')
	 || (pszBuf[j] == L'_')
		)
	){
		j++;
	}
	if( j == 0 || j >= nBufLen - 2  ){
		return FALSE;
	}
	if( L'@' != pszBuf[j] ){
		return FALSE;
	}
//	nAtPos = j;
	j++;
	nDotCount = 0;
//	nAlphaCount = 0;


	while( 1 ){
		nBgn = j;
		while( j < nBufLen &&
			(
			(pszBuf[j] >= L'a' && pszBuf[j] <= L'z')
		 || (pszBuf[j] >= L'A' && pszBuf[j] <= L'Z')
		 || (pszBuf[j] >= L'0' && pszBuf[j] <= L'9')
		 || (pszBuf[j] == L'-')
		 || (pszBuf[j] == L'_')
			)
		){
			j++;
		}
		if( 0 == j - nBgn ){
			return FALSE;
		}
		if( L'.' != pszBuf[j] ){
			if( 0 == nDotCount ){
				return FALSE;
			}else{
				break;
			}
		}else{
			nDotCount++;
			j++;
		}
	}
	if( NULL != pnAddressLenfth ){
		*pnAddressLenfth = j;
	}
	return TRUE;
}