//--------------------------------------------------------------
// Name: boolean set class
// Author: lh 2004-11-01
// Purpose: can quickly tell whether all the booleans are false
//--------------------------------------------------------------

#pragma once

class CBoolSet
{
public:
	CBoolSet() = default;

	// bit range 0~31
	bool IsTrue( unsigned int bit )		{ return (_value & 0x00000001 << bit)!=0;	}
	bool IsFalse( unsigned int bit )    { return (_value & 0x00000001 << bit)==0;	}
	void SetTrue( unsigned int bit )	{ _value |= 0x00000001 << bit;				}	// the bit is true
	void SetFalse( unsigned int bit )	{ _value &= ~(0x00000001 << bit);			}	// the bit is false

	void Set( unsigned int bit, bool v ){ if(v) SetTrue(bit); else SetFalse(bit);	}
	bool Get( unsigned int bit )		{ return (_value & 0x00000001 << bit)!=0;	}

	void AllFalse()						{ _value = 0;			}	// all false
	void AllTrue()						{ _value = 0xffffffff;	}	// all true

	bool IsNone()						{ return _value==0;		}	// whether all are false
	bool IsAny()						{ return _value!=0;		}	// whether any bit is true

private:
	uint32_t _value{ 0 };
};
