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

Token_Role :: enum {
    Token_Role_Key,
    Token_Role_Value,

	Token_Role_Count,
    Token_Role_Invalid 
}

////////////////////////////////////////////////
//~ gab : whatever you see between any two comma_tokens is represented using a node struct
Node :: struct {
	scope_type: Scope_Type,
	value_type: Node_Value_Type, 

	key: []byte,
	value: []byte,

	prev_sibling_node: ^Node,
	next_sibling_node: ^Node,

	parent_node: ^Node,
	child_node: ^Node,

	depth: u16, // what is the current depth level of this node's scope ? 
	index_at_depth: u16 // what is the index of this node in the current scope ?
}

approx_equal :: proc(lhs, rhs: f64) -> (bool) {	
	epsilon: f64 = 0.0000001

	diff: f64 = lhs - rhs
	return (diff > -epsilon) && (diff < epsilon)
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

////////////////////////////////////////////////
//~ gab : visualize this in rad_debugger !
produce_internal_json_representation :: proc(pairs_file_bytes: []byte) -> ([]Node, u32) {
	internal_json_representation := new([MAX_PAIR_PROCESS_COUNT]Node)

	current_node: ^Node = &internal_json_representation[0]
	next_free_node_index: u32 = 1

	current_slice: []byte = pairs_file_bytes

	token: Token
	cursor := get_next_token(current_slice, &token)

	active_token_role: Token_Role = .Token_Role_Invalid
	if token.type == .Token_Type_Open_Brace {
		active_token_role = .Token_Role_Key
		current_node.depth = 0
		current_node.index_at_depth = 0
	}

	for token.type != .Token_Type_Invalid {
		current_slice = current_slice[cursor:]
		cursor = get_next_token(current_slice, &token)

		#partial switch token.type {
			case .Token_Type_Open_Brace : {
				next_node: ^Node = &internal_json_representation[next_free_node_index]; next_free_node_index+= 1;

				current_node.value_type = .Node_Value_Type_Json
				current_node.child_node = next_node

				next_node.scope_type = .Scope_Type_Brace
				next_node.parent_node = current_node
				next_node.prev_sibling_node = nil 

				current_node = next_node

				active_token_role = .Token_Role_Key
				current_node.depth = current_node.parent_node.depth + 1
				current_node.index_at_depth = 0
			}
			case .Token_Type_Close_Brace : {
				current_node.next_sibling_node = nil 
				current_node = current_node.parent_node
			}
			case .Token_Type_Open_Bracket : {
				next_node: ^Node = &internal_json_representation[next_free_node_index]; next_free_node_index+= 1;

				current_node.value_type = .Node_Value_Type_Array
				current_node.child_node = next_node

				next_node.scope_type = .Scope_Type_Bracket
				next_node.parent_node = current_node
				next_node.prev_sibling_node = nil

				current_node = next_node

				active_token_role = .Token_Role_Value
				current_node.depth = current_node.parent_node.depth + 1
				current_node.index_at_depth = 0
			}
			case .Token_Type_Close_Bracket : {
				current_node.next_sibling_node = nil
				current_node = current_node.parent_node
			}
			case .Token_Type_Comma : {
				next_node: ^Node = &internal_json_representation[next_free_node_index]; next_free_node_index+= 1;

				current_node.next_sibling_node = next_node 

				next_node.prev_sibling_node = current_node
				next_node.scope_type = current_node.scope_type 
				next_node.parent_node = current_node.parent_node

				current_node = next_node

				if current_node.scope_type == .Scope_Type_Brace {
					active_token_role = .Token_Role_Key
				} else if current_node.scope_type == .Scope_Type_Bracket {
					active_token_role = .Token_Role_Value
				}

				current_node.depth = current_node.prev_sibling_node.depth
				current_node.index_at_depth = current_node.prev_sibling_node.index_at_depth + 1
			}
			case .Token_Type_Colon : {
				active_token_role = .Token_Role_Value
			}
			case .Token_Type_Bool : {
				current_node.value_type = .Node_Value_Type_Bool

				current_node.value = token.value
			}
			case .Token_Type_String : {
				current_node.value_type = .Node_Value_Type_String

				if active_token_role == .Token_Role_Key {
					current_node.key = token.value
				} else if active_token_role == .Token_Role_Value {
					current_node.value = token.value
				}
			}
			case .Token_Type_Number : {
				current_node.value_type = .Node_Value_Type_Number

				if active_token_role == .Token_Role_Key {
					current_node.key = token.value
				} else if active_token_role == .Token_Role_Value {
					current_node.value = token.value
				}
			}
		}
	}

	return internal_json_representation[:], next_free_node_index
}

