package pap_common

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strings"
import "core:unicode"

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
	value: []byte
}

get_next_token :: proc(buffer: []byte, token: ^Token) -> (int) {
	cursor: int
    for char_byte, char_index in buffer {
        if strings.is_space(cast(rune)char_byte) == false {
        	cursor = char_index
            break;
        }
    }

    if cursor >= len(buffer) {
    	token.type = .Token_Type_Invalid
    	return -1 
    }

    switch cast(rune)buffer[cursor] {
    	case '{' : {
    		token.type = .Token_Type_Open_Brace
    		cursor += 1
    	}
		case '}' : {
    		token.type = .Token_Type_Close_Brace
    		cursor += 1
    	}
		case '[' : {
    		token.type = .Token_Type_Open_Bracket
    		cursor += 1
    	}
		case ']' : {
    		token.type = .Token_Type_Close_Bracket
    		cursor += 1
    	}
		case ',' : {
    		token.type = .Token_Type_Comma
    		cursor += 1
    	}
		case ':' : {
    		token.type = .Token_Type_Colon
    		cursor += 1
    	}
		case 't' : fallthrough
		case 'f' : {
			token.type = .Token_Type_Bool
			token.value = buffer[cursor : cursor+1]
			cursor += 1
        }
    	case '"' : {
			string_start_index: int = cursor;
			cursor += 1

    		for char_byte in buffer[cursor:] {
        		if cast(rune)char_byte == '"' {
					cursor += 1
        			break
        		}

    			cursor += 1
			}

    		token.type = .Token_Type_String
			token.value = buffer[string_start_index : cursor]
    	}
		case '-' : fallthrough
		case '0'..='9' : {
			number_start_index: int = cursor;
    		for char_byte in buffer[cursor:] {
				character := cast(rune)char_byte
        		if unicode.is_digit(character) == false && character != '.' && character != '-' {
        			break
        		}

				cursor += 1	
			}

			token.type = .Token_Type_Number
			token.value = buffer[number_start_index : cursor]
        }
    }

    return cursor
}

parse_json_curly_scope :: proc(scope_slice: []byte) {
}

parse_json_array_scope :: proc(scope_slice: []byte) {
}

process_haversine_pairs_json_file :: proc(file_path: string) {
	file_bytes, _ := os.read_entire_file(file_path)	
	defer delete(file_bytes)

	cursor: int
	current_slice: []byte = file_bytes

	token: Token
	cursor = get_next_token(current_slice, &token)
	if token.type == .Token_Type_Number || token.type == .Token_Type_String {
		fmt.println(transmute(string)token.value)
	} else {
		fmt.println(token.type)
	}

	for token.type != .Token_Type_Invalid {
		current_slice = current_slice[cursor:]
		cursor = get_next_token(current_slice, &token)

		if token.type == .Token_Type_Number || token.type == .Token_Type_String {
			fmt.println(transmute(string)token.value)
		} else {
			fmt.println(token.type)
		}
	}
}