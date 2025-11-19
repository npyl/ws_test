#ifndef TYPECONV_H
#define TYPECONV_H

#include "fw_globals.h"

INT8U f_TypeConv_BufferASCII_Valid(PINT8U p_buffer, INT8U v_len);
INT8U f_TypeConv_BufferASCIIHEX_Valid(PINT8U p_buffer, INT8U v_len);

INT8U * f_TypeConv_CopyString(INT8U *p_str1, INT8U *p_output, INT8U v_maxlen);

INT8U f_TypeConv_CmpString(INT8U *p_str1, INT8U *p_str2);
INT8U f_TypeConv_CmpBuffer16(INT16U *p_str1, INT16U *p_str2, INT16U v_len);
INT16U f_TypeConv_StringLen(INT8U *p_str1);
INT8U f_TypeConv_StringLenPacket(INT8U *p_str1);

void f_TypeConv_BinHex8(INT8U bin, INT8 *str);
void f_TypeConv_BinHex16(INT16U v_bin, INT8 *str);
void f_TypeConv_BinHex32(INT32U v_bin, INT8 *str);

INT8U f_TypeConv_HexBin8(INT8U *hex);
INT16U f_TypeConv_HexBin16(INT8U *hex);
INT32U f_TypeConv_HexBin32(INT8U *hex);

void f_TypeConv_Dec2Bin(INT32 decimal, INT8U *binary);
INT32 f_TypeConv_Ascii2Int(INT8* p_string);


void f_TypeConv_HexToString(PINT8U p_data_hex, PINT8U p_data_str, INT8U v_len);

INT16U f_TypeConv_SwapEndian16(INT16U v_BigEndian);
INT32U f_TypeConv_SwapEndian32(INT32U v_BigEndian);

#endif /* #ifndef TYPECONV_H */
/* *************** END OF FILE TYPECONV_H ****************************** */
