#include <iostream>
#include <string>
#include <vector>

using namespace std;

int lex_file(int position, string program);
int lex_file_element(int position, string program);
int lex_file_element_ending(int position, string program);
int lex_line_ending(int position, string program);
int lex_space(int position, string program);
int lex_newline(int position, string program);
int lex_line_comment(int position, string program);
int lex_bracket_comment(int position, string program);
int lex_bracket_content(int position, string program);
int lex_command_invocation(int position, string program);
int lex_identifier(int position, string program);
int lex_arguments(int position, string program);
int lex_separated_arguments(int position, string program);
int lex_separation(int position, string program);
int lex_argument(int position, string program);
int lex_unquoted_argument(int position, string program);
int lex_quoted_argument(int position, string program);
int lex_quoted_element(int position, string program);
int lex_if_statement(int position, string program);
int lex_if_statement_body(int position, string program);
int lex_condition(int position, string program);
int lex_condition_body(int position, string program);
int lex_condition_ending(int position, string program);
bool is_number(int code);
bool is_latin(int code);
bool is_separated(char a);
int lex_separation_nullable(int position, string program);

string output_grammar;
vector<string> function_identifiers;
vector<string> arguments;
vector<string> values;

int main() {
    string program;
    for (string line; getline(cin, line);){
        program.append(line);
        program.push_back('\n');
    }
    if (program.length() > 0) {
        program.pop_back();
    }
    //cout << program << endl;
    int rez = lex_file(0, program);
    cout << "======== " << rez << " =========" << endl;
    cout << output_grammar << endl;

    cout << "ID:\n";
    for (int i = 0; i < function_identifiers.size(); i++) {
        cout << "ID[" << i << "] = " << function_identifiers[i] << endl;
    }

    cout << "ARG:\n";
    for (int i = 0; i < arguments.size(); i++) {
        cout << "ARG[" << i << "] = " << arguments[i] << endl;
    }

    cout << "VAL:\n";
    for (int i = 0; i < values.size(); i++) {
        cout << "VAL[" << i << "] = " << values[i] << endl;
    }

//    for (int i = 0; i < program.length(); i++) {
//        cout << i << ")" << program[i] << endl;
//    }

    if (rez == -1) cout << "ERROR" << endl;
    return 0;
}

int lex_file(int position, string program) {
    while (position >= 0 && position < program.length()) {
        position = lex_separation_nullable(position, program);
        //cout << "0000000000000000000000File pos = " << position << endl;
        //cout << program.substr(position) << "@@@@@@@@@@@@@@@@@@@@" << endl;
        if (position < 0) return -1;
        if (position >= program.length()) return position;
        position = lex_file_element(position, program);
        //cout << "0000000000000000000001File pos = " << position << endl;
        if (position < 0) return -1;
    }
    //--------------------
    //cout << "0000000000000000000002File pos = " << position << endl;
    return position;
}

int lex_file_element(int position, string program) {
    //cout << position << "=" << (int)program[position] << endl;
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ') {
            if (program[i] == '\n') {
                return i + 1;
            }
            position = i;
            break;
        }
    }
    //cout << position << "==" << (int)program[position] << endl;
    if ((position < program.length() - 3) && (program[position] == 'I') && (program[position + 1] == 'F') && (program[position + 3] != ')')) {
        if ((position > program.length() - 5) || (program[position + 2] != '(')) {
            cout << "If statement is wrong " << position << endl;
            //cout << "FJKDBHFDSKJGNJDSGNLSKLGKLSGKLFGKLNNLSKGNLKSFGNLKFS" << endl;
            return -1;
        } else {
            position = lex_if_statement(position + 3, program);
//            output_grammar.append("<IF>");
//            output_grammar.append("<(>");
//            position = lex_condition(position + 3, program);
            //if_statement :)
            //cout << "after_if_st_pos =" << position << "(" << program[position] << ")" << endl;
        }
    } else if (program[position] != '#') {
        //cout << "command_inv\n";
        position = lex_command_invocation(position, program);
        //cout << position << "===" << (int)program[position] << endl;
        if (position < 0) return -1;
        if (position >= program.length()) return position;
    }
    position = lex_line_ending(position, program);
    //cout << position << "====" << position << "(" << (int)program[position] << ")" << endl;
    if (position < 0) return -1;
    return position;
}

