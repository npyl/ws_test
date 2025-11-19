#include <stdlib.h>
#include <stdio.h>

#include "Typeconv.h"
#include "fw_globals.h"

#define EOP 0x03

/*!
 * \brief Sanity check for valid ASCII chars in a packet
 * @param p_buffer: Pointer to buffer with ASCII data
 * @param v_len: Length of data to check
 * @return: 0: for non-ASCII char found, 1: for valid ASCII string
 */

INT8U f_TypeConv_BufferASCII_Valid(PINT8U p_buffer, INT8U v_len)
{
    INT8U v_idx;
    INT8U v_data;

    /* Check for NULL buffer */
    if (p_buffer == NULL)
        return 0;

    /* Check for zero length */
    if (v_len == 0)
        return 0;

    /* Check each character */
    for(v_idx = 0; v_idx < v_len; v_idx++)
    {
        v_data = *(p_buffer + v_idx);

        /* Check if character is in printable ASCII range */
        if ((v_data < 0x20) || (v_data > 0x7E))
        {
            return 0;  /* Invalid character found */
        }
    }

    return 1;  /* All characters valid */
}


/*!
 * \brief Sanity check for valid Hex ASCII chars (0-9A-F) in a packet
 * @param p_buffer: Pointer to buffer with Hex ASCII data
 * @param v_len: Length of data to check
 * @return: 0: for invalid number, 1: for valid ASCII Hex number
 */
INT8U f_TypeConv_BufferASCIIHEX_Valid(PINT8U p_buffer, INT8U v_len)
{
    INT8U v_idx;
    INT8U v_data;
    INT8U v_result;

    /* default to Hex ASCII */
    v_result = 1;

    for(v_idx=0; v_idx<v_len; v_idx++)
    {
        v_data = *(p_buffer + v_idx);
        if ( (v_data < '0') || (v_data > '9') )
        {
            if ( (v_data < 'A') || (v_data > 'F') )
            {
                v_result = 0;
                break;
            }
        }
    }

    return v_result;
}



/*!
 * \brief Copy a NULL terminated string from one buffer to another
 * @param p_str1    : Source String pointer
 * @param p_output  : Destination String Pointer
 * @param v_maxlen  : Maximum output buffer length (limit copy)
 * @return Pointer to end of string
 * \details Check for NULL pointers. In this case (at least one pointer is NULL) no copy occurs
 */
INT8U * f_TypeConv_CopyString(INT8U *p_str1, INT8U *p_output, INT8U v_maxlen)
{
    INT8U *p_srcstr;
    INT8U *p_dststr;
    INT8U v_idx;

    p_srcstr = p_str1;
    p_dststr = p_output;

    // If pointer empty then return
    if (p_srcstr == NULL) return p_dststr;
    if (p_dststr == NULL) return p_dststr;

    v_idx = 0;

    while(*p_srcstr)
    {
        *p_dststr = *p_srcstr;
        p_srcstr++;
        p_dststr++;

        v_idx++;
        if (v_idx > v_maxlen-2)
        {
            *p_dststr = 0; /*add NULL termination and exit */
            break;
        }
    }

    return p_dststr;
}

/*!
 * \brief Compares two strings
 * @param p_str1 : Pointer to first string
 * @param p_str2 : Pointer to second string
 * @return 0 for NULL pointers, No match, or zero-length data. 1 for comparison success. 
 * \details Check for NULL pointers. In this case (at least one pointer is NULL) no comparison occurs
 */
INT8U f_TypeConv_CmpString(INT8U *p_str1, INT8U *p_str2)
{
    INT8U *p_srcstr;
    INT8U *p_dststr;
    INT8U v_idx;

    p_srcstr = p_str1;
    p_dststr = p_str2;

    // If pointer empty then return
    if (p_srcstr == NULL) return 0;
    if (p_dststr == NULL) return 0;

    v_idx = 0;
    
    while(*p_srcstr)
    {
        if ( *p_dststr != *p_srcstr) return 0;
        p_srcstr++;
        p_dststr++;
        v_idx++;
    }

    // check for NULL string input
    if (v_idx == 0) return 0;
    
    // Else indeed strings are same
    return 1;
}
// Compare two buffers 16-bit
/*!
 * \brief Compare binary buffer of given length
 * @param p_str1 : Pointer to reference binary data set (word aligned)
 * @param p_str2 : Pointer to checking binary data set (word aligned)
 * @param v_len  : Length of buffer to compare
 * @return 0 for NULL pointers, No match, or zero-length data. 1 for comparison success. 
 * \details Check for NULL pointers. In this case (at least one pointer is NULL) no comparison occurs
 */
