package pap_common

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strings"
import "core:unicode/utf8"

////////////////////////////////
//~ gab : parser global state 
////////////////////////////////

Token_Type :: enum {
	Token_Type_Open_Brace,
	Token_Type_Close_Brace,

	Token_Type_Open_Bracket,
	Token_Type_Close_Bracket,

    Token_Type_Number,
    Token_Type_String,
    Token_Type_Bool,

    Token_Type_Comma,
    Token_Type_Colon,

	Token_Type_Count,
    Token_Type_Invalid 
}

Token :: struct {
	type: Token_Type,
	value: string
}

get_next_token :: proc(buffer: []byte, token: ^Token) {
	cursor: int
    for char_byte, char_index in buffer {
        if strings.is_space(cast(rune)char_byte) == false {
        	cursor = char_index
            break;
        }
    }
}

parse_json_curly_scope :: proc(scope_slice: []byte) {
}

parse_json_array_scope :: proc(scope_slice: []byte) {
}

process_haversine_pairs_json_file :: proc(file_path: string) {
	file_bytes, _ := os.read_entire_file(file_path)	
	token: Token
	get_next_token(file_bytes, &token)
}