int lex_if_statement(int position, string program) {
    output_grammar.append("<IF>");
    output_grammar.append("<(>");
    //cout << "if~" << position << endl;
    position = lex_condition(position, program);
    if (position < 0 || position >= program.length()) {
        return -1;
    }

    position++;
    output_grammar.append("<)>\n");

    //cout << "if =" << position << endl;

    position = lex_line_ending(position, program);
    //cout << "if ==" << position << endl;
    if (position < 0 || position >= program.length()) {
        return -1;
    }

    position = lex_if_statement_body(position, program);
    //cout << "if ===" << position << endl;
    if (position < 0) return -1;

//    position = lex_line_ending(position, program);
//    cout << "if ====" << position << " (len = " << program.length() << ")" << endl;
//    if (position < 0) {
//        return -1;
//    }

    return position;
}

int lex_if_statement_body(int position, string program) {
    position = lex_separation(position, program);
    //cout << "body =" << position << " (" << program[position] << ")" << endl;
    if (position < 0 || position >= program.length()) {
        return -1;
    }

    bool has_else = false;
    int if_statement_start = position;
    int if_statement_end = program.length() - 1;
    bool is_closed = false;
    int if_depth = 1;
    for (int i = position; i < program.length(); i++) {
        if ((i < program.length() - 6) && (program[i] == 'E') && (program[i + 1] == 'N') && (program[i + 2] == 'D')
            && (program[i + 3] == 'I') && (program[i + 4] == 'F') && (program[i + 5] == '(') && (program[i + 6] == ')')) {
            if_depth--;
            //cout << "AHAHAHAHHAHAH------------" << endl;
        } else if ((i < program.length() - 3) && (program[i] == 'I') && (program[i + 1] == 'F') && (program[i + 2] == '(') && (program[i + 3] != ')')) {
            if ((i > 4) && (program[i - 4] == 'E') && (program[i - 3] == 'L') && (program[i - 2] == 'S') && (program[i - 1] == 'E')) {

            } else {
                //cout << "AHAHAHAHHAHAH++++++++++++" << endl;
                if_depth++;
            }
        }

        if (if_depth == 0) {
            is_closed = true;
            if_statement_end = i;
            break;
        }
    }

    if (!is_closed) {
        cout << "IF STATEMENT IS NOT CLOSED\n";
        return -1;
    }

    string if_statement_str = program.substr(if_statement_start, if_statement_end - if_statement_start);
    cout << "IF_ST_STR =" << if_statement_str << endl << "----------" << endl;
    //cout << "body start =" << if_statement_start << " (" << program[if_statement_start] << ")" << "; end = " << if_statement_end << " (" << program[if_statement_end] << ")" << endl;
    int block_start = 0;
    if_depth = 1;
    bool first_struct = true;
    for (int i = 0; i < if_statement_str.length(); i++) {
        //cout << if_statement_str[i] << endl;
        if ((i < if_statement_str.length() - 6) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'N') && (if_statement_str[i + 2] == 'D')
            && (if_statement_str[i + 3] == 'I') && (if_statement_str[i + 4] == 'F') && (if_statement_str[i + 5] == '(') && (if_statement_str[i + 6] == ')')) {
            if_depth--;
            cout << "YES " << i << endl;
        } else if ((i < if_statement_str.length() - 2) && (if_statement_str[i] == 'I') && (if_statement_str[i + 1] == 'F') && (if_statement_str[i + 2] == '(')) {
            if ((i > 0) && ((if_statement_str[i - 1] == 'E') || (if_statement_str[i - 1] == 'D'))) {

            } else {
                cout << "LEEEETS GOOO " << if_statement_str[i] << if_statement_str[i + 1] << if_statement_str[i + 2] << if_statement_str[i + 3] << endl;
                if_depth++;
            }
        } else if ((i < if_statement_str.length() - 7) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'L') && (if_statement_str[i + 2] == 'S')
        && (if_statement_str[i + 3] == 'E') && (if_statement_str[i + 4] == 'I') && (if_statement_str[i + 5] == 'F') && (if_statement_str[i + 6] == '(')
        && (if_depth == 1)) {
            if (has_else) {
                cout << "Error with else statement " << position << endl;
                return -1;
            }
            //cout << "ELSEIF=" << i + 7 << endl;
            position = lex_condition(i + 7, if_statement_str);
            if (position < 0 || position >= if_statement_str.length()) return -1;
            position++;
            int last_newline = output_grammar.find_last_of('\n');
            string condition_save;
            for (int jj = last_newline + 1; jj < output_grammar.length(); jj++) {
                condition_save.push_back(output_grammar[jj]);
            }
            output_grammar = output_grammar.substr(0, last_newline + 1);
            //cout << "ELSEIF==" << position << endl;
            //cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  ELSEIF BLOCK\n";
            string block = if_statement_str.substr(block_start, i - block_start);
            //cout << block << endl;
            //cout << "+++++++++++++++++++++++++++++++++++++++\n";
            int err = lex_file(0, block);
            if (err < 0) return -1;
            block_start = position;
            output_grammar.append("<ELSEIF>");
            output_grammar.append("<(>");
            output_grammar.append(condition_save);
            output_grammar.append("<)>\n");
            //cout << "ELSEIF===" << position << endl;
            if (first_struct) first_struct = false;
        } else if ((i < if_statement_str.length() - 6) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'L') && (if_statement_str[i + 2] == 'S')
                    && (if_statement_str[i + 3] == 'E') && (if_statement_str[i + 4] == '(') && (if_statement_str[i + 5] == ')') && (if_depth == 1)) {
            if (has_else) {
                cout << "Error with else statement " << position << endl;
                //print error
                return -1;
            } else {
                has_else = true;
                //cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA  ELSE BLOCK\n";
                string block = if_statement_str.substr(block_start, i - block_start);
                //cout << block << endl;
                //cout << "+++++++++++++++++++++++++++++++++++++++\n";
                int err = lex_file(0, block);
                if (err < 0) return -1;
                block_start = i + 6;
                output_grammar.append("<ELSE>");
                output_grammar.append("<(>");
                output_grammar.append("<)>\n");
            }
        }
    }

    string block = if_statement_str.substr(block_start, if_statement_str.length() - block_start);
    position = lex_file(0, block);

    //cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA   last block\n" << block << endl << "+++++++++++++++++++++++++++++++++++++++\n";
    //cout << "body ==" << position << endl;
    if (position < 0) return -1;
    output_grammar.append("<ENDIF>");
    output_grammar.append("<(>");
    output_grammar.append("<)>\n");
    return if_statement_end + 7;
}