INT8U f_TypeConv_CmpBuffer16(INT16U *p_str1, INT16U *p_str2, INT16U v_len)
{
    INT16U *p_srcstr;
    INT16U *p_dststr;
    INT16U v_idx;

    p_srcstr = p_str1;
    p_dststr = p_str2;

    // If pointer empty then return
    if (p_srcstr == NULL) return 0;
    if (p_dststr == NULL) return 0;

    v_idx = 0;
    
    for(v_idx=0; v_idx<v_len; v_idx++)
    {
        if ( *p_dststr != *p_srcstr) return 0;
        p_srcstr++;
        p_dststr++;
    }

    // check for NULL string input
    if (v_idx == 0) return 0;
    
    // Else indeed strings are same
    return 1;
}

/*!
 * \brief Compute length of null terminated string
 * @param p_str1 : Pointer to null terminated string
 * @return 0 for Null pointer or length of string.
 */
INT16U f_TypeConv_StringLen(INT8U *p_str1)
{
    INT8U *p_srcstr;
    INT16U v_idx;

    p_srcstr = p_str1;

    // If pointer empty then return
    if (p_srcstr == NULL) return 0;

    v_idx = 0;
    
    while(*p_srcstr)
    {
        p_srcstr++;
        v_idx++;
    }

    // length of string
    return v_idx;
}

/*!
 * \brief Compute length of null terminated string
 * @param p_str1 : Pointer to null terminated string
 * @return 0 for Null pointer or length of string.
 */
INT8U f_TypeConv_StringLenPacket(INT8U *p_str1)
{
    INT8U *p_srcstr;
    INT8U v_idx;

    p_srcstr = p_str1;

    // If pointer empty then return
    if (p_srcstr == NULL) return 0;

    v_idx = 0;

    while(*p_srcstr != EOP )
    {
        p_srcstr++;
        v_idx++;
    }

    // length of string
    return v_idx;
}

/*
 * Conversion Functions
 */

/*!
 * \brief Convert INT8U to Hex string
 * \details A single binary INT8U data is converted to hex ascii (2-bytes) string
 * @param bin : Binary Byte to convert
 * @param str : String (2-byte) pointer to output
 */

void f_TypeConv_BinHex8(INT8U bin, INT8 *str)
{
    const INT8 lut_Hex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *(str+0) = lut_Hex[(bin >> 4) & 0x0f];
    *(str+1) = lut_Hex[bin & 0x0f];
}


/*!
 * \brief Convert INT16U to Hex string [4-bytes ASCII]
 * @param v_bin : Binary 16-bit value to convert
 * @param str   : ASCII Hex string
 */
void f_TypeConv_BinHex16(INT16U v_bin, INT8 *str)
{
    INT8U v_byte;
    INT8 *p_mystr;

    p_mystr = str;

    v_byte = MSB16(v_bin); //(INT8U) (v_bin & 0xff);
    f_TypeConv_BinHex8(v_byte, p_mystr);
    v_byte = LSB16(v_bin); //(INT8U) ((v_bin >> 8) & 0xff);
    p_mystr += 2;
    f_TypeConv_BinHex8(v_byte, p_mystr);
}

/*!
 * \brief Convert INT32U to Hex string [8-bytes ASCII]
 * @param v_bin : Binary 32-bit value to convert
 * @param str   : ASCII Hex string
 */
void f_TypeConv_BinHex32(INT32U v_bin, INT8 *str)
{
    INT8U v_byte;
    INT8 *p_mystr;

    p_mystr = str;
    v_byte = MSB32_3(v_bin); //(INT8U) (v_bin & 0xff);
    f_TypeConv_BinHex8(v_byte, p_mystr);
    v_byte = MSB32_2(v_bin); //(INT8U) ((v_bin >> 8) & 0xff);
    p_mystr += 2;
    f_TypeConv_BinHex8(v_byte, p_mystr);

    v_byte = MSB32_1(v_bin); //(INT8U) (v_bin & 0xff);
    p_mystr += 2;
    f_TypeConv_BinHex8(v_byte, p_mystr);
    v_byte = MSB32_0(v_bin); //(INT8U) ((v_bin >> 8) & 0xff);
    p_mystr += 2;
    f_TypeConv_BinHex8(v_byte, p_mystr);
}

