#pragma once

#include "CConvert.h"


//!できる限り全角ひらがなにする
class CConvert_ToZenhira : public CConvert{
public:
	bool DoConvert(CNativeW2* pcData);
};