int lex_condition(int position, string program) {
//    cout << "-------------------------0" << endl;
//    cout << program << endl << "-------------------------1" << endl;
    //cout << "-----------" << position << endl;
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ' && program[position] != '\n') {
            position = i;
            break;
        }
    }

    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }

    if ((position < program.length() - 3) && (program[position] == 'N') && (program[position + 1] == 'O') && (program[position + 2] == 'T')
    && (is_separated(program[position + 3]))) {
        position = lex_separation(position + 3, program);
        output_grammar.append("<NOT>");
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }

        if (program[position] == '(') {
            output_grammar.append("<(>");
            position = lex_condition(position + 1, program);
            if (position < 0 || position >= program.length()) {
                cout << "Condition is not closed " << position << endl;
                return -1;
            }

            position++;
            //output_grammar.pop_back();
            output_grammar.append("<)>");
        } else {
            position = lex_condition(position, program);
        }
    } else {
        if (program[position] == '(') {
            output_grammar.append("<(>");

            position = lex_condition(position + 1, program);
            if (position < 0 || position >= program.length()) {
                cout << "Condition is not closed " << position << endl;
                return -1;
            }
            position++;
            output_grammar.append("<)>");
        } else {
            position = lex_condition_body(position, program);
        }
    }

    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }
    //cout << "-----------" << position << endl;

    position = lex_separation(position, program);
    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }

