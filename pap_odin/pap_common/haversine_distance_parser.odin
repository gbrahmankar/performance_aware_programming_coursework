package pap_common

import "core:fmt"
import "core:os"
import "core:slice"
import "core:strconv"
import "core:strings"
import "core:unicode"

MAX_PAIR_PROCESS_COUNT :: 1 << 27

Coordinate_Pair :: struct {
	x0: f64,
	y0: f64,
	x1: f64,
	y1: f64	
}

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

Scope_Type :: enum {
	Scope_Type_Brace,
	Scope_Type_Bracket,

	Scope_Type_Count,
    Scope_Type_Invalid 
}

Node_Value_Type :: enum {
	Node_Value_Type_Number,
	Node_Value_Type_String,
	Node_Value_Type_Bool,

	Node_Value_Type_Array,
	Node_Value_Type_Json,

	Node_Value_Type_Count,
    Node_Value_Type_Invalid 
}

////////////////////////////////////////////////
// gab : whatever you see between any two comma_tokens is represented using a node struct
Node :: struct {
	scope_type: Scope_Type,
	value_type: Node_Value_Type, 

	key: []byte,
	value: []byte,

	prev_sibling_node: ^Node,
	next_sibling_node: ^Node,

	parent_node: ^Node, // only first_sibling_in_scope_node will have a parent_node
	child_node: ^Node,

	first_sibling_in_scope_node: ^Node 
}

get_next_token :: proc(buffer: []byte, token: ^Token) -> (int) {
	token.type = .Token_Type_Invalid

	cursor: int
    for char_byte, char_index in buffer {
        if strings.is_space(cast(rune)char_byte) == false {
        	cursor = char_index
            break;
        }
    }

    if cursor >= len(buffer) {
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
			cursor += 1

			string_start_index: int = cursor;
    		for char_byte in buffer[cursor:] {
        		if cast(rune)char_byte == '"' {
					cursor += 1
        			break
        		}

    			cursor += 1
			}

    		token.type = .Token_Type_String
			token.value = buffer[string_start_index : cursor-1]
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

process_haversine_pairs_json_file :: proc(file_path: string) {
	file_bytes, _ := os.read_entire_file(file_path)	
	defer delete(file_bytes)

	cursor: int
	current_slice: []byte = file_bytes

	////////////////////////////////////////////////
	// gab : for the pure joy of json_parsing 
	cursor = 0
	current_slice = file_bytes

	json_full_representation := new([MAX_PAIR_PROCESS_COUNT]Node)
	defer free(json_full_representation)	
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// gab : for solving the haversine_distance_problem 
	cursor = 0
	current_slice = file_bytes

	all_pairs := new([MAX_PAIR_PROCESS_COUNT]Coordinate_Pair)
	defer free(all_pairs)

	current_pair_index: int
	current_coordinate: string

	token: Token
	cursor = get_next_token(current_slice, &token)
	for token.type != .Token_Type_Invalid {
		current_slice = current_slice[cursor:]
		cursor = get_next_token(current_slice, &token)

		if token.type == .Token_Type_String {
			switch transmute(string)token.value {
				case "x0" : {
					current_coordinate = "x0" 
				}	
				case "y0" : {
					current_coordinate = "y0" 
				}	
				case "x1" : {
					current_coordinate = "x1" 
				}	
				case "y1" : {
					current_coordinate = "y1" 
				}	
			}
		}

		if token.type == .Token_Type_Number {
			// fmt.println("current_pair_index =", current_pair_index)
			switch current_coordinate {
				case "x0" : {
					all_pairs[current_pair_index].x0, _ = strconv.parse_f64(string(token.value))
					// fmt.println("x0 =", all_pairs[current_pair_index].x0)
				}	
				case "y0" : {
					all_pairs[current_pair_index].y0, _ = strconv.parse_f64(transmute(string)token.value)
					// fmt.println("y0 =", all_pairs[current_pair_index].y0)
				}	
				case "x1" : {
					all_pairs[current_pair_index].x1, _ = strconv.parse_f64(transmute(string)token.value)
					// fmt.println("x1 =", all_pairs[current_pair_index].x1)
				}	
				case "y1" : {
					all_pairs[current_pair_index].y1, _ = strconv.parse_f64(transmute(string)token.value)
					// fmt.println("y1 =", all_pairs[current_pair_index].y1)
					current_pair_index += 1
				}	
			}
		}

		/*
		if token.type == .Token_Type_Number || token.type == .Token_Type_String {
			fmt.println(transmute(string)token.value)
		} else {
			fmt.println(token.type)
		}
		*/
	}
	////////////////////////////////////////////////	
}