/*!
 * \brief Convert ASCII hex string (2-digits) to INT8U
 * @param hex   : Hex string 2-bytes long
 * @return      : INT16U value of hex string, or 0xFF in case of invalid hex digits
 *
 */
INT8U f_TypeConv_HexBin8(INT8U *hex)
{
    INT8U data = 0;
    INT8U i;
    INT8U cbyte;
    INT8U v_valid;

    /* check for valid string hex digits */
    v_valid = f_TypeConv_BufferASCIIHEX_Valid(hex, 2);
    if (v_valid == 0) return 0xFF;

    for(i=0; i<2; i++)
    {
        cbyte = *(hex+i);

        if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
        else cbyte-= '0';
        data |= (cbyte << (4*(1-i)));
    }

    return data;
}

/*!
 * \brief Convert ASCII hex string (4-digits) to INT16U
 * @param hex   : Hex string 4-bytes long
 * @return      : INT16U value of hex string, or 0xFFFF in case of invalid hex digits
 *
 */
INT16U f_TypeConv_HexBin16(INT8U *hex)
{
    INT16U data = 0;
    INT8U i;
    INT8U cbyte;
    INT8U v_valid;

    /* check for valid string hex digits */
    v_valid = f_TypeConv_BufferASCIIHEX_Valid(hex, 4);
    if (v_valid == 0) return 0xFFFF;

    // Little Endian
    cbyte = f_TypeConv_HexBin8(hex+0);
    data = (INT32U) (cbyte << 8);
    cbyte = f_TypeConv_HexBin8(hex+2);
    data |= (INT32U) (cbyte << 0);

#if 0
    for(i=0; i<4; i++)
    {
        cbyte = *(hex+i);
        // check for valid string hex digits
        if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
        else cbyte-= '0';
        data |= (cbyte << (4*(3-i)));
    }
#endif

    return data;
}

/*!
 * \brief Convert ASCII hex string (8-digits) to INT32U
 * @param hex   : Hex string 8-bytes long
 * @return      : INT32U value of hex string, or 0xFFFFFFFF in case of invalid hex digits
 *
 */
INT32U f_TypeConv_HexBin32(INT8U *hex)
{
    INT32U v_data;
    INT8U i;
    INT8U cbyte;
    INT8U v_valid;

    v_data = 0L;

    /* check for valid string hex digits */
    v_valid = f_TypeConv_BufferASCIIHEX_Valid(hex, 8);
    if (v_valid == 0) return 0xFFFFFFFF;

    // Little Endian
    cbyte = f_TypeConv_HexBin8(hex+6);
    v_data = (INT32U) (cbyte << 0);
    cbyte = f_TypeConv_HexBin8(hex+4);
    v_data |= (INT32U) (cbyte << 8);
    cbyte = f_TypeConv_HexBin8(hex+2);
    v_data |= (INT32U) (cbyte << 16);
    cbyte = f_TypeConv_HexBin8(hex+0);
    v_data |= (INT32U) (cbyte << 24);

#if 0
    for(i=0; i<8; i++)
    {

        cbyte = *(hex+i);
        // do the conversion
        if ( cbyte >= 'A') cbyte = (INT8U) ((cbyte - 'A') + 10);
        else cbyte-= '0';
        v_data |= (INT32U) (cbyte << (4*(7-i)));
    }

#endif

    return v_data;
}


/*!
 * \brief accepts a decimal integer and returns a binary coded string
 * @param decimal : INT32 binary number
 * @param binary  : String of binary digits (0/1) representing the binary value
 */
void f_TypeConv_Dec2Bin(INT32 decimal, INT8U *binary)
{
    int k = 0, n = 0;
    int neg_flag = 0;
    int remain;
    //int old_decimal; // for test
    INT8U temp[80];
    
    // take care of negative input
    
    if (decimal < 0)
    {
        decimal = -decimal;
        neg_flag = 1;
    }
    
    do
    {
        //old_decimal = decimal; // for test
        remain = decimal % 2;
        // whittle down the decimal number
        decimal = decimal / 2;
        // this is a test to show the action
        //printf("%d/2 = %d remainder = %d\n", old_decimal, decimal, remain);
        // converts digit 0 or 1 to character '0' or '1'
        temp[k++] = (unsigned char) (remain + '0');
    } while (decimal > 0);
 
    if (neg_flag)
        temp[k++] = '-'; // add - sign
    else
    temp[k++] = ' '; // space

    // reverse the spelling
    while (k >= 0)
        binary[n++] = temp[--k];
    
    binary[n-1] = 0; // end with NULL
}