//    cout << "-------------------------2" << endl;
//    cout << program << endl << "-------------------------3" << endl;
    //cout << "-----------" << position << endl;
    if (program[position] != ')') {
        //cout << "ENTERING ENDING TERRITORY:" << position << "(" << program[position] << ")\n";
//        cout << "!!!!!!!!!!!!!!!!" << endl << program << "!!!!!!!!!!!!!!!!!!!!" << endl;
        return lex_condition_ending(position, program);
    } else {
        //output_grammar.append("<)>\n");
        return position;
    }
}

int lex_condition_body(int position, string program) {
    position = lex_argument(position, program);

    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }

    position = lex_separation(position, program);

    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }

    if (program[position] == ')') {
        //output_grammar.append("<)>\n");
        return position;
    }
    else if ((position < program.length() - 4) && (program[position] == 'L') && (program[position + 1] == 'E')
                && (program[position + 2] == 'S') && (program[position + 3] == 'S') && (is_separated(program[position + 4]))) {
        output_grammar.append("<LESS>");
        position = lex_separation(position + 4, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }

        position = lex_argument(position, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }
        return position;
    } else if ((position < program.length() - 7) && (program[position] == 'G') && (program[position + 1] == 'R')
               && (program[position + 2] == 'E') && (program[position + 3] == 'A') && (program[position + 4] == 'T')
               && (program[position + 5] == 'E') && (program[position + 6] == 'R') && (is_separated(program[position + 7]))) {
        output_grammar.append("<GREATER>");
        position = lex_separation(position + 7, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }

        position = lex_argument(position, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }
        return position;
    } else if ((position < program.length() - 5) && (program[position] == 'E') && (program[position + 1] == 'Q')
               && (program[position + 2] == 'U') && (program[position + 3] == 'A') && (program[position + 4] == 'L')
               && (is_separated(program[position + 5]))) {
        output_grammar.append("<EQUAL>");
        position = lex_separation(position + 5, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }

        position = lex_argument(position, program);
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }
        return position;
    }
    return position;
}

int lex_condition_ending(int position, string program) {
    //cout << "ending position=" << position << "(" << program[position] << ")\n";
    if ((position < program.length() - 2) && (program[position] == 'O') && (program[position + 1] == 'R') && (is_separated(program[position + 2]))) {
        position = lex_separation(position + 2, program);
        output_grammar.append("<OR>");
        //cout << "OR LOCATED!\n";
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }
        position = lex_condition(position, program);
    } else if ((position < program.length() - 3) && (program[position] == 'A') && (program[position + 1] == 'N')
                && (program[position + 2] == 'D') && (is_separated(program[position + 3]))) {
        position = lex_separation(position + 3, program);
        output_grammar.append("<AND>");
        if (position < 0 || position >= program.length()) {
            cout << "Condition is not closed " << position << endl;
            return -1;
        }
        position = lex_condition(position, program);
    } else {
        return -1;
    }

    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }
    return position;
}

int lex_line_ending(int position, string program) {
    while(position < program.length()) {
        //cout << "line ending " << position << "=" << (int)program[position] << endl; //<< "=" << program[position - 1] << (int)program[position] << program[position + 1] << endl;
        //cout << "!!!!!!!!!" << program << "!!!!!!!!!\n";
        if (program[position] == '\n') {
            position++;
            return position;
        }

        for (int i = position; i < program.length(); i++) {
            if (program[i] != '\t' && program[i] != ' ') {
                position = i;
                break;
            } else if (i == program.length() - 1) {
                return i + 1;
            }
        }

        if (program[position] == '#') {
            if ((position < program.length() - 2) && (program[position + 1] == '[') && (program[position + 2] == '[')) {
                position = lex_bracket_comment(position + 3, program);
//                for (int i = position; i < program.length(); i++) {
//                    if ((program[i] == ']') && (i < program.length() - 1) && (program[i + 1] == ']')) {
//                        position = i + 2;
//                        break;
//                    }
//                }
            } else {
                return position = lex_line_comment(position + 1, program);
            }
        } else {
            cout << "Bad symbols in line ending" << position << endl;
            return -1;
        }
    }
    return position;
}

int lex_line_comment(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] == '\n') {
            position = i + 1;
            break;
        }
    }
    return position;
}

int lex_bracket_comment(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if ((program[i] == ']') && (i < program.length() - 1) && (program[i + 1] == ']')) {
            position = i + 2;
            break;
        }
    }
    return position;
}

