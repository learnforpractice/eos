#include <stdint.h>
#include <stdlib.h>
#include <string.h>
const char mp_frozen_str_names[] = {
"foo.mpy\0"
"db.mpy\0"
"\0"};
const uint32_t mp_frozen_str_sizes[] = {
109,
2870,
0};
const char mp_frozen_str_content[] = {
"M\x05\x02\x1f" " P\x00\x0c\x00\x07\x16" "contract.pye\x00" "2\x00\x16\x12" "say_hello2\x01\x16\n" "applyQc\x00\x02" "H\x08\x0c\x03\x05" " \x00\x12\x00" "{#\x00" "4\x01" "YQc\x01\x00" "s\x0b" "hello,worldP3\x0e\x05\x03" "` \x00\x12\x00" "{\xb0\xb1\xb2" "4\x03" "YQc\x00\x00\x02" "a\x02" "b\x02" "c\0"
"M\x05\x02\x1f" " \x84\x10\x18" "*\x00\x07\x16" "contract.py((J$$$$D$D\x8e" "2n n \x00\x80" "Q\x1b\x08" "json\x16\x01\x80" "Q\x1b\x0c" "struct\x16\x01\x80\x10\x00\x05" "*\x01\x1b\n" "chaini\x80\x16\n" "idx64\x81\x16\x0c" "idx128\x82\x16\x0c" "idx256\x83\x16\x14" "idx_double\x84\x16\x1e" "idx_long_double\x80\x16" " primary_type_i64\x81\x16\"" "primary_type_i256T2\x00\x10\x0e" "ChainDB\x11\x00" "u4\x03\x16\x01" "T2\x01\x10\x18" "ChainDBKey64\x11\x03" "4\x03\x16\x03" "T2\x02\x10\x1a" "ChainDBKey256\x11\x05" "4\x03\x16\x03" "T2\x03\x10\x14" "MultiIndex4\x02\x16\x01" "Qc\x00\x04\x81" "x\x00" " \x05\x1d\x8c\x0e\x85\x14" "ee`e`\x85\x08\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a" "2\x00\x16\x00\x11" "2\x01\x16\x00" "S2\x02\x16\x00" "V2\x03\x16\x08" "load2\x04\x16\n" "store2\x05\x16\x00\x80" "Qc\x00\x06\x84\x14\xba\x04" ",\x00\x11\x07\x80\x0f" "%%%%E(''''J''''\x00\xb2\xb0\x18\x08" "code\xb3\xb0\x18\n" "scope\xb4\xb0\x18\n" "table\xb5\xb0\x18\x12" "data_type\xb1\xb0\x18\x18" "primary_type\xb1\x12\x1b\xd9" "D&\x80\x12\x16" "db_find_i64\xb0\x18\x0e" "db_find\x12\x14" "db_get_i64\xb0\x18\x0c" "db_get\x12\x18" "db_store_i64\xb0\x18\x10" "db_store\x12\x1a" "db_update_i64\xb0\x18\x12" "db_update\x12\x1a" "db_remove_i64\xb0\x18\x12" "db_removeB#\x80\x12\x18" "db_find_i256\xb0\x18\x13\x12\x16" "db_get_i256\xb0\x18\x13\x12\x1a" "db_store_i256\xb0\x18\x13\x12\x1c" "db_update_i256\xb0\x18\x13\x12\x1c" "db_remove_i256\xb0\x18\x13" "Qc\x00\x00\x00\x89" "!))))t:\x0e\x00" "S+\x80" "#\x00\xb0\x14\x1d\xb0\x13\x0b\xb0\x13\x0b\xb0\x13\x0b\xb1" "6\x04" "c\x00\x00\x00\x89\x16" "primary_key\x81" "$*\x14\x00" "V\x0b\x80" "&&$(\x00\xb1\x80\xd7" "D\x04\x80\x12\x00" "(e\xb0\x14\x1d\xb1" "6\x01\xc2\xb0\x13\x0f\x14\x0c" "unpack\xb2" "6\x01" "c\x00\x00\x00\x89\x06" "itr\x81\x10" "*\x14" "5\x0b\x80" ",(&\"\x00\xb0\x14\x00" "S\xb1" "6\x01\xc2\xb2\x80\xd7" "D\x02\x80" "Qc\xb0\x14\x00" "V\xb2" "6\x01" "c\x00\x00\x00\x89\r\x82" "|Z\x16" "5\x05\x80" "2'4&]\x00\xb1\x14\x1e" "get_primary_key6\x00\xc2\xb0\x14\x19\xb0\x13\x19\xb0\x13\x19\xb0\x13\x19\xb2" "6\x04\xc3\xb3\x80\xd7" "D\x1d\x80\xb0\x14" "%\xb0\x13\x05\xb0\x13\x05\xb1\x13\n" "payer\xb2\xb1\x14\x08" "pack6\x00" "6\x05" "YB\x12\x80\xb0\x14" "'\xb3\xb1\x13\x05\xb1\x14\x05" "6\x00" "6\x03" "YQc\x00\x00\x00\x89\x06" "obj\x81" "PB\x14\x00\x80\x15\x80" ":4&$\x00\xb0\x14\x13\xb0\x13\x13\xb0\x13\x11\xb0\x13\x11\xb1" "6\x04\xc2\xb2\x80\xd7" "D\x04\x80\x12\x00" "(e\xb0\x14" "'\xb2" "6\x01" "YQc\x00\x00\x00\x89\x1b" "p\x00\x0e\x18" "ChainDBKey64\x0f\x8c" "@\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a" "2\x00\x16\x00\x11" "Qc\x00\x01" "t\xe1\x04\x0e\x00\x11\x03\x80" "A\x00\x12\x0e" "ChainDB\x14\x00\x11\xb0\x12" " primary_type_i64\xb1\xb2\xb3\xb4" "6\x06" "YQc\x00\x00\x00\x89\x11\x11\x11" ")p\x00\x0e\x1a" "ChainDBKey256\x0f\x8c" "D\x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a" "2\x00\x16\x00\x11" "Qc\x00\x01" "t\xe1\x04\x0e\x00\x11\x03\x80" "E\x00\x12\x0f\x14\x00\x11\xb0\x12\"" "primary_type_i256\xb1\xb2\xb3\xb4" "6\x06" "YQc\x00\x00\x00\x89\x0f\x0f\x0f\x0f\x84" "4\x00" "F\x14" "MultiIndex\x0f\x8c" "H\x85\x0c" "ee`\x85\x0c" "e`e \x85\x15\x85\r" "eee \x85\x08" "eeee e \x00\x11\x00\x17\x16\x00\x16\x10\x03\x16\x00\x1a" "2\x00\x16\x00\x11" "2\x01\x16\x00" "S2\x02\x16\x00" "V2\x03\x16" "(get_secondary_values2\x04\x16\x00\x0f" "2\x05\x16\x00\x1c" "2\x06\x16" "+2\x07\x16\x00\x80" "2\x08\x16\x00\x0b" "2\x09\x16\x18" "__contains__2\n\x16\x00\x13" "2\x0b\x16\x00\x19" "2\x0c\x16" "&get_secondary_index2\r\x16\x14" "upperbound2\x0e\x16\x14" "lowerbound2\x0f\x16\x10" "idx_find2\x10\x16\x1c" "idx_upperbound2\x11\x16\x1c" "idx_lowerboundQc\x00\x12\x83" "L\xd1\x04" " \x00\x11\x15\x80" "I%%%*%%&/1\x00\xb1\xb0\x18\x1d\xb2\xb0\x18\x1d\xb3\xb0\x18\x1d\xb4\x14" "*get_secondary_indexes6\x00\xb0\x18\x0e" "indexes\xb4\xb0\x18" "!\x80\xb0\x18" "-+\x00\xb0\x18\x14" "idx_tables\x12\x00" "k\xb0\x13\x07" "4\x01\x80" "B \x80" "W\xc5\x12\x00" "^\xb0\x13\x0b" "4\x01" "#\x05\xef" "E\x01\x80\xb5\xc3\xb0\x13\x05\x14\x00" "<\xb3" "6\x01" "Y\x81\xe5" "XZ\xd7" "C\xda\x7f" "YYQc\x01\x00\x00\x89\x0f\x0f\x07\r" "i\x14" "18446744073709551600p2\x0e\x00" "S\x11\x80" "U\x00\x12\x16" "db_find_i64\xb0\x13\x0b\xb0\x13\x0b\xb0\x13\x0b\xb1" "4\x04" "c\x00\x00\x00\x89\x11\x81" " *\x14\x00" "V\x0b\x80" "X&$'\x00\xb1\x80\xd7" "D\x04\x80\x12\x00" "(e\x12\x14" "db_get_i64\xb1" "4\x01\xc2\xb0\x13\x0f\x14\x0c" "unpack\xb2" "6\x01" "c\x00\x00\x00\x89\x06" "itr\x82" "T\x8a\x10" " +\x0b\x80" "^#\")'4&\"('\x00" "+\x00\xc2\x80\xc3\xb0\x13\x19" "_K3\x00\xc4\xb0\x13\x19\xb3" "U\xc5\x12" "&db_idx_find_primary\xb4\xb0\x13\x19\xb0\x13\x19\xb5\xb1" "4\x05" "0\x02\xc6\xc7\xb6\x80\xd7" "D\x02\x80" "Qc\xb2\x14\x00" "<\xb7" "6\x01" "Y\xb3\x81\xe5\xc3" "B\xca\x7f\xb2" "c\x00\x00\x00\x89\x17\x81\x18" "*\x14\x00\x0f\r\x80" "j(&$\x00\xb0\x14\x00" "S\xb1" "6\x01\xc2\xb2\x80\xd7" "D\x04\x80\x12\x00" "(e\xb0\x14\x00" "V\xb2" "6\x01" "c\x00\x00\x00\x89\x03\x81\x0c" "+\x10\x00\x1c\x03\x80" "p/\x00\xb1\xb2\x14\x1e" "get_primary_key6\x00\xd9" "C\x04\x80\x12\x00\x1f" "e\xb0\x14" "/\xb2" "6\x01" "YQc\x00\x00\x00\x89\x07\x06" "obj\x87\x14\xa2\x10" "0\x05\x09\x80" "t'3&9\")'9J1\")'4)&-\x00\xb1\x14\x09" "6\x00\xc2\x12" "!\xb0\x13\x0f\xb0\x13\x0f\xb0\x13" "!\xb2" "4\x04\xc3\xb3\x80\xd7" "DN\x80\x12\x18" "db_store_i64\xb0\x13\x05\xb0\x13\x05\xb1\x13\n" "payer\xb2\xb1\x14\x08" "pack6\x00" "4\x05" "Y\x80\xc4\xb0\x13\x1d" "_K(\x00\xc5\xb0\x13\x1d\xb4" "U\xc6\x12\x18" "db_idx_store\xb5\xb0\x13\r\xb6\xb1\x13\x0b\xb2\xb1\x14" "!6\x00\xb4" "U4\x06" "Y\xb4\x81\xe5\xc4" "B\xd5\x7f" "BZ\x80\x12\x1a" "db_update_i64\xb3\xb1\x13\x05\xb1\x14\x0f" "6\x00" "4\x03" "Y\x80\xc4\xb0\x13\x0f" "_K?\x00\xc5\xb0\x13\x0f\xb4" "U\xc6\x12" "#\xb5\xb0\x13\x17\xb0\x13\x11\xb6\xb2" "4\x05" "0\x02\xc3\xc7\xb1\x14\x11" "6\x00\xb4" "U\xc8\xb8\xb7\xd9" "C\r\x80\x12\x1a" "db_idx_update\xb5\xb3\xb1\x13\x11\xb8" "4\x04" "Y\xb4\x81\xe5\xc4" "B\xbe\x7f" "Qc\x00\x00\x00\x89" "#\x83" "d\x82\x10\"\x00\x80" "!\x80\x89" "3&$'\")'4*(\x00\x12\x1f\xb0\x13\x0f\xb0\x13\x0f\xb0\x13\x1d\xb1" "4\x04\xc2\xb2\x80\xd7" "D\x04\x80\x12\x00" "(e\x12\x1a" "db_remove_i64\xb2" "4\x01" "Y\x80\xc3\xb0\x13\x19" "_K5\x00\xc4\xb0\x13\x19\xb3" "U\xc5\x12\x19\xb4\xb0\x13\r\xb0\x13\r\xb5\xb1" "4\x05" "0\x02\xc2\xc6\xb2\x80\xdb" "C\x04\x80\x12\x00\x1f" "e\x12\x1a" "db_idx_remove\xb4\xb2" "4\x02" "Y\xb3\x81\xe5\xc3" "B\xc8\x7f" "Qc\x00\x00\x00\x89" ")L\"\x0e\x00\x0b\x15\x80\x96\x00\xb0\x14\x00\x80\xb1" "6\x01" "YQc\x00\x00\x00\x89\x03" "x2\x0e\x18" "__contains__\x05\x80\x99\x00\x12\x17\xb0\x13\r\xb0\x13\r\xb0\x13\x17\xb1" "4\x04\x80\xdb" "c\x00\x00\x00\x89\r\x81\x04" "!\x10\x00\x13\x0b\x80\x9c" "5\x00\x12\x14" "db_end_i64\xb0\x13\x0b\xb0\x13\x0b\xb0\x13\x0b" "4\x03\xb0\x18" "/\xb0" "c\x00\x00\x00\x89\x82\x18\x19\x18\x00\x19\x0b\x80\xa0" ")$3)$\x00\xb0\x13\x03\x7f\xd9" "D\x04\x80\x12\x00" "3e\x12\x1e" "db_previous_i64\xb0\x13\x03" "4\x01" "0\x02\xb0\x18\x01\xb0\x18\x0f\xb0\x13\x03\x80\xd7" "D\x04\x80\x12\x00" "3e\xb0\x14\x00" "V\xb0\x13\x01" "6\x01" "c\x00\x00\x00\x89" "h*\x0e" "&get_secondary_index\x09\x80\xa8\x00\x12\x1c" "SecondaryIndex\xb0\xb0\x13\x1f\xb1" "U\xb0\x13" "94\x03" "c\x00\x00\x00\x89\x06" "idxp2\x0e\x14" "upperbound\x0b\x80\xab\x00\x12\"" "db_upperbound_i64\xb0\x13\x1b\xb0\x13\x1b\xb0\x13\x1b\xb1" "4\x04" "c\x00\x00\x00\x89\x0e" "primaryp2\x0e\x14" "lowerbound\r\x80\xae\x00\x12\"" "db_lowerbound_i64\xb0\x13\r\xb0\x13\r\xb0\x13\r\xb1" "4\x04" "c\x00\x00\x00\x89\r\x81\x14" "K\x10\x10" "idx_find\r\x80\xb1" "'\x00\xb0\x13\x19\xb1" "U\xc3\x12" "*db_idx_find_secondary\xb3\xb0\x13\x0f\xb0\x13\x0f\xb0\x13\x0f\xb2" "4\x05" "c\x00\x00\x00\x89\x00\\\x1a" "secondary_key\x81\x14" "K\x10\x1c" "idx_upperbound\x0f\x80\xb5" "'\x00\xb0\x13\x0f\xb1" "U\xc3\x12\"" "db_idx_upperbound\xb3\xb0\x13\x0f\xb0\x13\x0f\xb0\x13\x0f\xb2" "4\x05" "c\x00\x00\x00\x89\x00\\\x0f\x81\x14" "K\x10\x1c" "idx_lowerbound\x0f\x80\xb9" "'\x00\xb0\x13\x0f\xb1" "U\xc3\x12\"" "db_idx_lowerbound\xb3\xb0\x13\x0f\xb0\x13\x0f\xb0\x13\x0f\xb2" "4\x05" "c\x00\x00\x00\x89\x00\\\x0f\0"
"\0"};