/*!
 * 
 * \brief Convert ASCII decimal string to integer
 * @param p_string : Pointer to decimal string
 * @return Integer of decimal string
 */
#if 0
INT32 f_TypeConv_Ascii2Int(INT8* p_string)
{
    INT32 v_result;
    INT8 *p_char;
    INT8U v_idx;
    INT8U v_maxidx;
    INT8U v_flag;
    INT8U v_temp;
    INT8 v_sign;

    // Max Integer (Unsigned) = 4294967296
    // Max Integer (Signed)   = +/-2147483648
    v_idx = 0;
    p_char = p_string;
    v_flag = 1;
    v_result = 0;
    v_sign = 1; // assume positive number initially

    while ( v_flag == 1)
    {
        // Check input string for valid chars
        // Accept +/- only on the beginning
        switch (*p_char)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                p_char++;
                v_idx++;
                break;

            case '-':
                v_sign = -1;  // reverse sign when needed
            case '+':
                // on positive sign do nothing, we already assumed that
                // Check proper notation, +/- sign allowed only in the beginning
                if (v_idx != 0)
                {
                    v_flag = 0;
                }
                else
                {
                    p_char++;
                    v_idx++;
                }
                break;

            case 0: // normal termination
            case ' ':
                v_flag = 2;
                break;

            default:
                v_flag = 0;
        }

        // Too many chars stop further processing
        if ( v_idx > 11) v_flag = 0;

    }

    // if no valid chars processed
    if ( v_idx == 0) v_flag = 0;

    // if v_idx == 0 then return default value (0)
    if (v_flag != 0)
    {
        // valid data, process
        --p_char;
        v_maxidx = v_idx;
        for (v_idx=0; v_idx < v_maxidx; v_idx++)
        {
            if ( (*p_char != '+') && (*p_char != '-') )
            {
                v_temp = (INT8U) (*p_char - '0');
                v_result +=  v_temp * f_TypeConv_Exp(10, v_idx);
                p_char--;
            }
            else
            {
                // Should be the last char processed
                v_result *= v_sign;
                p_char--;
            }
        }

    }

    return v_result;
}
#endif

/*!
 * @fn void f_TypeConv_HexToString(PINT8U, PINT8U, INT8U)
 * @brief Converts a HEX (binary) string to HEX ASCII data
 *
 * @param p_data_hex : Pointer to Input data to convert
 * @param p_data_str : Pointer to ASCII output data (as HEX), size should be double of p_data_hex
 * @param v_len      : Length of data to convert
 */
void f_TypeConv_HexToString(PINT8U p_data_hex, PINT8U p_data_str, INT8U v_len)
{
    INT8U v_idx;

    /* ASCII EEPROM data */
    for(v_idx=0;v_idx<v_len;v_idx++)
    {
        // msprintf((char *) (p_data_str+2*v_idx), "%02X", (char) *(p_data_hex+v_idx) );
        sprintf((char *) (p_data_str+2*v_idx), "%02X", (char) *(p_data_hex+v_idx) );
    }
}



/*!
 * @fn INT16U f_TypeConv_SwapEndian16(INT16U)
 * @brief Convert to Little Endian from Big Endian data
 *
 * @param v_BigEndian   : Value to convert
 * @return Little Endian equivalent
 */
INT16U f_TypeConv_SwapEndian16(INT16U v_BigEndian)
{
    INT16U v_LittleEndian;
    type_INT16U u_MyValue;

    u_MyValue.word16 = v_BigEndian;

    v_LittleEndian = (u_MyValue.bytes.LSB << 8);
    v_LittleEndian |= u_MyValue.bytes.MSB;

    return v_LittleEndian;
}

/*!
 * @fn INT32U f_TypeConv_SwapEndian32(INT32U)
 * @brief Convert to Little Endian from Big Endian data
 *
 * @param v_BigEndian   : Value to convert
 * @return Little Endian equivalent
 */
INT32U f_TypeConv_SwapEndian32(INT32U v_BigEndian)
{
    INT16U v_LittleEndian;
    type_INT32U u_MyValue;

    u_MyValue.word32 = v_BigEndian;

    v_LittleEndian = (u_MyValue.bytes.Byte0 << 24);
    v_LittleEndian |= (u_MyValue.bytes.Byte1 << 16);
    v_LittleEndian |= (u_MyValue.bytes.Byte2 << 8);
    v_LittleEndian |= (u_MyValue.bytes.Byte3 << 0);

    return v_LittleEndian;
}