int lex_command_invocation(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ') {
            position = i;
            break;
        }
    }
    position = lex_identifier(position, program);
    if (position < 0) return -1;

    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ') {
            if (program[i] != '(') {
                return -1;
            }
            output_grammar.append("<(>");
            position = i + 1;
            break;
        }
    }
    position = lex_arguments(position, program);
    output_grammar.append("<)>\n");
    if (position < 0) { //|| position >= program.length()) {
        return -1;
    }

    position++;
    return position;
    //--------------------------
}

int lex_identifier(int position, string program) {
    string identifier;
    string output;
    for (int i = position; i < program.length(); i++) {
        int code = (int)program[i];
        if (!is_latin(code) && !is_number(code) && (code != 95)) {
            position = i;
            break;
        }
        identifier.push_back(program[i]);
    }

    if (position < 0 || position >= program.length()) {
        cout << "Identifier is poorly defined " << position << endl;
        return -1;
    }

    bool already_exists = false;
    int ident_pos = function_identifiers.size();
    for (int j = 0; j < function_identifiers.size(); j++) {
        if (function_identifiers[j] == identifier) {
            ident_pos = j;
            already_exists = true;
        }
    }
    if (!already_exists) {
        function_identifiers.push_back(identifier);
    }
    output.append("<Id,");

    int tmp = ident_pos;
    string number;
    do {
        int dig = tmp % 10;
        number.push_back(dig + 48);
        tmp /= 10;
    } while (tmp > 0);
    for (int jj = 0; jj < number.length() / 2; jj++) {
        char kk = number[jj];
        number[jj] = number[number.length() - 1 - jj];
        number[number.length() - 1 - jj] = kk;
    }
    output.append(number);
    output.append(">");
    //cout << output;
    output_grammar.append(output);
    return position;
}

int lex_arguments(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ' && program[i] != '\n') {
            position = i;
            break;
        }
    }

    if (position < 0 || position >= program.length()) {
        return -1;
    }

    position = lex_argument(position, program);

    if (position < 0 || position >= program.length()) {
        return -1;
    }

    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ') {
            position = i;
            break;
        }
    }

    if (position < 0 || position >= program.length()) {
        cout << "Command is not closed " << position << endl;
        return -1;
    }

    if (program[position] != ')') {
        position = lex_separated_arguments(position, program);
    }

    return position;
}

int lex_separated_arguments(int position, string program) {
    position = lex_separation(position, program);

    if (program[position] == ')') {
        //output_grammar.append("<)>");
        return position;
    }

    position = lex_arguments(position, program);

    if (position < 0 || position >= program.length()) {
        return -1;
    }
    return position;
}

int lex_separation(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ' && program[i] != '\n') {
            position = i;
            break;
        }
    }

    if (position < 0 || position >= program.length()) return -1;

    if (program[position] != '#') {
        return position;
    } else {
        if ((position < program.length() - 2) && (program[position + 1] == '[') && (program[position + 2] == '[')) {
            position += 3;
            //cout << position << " abab\n";
            position = lex_bracket_comment(position, program);
            //cout << position << " abab\n";
        } else {
            position++;
            position = lex_line_comment(position, program);
        }
    }

    if (position < 0 || position >= program.length()) {
        cout << "Error with comments " << position << endl;
        return -1;
    }

    return lex_separation(position, program);
}

int lex_separation_nullable(int position, string program) {
    //cout << "nullable pos=" << position << endl;
    bool is_null = true;
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ' && program[i] != '\n') {
            position = i;
            is_null = false;
            break;
        }
    }

    if (is_null) {
        //cout << "some null stuf ^)\n";
        return program.length();
    }
    if (position < 0 || position >= program.length()) return -1;

    if (program[position] != '#') {
        return position;
    } else {
        if ((position < program.length() - 2) && (program[position + 1] == '[') && (program[position + 2] == '[')) {
            position += 3;
            //cout << position << " abab\n";
            position = lex_bracket_comment(position, program);
            //cout << position << " abab\n";
        } else {
            position++;
            position = lex_line_comment(position, program);
        }
    }

    if (position < 0 || position >= program.length()) {
        return -1;
    }

    return lex_separation(position, program);
}