produce_coordinate_pairs_haversine_answers_and_average_sum :: proc(internal_json_representation: []Node) -> ([]Coordinate_Pair, u32, []f64, f64) {
	running_sum: f64

	coordinate_pairs := new([MAX_PAIR_PROCESS_COUNT]Coordinate_Pair)
	distances_between_pairs := new([MAX_PAIR_PROCESS_COUNT]f64)

	current_pair_index: u32
	current_coordinate: string

	for node, node_index in internal_json_representation {
		if len(node.key) > 0 {
			switch string(node.key) {
				case "x0" : {
					coordinate_pairs[current_pair_index].x0, _ = strconv.parse_f64(string(node.value))
				}	
				case "y0" : {
					coordinate_pairs[current_pair_index].y0, _ = strconv.parse_f64(string(node.value))
				}	
				case "x1" : {
					coordinate_pairs[current_pair_index].x1, _ = strconv.parse_f64(string(node.value))
				}	
				case "y1" : {
					coordinate_pairs[current_pair_index].y1, _ = strconv.parse_f64(string(node.value))

        			pair_distance: f64 = reference_haversine(coordinate_pairs[current_pair_index].x0, 
        				coordinate_pairs[current_pair_index].y0, 
        				coordinate_pairs[current_pair_index].x1, 
        				coordinate_pairs[current_pair_index].y1)

        			distances_between_pairs[current_pair_index] = pair_distance

					fmt.printfln("{{ x0=%v, y0=%v, x1=%v, y1=%v : d=%v }}", 
						coordinate_pairs[current_pair_index].x0,
						coordinate_pairs[current_pair_index].y0,
						coordinate_pairs[current_pair_index].x1,
						coordinate_pairs[current_pair_index].y1,
						pair_distance)

					running_sum += pair_distance

					current_pair_index += 1
				}	
			}
		}
	}

	average_sum: f64 = running_sum / cast(f64)current_pair_index
	fmt.printfln("\navg_sum=%v", average_sum)

	return coordinate_pairs[:], current_pair_index, distances_between_pairs[:], average_sum
}

tally_produced_distances_and_avg_sum_against_reference :: proc(distances_between_pairs: []f64, average_sum: f64, reference_file_path: string) {	
	ref_file_bytes, _ := os.read_entire_file(reference_file_path)	
	defer delete(ref_file_bytes)

	pass: bool = true

	ref_file_content_as_string := string(ref_file_bytes)
	ref_distances, _ := strings.split_lines(ref_file_content_as_string)
	for distance_index in 0..<len(distances_between_pairs) {
		ref_distance_f64, _ := strconv.parse_f64(ref_distances[distance_index])
		if !approx_equal(distances_between_pairs[distance_index], ref_distance_f64) {
			pass = false
		}
	}

	ref_average_sum_f64, _ := strconv.parse_f64(ref_distances[len(distances_between_pairs)])
	if !approx_equal(average_sum, ref_average_sum_f64) {
		pass = false
	}

	if pass == false {
		fmt.println("failed_distance_and_sum_checks !")
	} else {
		fmt.println("passed_distance_and_sum_checks !")	
	}
}

process_haversine_pairs_json_file :: proc(pairs_file_path: string) -> ([]Coordinate_Pair, u32, []f64, f64) {
	pairs_file_bytes, _ := os.read_entire_file(pairs_file_path)	
	defer delete(pairs_file_bytes)

	internal_json_representation, number_of_nodes_in_the_representation := produce_internal_json_representation(pairs_file_bytes)
	defer delete(internal_json_representation)	

	return produce_coordinate_pairs_haversine_answers_and_average_sum(internal_json_representation[:number_of_nodes_in_the_representation])
}