int lex_argument(int position, string program) {
    if (program[position] == '\"') {
        position++;
        position = lex_quoted_argument(position, program);
    } else {
        position = lex_unquoted_argument(position, program);
    }

    if (position < 0 || position >= program.length()) {
        return -1;
    }

    return position;
}

int lex_unquoted_argument(int position, string program) {
    string arg;
    string output;

    for (int i = position; i < program.length(); i++) {
        if ((program[i] == '#') || (program[i] == '(') || (program[i] == ')')
        || (program[i] == '\\') || (program[i] == ' ') || (program[i] == '\t') || (program[i] == '\n')) {
            position = i;
            bool is_value = true;
            if (!is_number(arg[0])) is_value = false;
            else {
                for (char j : arg) {
                    if (!is_number(j) && j != '.' && j != ',') {
                        is_value = false;
                        break;
                    }
                }
            }
            if (is_value) {
                bool already_exists = false;
                int arg_pos = values.size();
                for (int j = 0; j < values.size(); j++) {
                    if (values[j] == arg) {
                        arg_pos = j;
                        already_exists = true;
                    }
                }
                if (!already_exists) {
                    values.push_back(arg);
                }
                output.append("<Val,");

                int tmp = arg_pos;
                string number;
                do {
                    int dig = tmp % 10;
                    number.push_back(dig + 48);
                    tmp /= 10;
                } while (tmp > 0);
                for (int jj = 0; jj < number.length() / 2; jj++) {
                    char kk = number[jj];
                    number[jj] = number[number.length() - 1 - jj];
                    number[number.length() - 1 - jj] = kk;
                }
                output.append(number);
                output.append(">");
                output_grammar.append(output);
            } else {
                bool already_exists = false;
                int arg_pos = arguments.size();
                for (int j = 0; j < arguments.size(); j++) {
                    if (arguments[j] == arg) {
                        arg_pos = j;
                        already_exists = true;
                    }
                }
                if (!already_exists) {
                    arguments.push_back(arg);
                }
                output.append("<Arg,");

                int tmp = arg_pos;
                string number;
                do {
                    int dig = tmp % 10;
                    number.push_back(dig + 48);
                    tmp /= 10;
                } while (tmp > 0);
                for (int jj = 0; jj < number.length() / 2; jj++) {
                    char kk = number[jj];
                    number[jj] = number[number.length() - 1 - jj];
                    number[number.length() - 1 - jj] = kk;
                }
                output.append(number);
                output.append(">");
                output_grammar.append(output);
            }
            if (program[position] == '\n') return position + 1;
            return position;
        } else {
            arg.push_back(program[i]);
        }
    }
    cout << "Argument is poorly defined " << position << endl;
    return -1;
}

int lex_quoted_argument(int position, string program) {
    string arg = "\"";
    string output;
    for (int i = position; i < program.length(); i++) {
        if (program[i] == '\"') {
            position = i + 1;
            arg.push_back('\"');
            bool already_exists = false;
            int arg_pos = values.size();
            for (int j = 0; j < values.size(); j++) {
                if (values[j] == arg) {
                    arg_pos = j;
                    already_exists = true;
                }
            }
            if (!already_exists) {
                values.push_back(arg);
            }
            output.append("<Val,");

            int tmp = arg_pos;
            string number;
            do {
                int dig = tmp % 10;
                number.push_back(dig + 48);
                tmp /= 10;
            } while (tmp > 0);
            for (int jj = 0; jj < number.length() / 2; jj++) {
                char kk = number[jj];
                number[jj] = number[number.length() - 1 - jj];
                number[number.length() - 1 - jj] = kk;
            }
            output.append(number);
            output.append(">");
            output_grammar.append(output);;
            return position;
        } else {
            arg.push_back(program[i]);
        }
    }
    cout << "Commas are not closed " << position << endl;
    return -1;
}

bool is_latin(int code){
    return ((code > 64) && (code < 91)) || ((code > 96) && (code < 123));
}

bool is_number(int code){
   return (code > 47) && (code < 58);
}

bool is_separated(char a) {
    return (a == '\t') || (a == ' ') || (a == '\n') || (a == '(') || (a == '#');
}

