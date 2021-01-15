#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Node {
public:
    vector<int> children;
    string name;
    int real_position;
    Node(string str, int pos) {
        vector<int> vector;
        children = vector;
        name = str;
        real_position = pos;
    };
};

int lex_file(int position, string program);
int lex_file_element(int position, string program);
int lex_line_ending(int position, string program);
int lex_line_comment(int position, string program);
int lex_bracket_comment(int position, string program);
int lex_command_invocation(int position, string program);
int lex_identifier(int position, string program);
int lex_arguments(int position, string program);
int lex_separated_arguments(int position, string program);
int lex_separation(int position, string program);
int lex_argument(int position, string program);
int lex_unquoted_argument(int position, string program);
int lex_quoted_argument(int position, string program);
int lex_if_statement(int position, string program);
int lex_if_statement_body(int position, string program);
int lex_condition(int position, string program);
int lex_condition_body(int position, string program);
int lex_condition_ending(int position, string program);
bool is_number(int code);
bool is_latin(int code);
bool is_separated(char a);
int lex_separation_nullable(int position, string program);
int parse(string grammar, int position, vector<Node>& tree, vector<int> state, int current_node);
string parse_new_element(string expr);
int parse_new_element_number(string expr);
int find_next_element(string grammar, int position);
int correct_next_position(string grammar, int next_position);
bool try_combine(const vector<int>& state);
int print_tree(vector<Node> tree);

string output_grammar;
vector<string> function_identifiers;
vector<string> arguments;
vector<string> values;

int nodes_size = 0;

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

    if (rez != -1 ) {
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
    } else {
        cout << "ERROR" << endl;
        return 0;
    }

    vector<Node> nodes;
    vector<int> states;
    states.push_back(0);
    int tst = parse(output_grammar, 0, nodes, states, 0);
    return 0;
}

int lex_file(int position, string program) {
    while (position >= 0 && position < program.length()) {
        position = lex_separation_nullable(position, program);
        if (position < 0) return -1;
        if (position >= program.length()) return position;
        position = lex_file_element(position, program);
        if (position < 0) return -1;
    }
    return position;
}

int lex_file_element(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ') {
            if (program[i] == '\n') {
                return i + 1;
            }
            position = i;
            break;
        }
    }

    if ((position < program.length() - 3) && (program[position] == 'I') && (program[position + 1] == 'F') && (program[position + 3] != ')')) {
        if ((position > program.length() - 5) || (program[position + 2] != '(')) {
            cout << "If statement is wrong " << position << endl;
            return -1;
        } else {
            position = lex_if_statement(position + 3, program);
        }
    } else if (program[position] != '#') {
        position = lex_command_invocation(position, program);
        if (position < 0) return -1;
        if (position >= program.length()) return position;
    }
    position = lex_line_ending(position, program);
    if (position < 0) return -1;
    return position;
}

int lex_if_statement(int position, string program) {
    output_grammar.append("<IF>");
    output_grammar.append("<(>");
    position = lex_condition(position, program);
    if (position < 0 || position >= program.length()) {
        return -1;
    }

    position++;
    output_grammar.append("<)>\n");

    position = lex_line_ending(position, program);
    if (position < 0 || position >= program.length()) {
        return -1;
    }

    position = lex_if_statement_body(position, program);
    if (position < 0) return -1;

    return position;
}

int lex_if_statement_body(int position, string program) {
    position = lex_separation(position, program);
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
        } else if ((i < program.length() - 3) && (program[i] == 'I') && (program[i + 1] == 'F') && (program[i + 2] == '(') && (program[i + 3] != ')')) {
            if ((i > 4) && (program[i - 4] == 'E') && (program[i - 3] == 'L') && (program[i - 2] == 'S') && (program[i - 1] == 'E')) {

            } else {
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
    int block_start = 0;
    if_depth = 1;
    bool first_struct = true;
    for (int i = 0; i < if_statement_str.length(); i++) {
        if ((i < if_statement_str.length() - 6) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'N') && (if_statement_str[i + 2] == 'D')
            && (if_statement_str[i + 3] == 'I') && (if_statement_str[i + 4] == 'F') && (if_statement_str[i + 5] == '(') && (if_statement_str[i + 6] == ')')) {
            if_depth--;
        } else if ((i < if_statement_str.length() - 2) && (if_statement_str[i] == 'I') && (if_statement_str[i + 1] == 'F') && (if_statement_str[i + 2] == '(')) {
            if ((i > 0) && ((if_statement_str[i - 1] == 'E') || (if_statement_str[i - 1] == 'D'))) {

            } else {
                if_depth++;
            }
        } else if ((i < if_statement_str.length() - 7) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'L') && (if_statement_str[i + 2] == 'S')
                   && (if_statement_str[i + 3] == 'E') && (if_statement_str[i + 4] == 'I') && (if_statement_str[i + 5] == 'F') && (if_statement_str[i + 6] == '(')
                   && (if_depth == 1)) {
            if (has_else) {
                cout << "Error with else statement " << position << endl;
                return -1;
            }
            position = lex_condition(i + 7, if_statement_str);
            if (position < 0 || position >= if_statement_str.length()) return -1;
            position++;
            int last_newline = output_grammar.find_last_of('\n');
            string condition_save;
            for (int jj = last_newline + 1; jj < output_grammar.length(); jj++) {
                condition_save.push_back(output_grammar[jj]);
            }

            output_grammar = output_grammar.substr(0, last_newline + 1);
            string block = if_statement_str.substr(block_start, i - block_start);
            int err = lex_file(0, block);
            if (err < 0) return -1;
            block_start = position;
            output_grammar.append("<ELSEIF>");
            output_grammar.append("<(>");
            output_grammar.append(condition_save);
            output_grammar.append("<)>\n");
            if (first_struct) first_struct = false;
        } else if ((i < if_statement_str.length() - 6) && (if_statement_str[i] == 'E') && (if_statement_str[i + 1] == 'L') && (if_statement_str[i + 2] == 'S')
                   && (if_statement_str[i + 3] == 'E') && (if_statement_str[i + 4] == '(') && (if_statement_str[i + 5] == ')') && (if_depth == 1)) {
            if (has_else) {
                cout << "Error with else statement " << position << endl;
                return -1;
            } else {
                has_else = true;
                string block = if_statement_str.substr(block_start, i - block_start);
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

    if (position < 0) return -1;
    output_grammar.append("<ENDIF>");
    output_grammar.append("<(>");
    output_grammar.append("<)>\n");
    return if_statement_end + 7;
}

int lex_condition(int position, string program) {
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

    position = lex_separation(position, program);
    if (position < 0 || position >= program.length()) {
        cout << "Condition is not closed " << position << endl;
        return -1;
    }

    if (program[position] != ')') {
        return lex_condition_ending(position, program);
    } else {
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
    if ((position < program.length() - 2) && (program[position] == 'O') && (program[position + 1] == 'R') && (is_separated(program[position + 2]))) {
        position = lex_separation(position + 2, program);
        output_grammar.append("<OR>");
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
    if (position < 0) {
        return -1;
    }

    position++;
    return position;
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
            position = lex_bracket_comment(position, program);
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
    bool is_null = true;
    for (int i = position; i < program.length(); i++) {
        if (program[i] != '\t' && program[i] != ' ' && program[i] != '\n') {
            position = i;
            is_null = false;
            break;
        }
    }

    if (is_null) {
        return program.length();
    }
    if (position < 0 || position >= program.length()) return -1;

    if (program[position] != '#') {
        return position;
    } else {
        if ((position < program.length() - 2) && (program[position + 1] == '[') && (program[position + 2] == '[')) {
            position += 3;
            position = lex_bracket_comment(position, program);
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

int parse(string grammar, int position, vector<Node>& tree, vector<int> state, int current_node) {
    cout << "PARSEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n";
    int result = -1;
    if (position >= grammar.length() - 1) {
        for (int jj : state) {
            cout << jj << " ";
        }
        cout << endl;
    }
    if ((position >= grammar.length() - 1) && (state.size() == 1)) {
        print_tree(tree);
        return 0;
    }
    //try combine
    //do deeper
    //if good continue to the end
    //if bad rollback
    vector<int> new_states = state; //change nodes too
    bool is_combined = try_combine(new_states);
    if (is_combined) {
        //do smthg with tree
        nodes_size--; //????
        current_node = tree[nodes_size - 1].real_position; //????
        result = parse(grammar, position, tree, new_states, current_node);
        if (result == 0) return 0;
    }

    cout << position << "from possible" << grammar.length() << endl;
    int next_position = find_next_element(grammar, position);
    string current_lexxem = grammar.substr(position, next_position - position);
    cout << "Lexem=" << current_lexxem << endl;
    string name = parse_new_element(current_lexxem);
    cout << "Name=" << name << endl;
    next_position = correct_next_position(grammar, next_position);
    switch(name[0]) {
        case 'I':
            if (name[1] == 'F') {
                //cout << "WE IF NOW!!\n";
                //IF
                //state 0 || state 2

                if (state[state.size() - 1] == 0) {
                    state[state.size() - 1] = 2;
                    state.push_back(1);
                    int if_position = tree.size();
                    Node node_if("IF", if_position);
                    tree.push_back(node_if);

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
                        int cond_position = tree.size();
                        Node node_cond("Condition", cond_position);
                        tree.push_back(node_cond);
                        tree[if_position].children.push_back(cond_position);
                        vector<int> parent_positions;
                        parent_positions.push_back(cond_position);
                        int depth = 1;
                        while (depth > 0) {
                            position = next_position;
                            next_position = find_next_element(grammar, position);
                            current_lexxem = grammar.substr(position, next_position - position);
                            name = parse_new_element(current_lexxem);
                            next_position = correct_next_position(grammar, next_position);
                            if (name == "Arg") {
                                int var_position = tree.size();
                                Node node_var("Variable", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);

                                int number = parse_new_element_number(current_lexxem);
                                Node node_arg(arguments[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_arg);
                            } else if (name == "Val") {
                                int var_position = tree.size();
                                Node node_var("Value", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);

                                int number = parse_new_element_number(current_lexxem);
                                Node node_val(values[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_val);
                            } else if (name == "OR") {
                                int var_position = tree.size();
                                Node node_var("OR", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "AND") {
                                int var_position = tree.size();
                                Node node_var("AND", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "NOT") {
                                int var_position = tree.size();
                                Node node_var("NOT", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "EQUAL") {
                                int var_position = tree.size();
                                Node node_var("EQUAL", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "GREATER") {
                                int var_position = tree.size();
                                Node node_var("GREATER", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "LESS") {
                                int var_position = tree.size();
                                Node node_var("LESS", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "(") {
                                depth++;
                                int cond_position_rec = tree.size();
                                Node node_cond_rec("Condition", cond_position);
                                tree.push_back(node_cond_rec);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(cond_position_rec);
                                parent_positions.push_back(cond_position_rec);
                            } else if (name == ")") {
                                depth--;
                                parent_positions.pop_back();
                            }
                        }
                    } else {
                        cout << "Error\n";
                        exit(1);
                    }
                    int file_position = tree.size();
                    Node node_file("File", file_position);
                    tree.push_back(node_file);
                    tree[file_position].children.push_back(if_position);
                    next_position = parse(grammar, next_position, tree, state, if_position);
                    state.pop_back();
                    return parse(grammar, next_position, tree, state, file_position);
                } else if ((state[state.size() - 1] == 2) || (state[state.size() - 1] == 1)) {
                    //state[state.size() - 1] = 1;
                    state.push_back(1);
                    int if_position = tree.size();
                    Node node_if("IF", if_position);
                    tree.push_back(node_if);
                    tree[current_node].children.push_back(if_position);

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
                        int cond_position = tree.size();
                        Node node_cond("Condition", cond_position);
                        tree.push_back(node_cond);
                        tree[if_position].children.push_back(cond_position);
                        vector<int> parent_positions;
                        parent_positions.push_back(cond_position);
                        int depth = 1;
                        while (depth > 0) {
                            position = next_position;
                            next_position = find_next_element(grammar, position);
                            current_lexxem = grammar.substr(position, next_position - position);
                            name = parse_new_element(current_lexxem);
                            next_position = correct_next_position(grammar, next_position);
                            if (name == "Arg") {
                                int var_position = tree.size();
                                Node node_var("Variable", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);

                                int number = parse_new_element_number(current_lexxem);
                                Node node_arg(arguments[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_arg);
                            } else if (name == "Val") {
                                int var_position = tree.size();
                                Node node_var("Value", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);

                                int number = parse_new_element_number(current_lexxem);
                                Node node_val(values[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_val);
                            } else if (name == "OR") {
                                int var_position = tree.size();
                                Node node_var("OR", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "AND") {
                                int var_position = tree.size();
                                Node node_var("AND", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "NOT") {
                                int var_position = tree.size();
                                Node node_var("NOT", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "EQUAL") {
                                int var_position = tree.size();
                                Node node_var("EQUAL", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "GREATER") {
                                int var_position = tree.size();
                                Node node_var("GREATER", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "LESS") {
                                int var_position = tree.size();
                                Node node_var("LESS", var_position);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(var_position);
                                tree.push_back(node_var);
                            } else if (name == "(") {
                                depth++;
                                int cond_position_rec = tree.size();
                                Node node_cond_rec("Condition", cond_position);
                                tree.push_back(node_cond_rec);
                                tree[parent_positions[parent_positions.size() - 1]].children.push_back(cond_position_rec);
                                parent_positions.push_back(cond_position_rec);
                            } else if (name == ")") {
                                depth--;
                                parent_positions.pop_back();
                            }
                        }
                    } else {
                        cout << "Error\n";
                        exit(1);
                    }
                    cout << "entered if statement !!!!!!!!!!!!!!!!!!!!!!\n";
                    //print_tree(tree);
                    next_position = parse(grammar, next_position, tree, state, if_position);
                    //print_tree(tree);
                    cout << "left if statement !!!!!!!!!!!!!!!!!!!!!!\n";
                    state.pop_back();
                    return parse(grammar, next_position, tree, state, current_node);
                }
            } else if (name[1] == 'd') {
                cout << current_lexxem << " went to id\n";
                for (int jj : state) {
                    cout << jj << " ";
                }
                cout << endl;

                //tree[current_node].children.push_back(node_position);
                int number = parse_new_element_number(current_lexxem);
                Node node_id(function_identifiers[number], tree.size());
                tree.push_back(node_id);

                int node_position = tree.size();
                Node node("FunctionId", node_position);
                tree.push_back(node);
                tree[node_position].children.push_back(tree.size() - 2);

                if (state[state.size() - 1] == 0) {
                    //cout << "STATE = 0\n";
                    state[state.size() - 1] = 2;
                    int com_position = tree.size();
                    Node node_command("CommandInvocation", com_position);
                    node_command.children.push_back(node_position);
                    tree.push_back(node_command);

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
//                        Node node_par_1("(", tree.size());
//                        tree[com_position].children.push_back(tree.size());
//                        tree.push_back(node_par_1);

                        int arg_position = tree.size();
                        Node node_arguments("Arguments", arg_position);
                        tree[com_position].children.push_back(arg_position);
                        tree.push_back(node_arguments);
                        while (name != ")") {
                            position = next_position;
                            next_position = find_next_element(grammar, position);
                            current_lexxem = grammar.substr(position, next_position - position);
                            name = parse_new_element(current_lexxem);
                            next_position = correct_next_position(grammar, next_position);
                            if (name == "Arg") {
                                int var_position = tree.size();
                                Node node_var("Variable", var_position);
                                tree[arg_position].children.push_back(var_position);
                                tree.push_back(node_var);

                                number = parse_new_element_number(current_lexxem);
                                Node node_arg(arguments[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_arg);
                            } else if (name == "Val") {
                                int var_position = tree.size();
                                Node node_var("Value", var_position);
                                tree[arg_position].children.push_back(var_position);
                                tree.push_back(node_var);

                                number = parse_new_element_number(current_lexxem);
                                Node node_val(values[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_val);
                            }
                        }
//                        Node node_par_2(")", tree.size());
//                        tree[com_position].children.push_back(tree.size());
//                        tree.push_back(node_par_2);
                    } else {
                        cout << "Error\n";
                        exit(1);
                    }
                    int file_position = tree.size();
                    Node node_file("File", file_position);
                    tree.push_back(node_file);
                    tree[file_position].children.push_back(com_position);
                    return parse(grammar, next_position, tree, state, file_position);
                } else if ((state[state.size() - 1] == 2) || (state[state.size() - 1] == 1)) {
                    cout << "STATE = 1 or 2\n";
                    int com_position = tree.size();
                    Node node_command("CommandInvocation", com_position);
                    node_command.children.push_back(node_position);
                    tree.push_back(node_command);
                    tree[current_node].children.push_back(com_position);

                    //cout << current_node << ". " << tree[current_node].name << endl;

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
//                        Node node_par_1("(", tree.size());
//                        tree[com_position].children.push_back(tree.size());
//                        tree.push_back(node_par_1);

                        int arg_position = tree.size();
                        Node node_arguments("Arguments", arg_position);
                        tree[com_position].children.push_back(arg_position);
                        tree.push_back(node_arguments);
                        while (name != ")") {
                            position = next_position;
                            next_position = find_next_element(grammar, position);
                            current_lexxem = grammar.substr(position, next_position - position);
                            name = parse_new_element(current_lexxem);
                            next_position = correct_next_position(grammar, next_position);
                            if (name == "Arg") {
                                int var_position = tree.size();
                                Node node_var("Variable", var_position);
                                tree[arg_position].children.push_back(var_position);
                                tree.push_back(node_var);

                                number = parse_new_element_number(current_lexxem);
                                Node node_arg(arguments[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_arg);
                            } else if (name == "Val") {
                                int var_position = tree.size();
                                Node node_var("Value", var_position);
                                tree[arg_position].children.push_back(var_position);
                                tree.push_back(node_var);

                                number = parse_new_element_number(current_lexxem);
                                Node node_val(values[number], tree.size());
                                tree[var_position].children.push_back(tree.size());
                                tree.push_back(node_val);
                            }
                        }
//                        Node node_par_2(")", tree.size());
//                        tree[com_position].children.push_back(tree.size());
//                        tree.push_back(node_par_2);
                    } else {
                        cout << "Error\n";
                        exit(1);
                    }
                    //print_tree(tree);
                    return parse(grammar, next_position, tree, state, current_node);
                }
            }
            break;
        case 'V':
            if (name[1] == 'a') {
                int node_position = tree.size();
                Node node("Value", node_position);
                tree.push_back(node);
                //tree[current_node].children.push_back(node_position);
                int number = parse_new_element_number(current_lexxem);
                Node node_id(values[number], tree.size());
                tree[node_position].children.push_back(tree.size());
                tree.push_back(node_id);
            }
            break;
        case 'A':
            if (name[1] == 'r') {
                int node_position = tree.size();
                Node node("Argument", node_position);
                tree.push_back(node);
                //tree[current_node].children.push_back(node_position);
                int number = parse_new_element_number(current_lexxem);
                Node node_id(arguments[number], tree.size());
                tree[node_position].children.push_back(tree.size());
                tree.push_back(node_id);
            }
            break;
        case '(':
            break;
        case ')':
            break;
        case 'E':
            if (name == "ELSE") {
                if (state[state.size() - 1] == 1) {
                    //state.push_back(1);
                    //cout << "STATE = 0\n";
                    //state[state.size() - 1] = 2;
                    int el_position = tree.size();
                    Node node_el("ELSE", el_position);
                    //node_command.children.push_back(node_position);
                    tree.push_back(node_el);
                    tree[current_node].children.push_back(el_position);

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
                        position = next_position;
                        next_position = find_next_element(grammar, position);
                        current_lexxem = grammar.substr(position, next_position - position);
                        name = parse_new_element(current_lexxem);
                        next_position = correct_next_position(grammar, next_position);
                        if (name != ")") {
                            cout << "ERROR";
                            exit(1);
                        }
                    } else {
                        cout << "ERROR";
                        exit(1);
                    }
                    //state.pop_back();
                    //TVOYA OSHIBKA
                    return parse(grammar, next_position, tree, state, el_position);
                    //state.pop_back();
                    //return parse(grammar, next_position, tree, state, current_node);
                } else {
                    cout << "ERROR\n";
                    exit(1);
                }
            }
            if (name == "ENDIF") {
                if (state[state.size() - 1] == 1) {
//                    int end_position = tree.size();
//                    Node node_end("ENDIF", end_position);
//                    //node_command.children.push_back(node_position);
//                    tree.push_back(node_end);
//                    tree[current_node].children.push_back(end_position);

                    position = next_position;
                    next_position = find_next_element(grammar, position);
                    current_lexxem = grammar.substr(position, next_position - position);
                    name = parse_new_element(current_lexxem);
                    next_position = correct_next_position(grammar, next_position);
                    if (name == "(") {
                        position = next_position;
                        next_position = find_next_element(grammar, position);
                        current_lexxem = grammar.substr(position, next_position - position);
                        name = parse_new_element(current_lexxem);
                        next_position = correct_next_position(grammar, next_position);
                        if (name != ")") {
                            cout << "ERROR";
                            exit(1);
                        }
                    }
                } else {
                    cout << "ERROR";
                    exit(1);
                }
                return next_position;
            }
            break;
        default:
            cout << "Bad";
            return -1;
    }
    //print_tree(tree);
    return 0;
    //return parse(grammar, next_position, tree, state, 0);
}

bool try_combine(const vector<int>& state) {
    return false;
}

int correct_next_position(string grammar, int next_position) {
    for (int i = next_position; i < grammar.length(); i++) {
        if (grammar[i] == '<' || i == grammar.length() - 1) {
            next_position = i;
            break;
        }
    }
}

int find_next_element(string grammar, int position) {
    if (grammar[position] != '<') cout << "smtg wrong\n";
    for (int i = position; i < grammar.length(); i++) {
        if (grammar[i] == '>') {
            position = i + 1;
            break;
        }
    }
    return position;
}

string parse_new_element(string expr) {
    string name;
    for (int i = 1; i < expr.length(); i++) {
        if (expr[i] == ',' || expr[i] == '>') {
            break;
        } else {
            name.push_back(expr[i]);
        }
    }
    return name;
}

int parse_new_element_number(string expr) {
    string number;
    bool is_part = false;
    for (int i = 1; i < expr.length(); i++) {
        if (expr[i] == ',') {
            is_part = true;
        } else if (expr[i] == '>'){
            break;
        } else if (is_part) {
            number.push_back(expr[i]);
        }
    }

    int num = 0;
    for (int i = 0; i < number.length(); i++) {
        //cout << number[i] << endl;
        num = num * 10 + ((int)number[i] - 48);
    }
    //cout << number << "=" << num << endl;
    return num;
}

int print_tree(vector<Node> tree) {
//    for (int i = 0; i < tree.size(); i++) {
//        cout << "Name=" << tree[i].name << endl;
//        cout << "Children=";
//        for (int j = 0; j < tree[i].children.size(); j++) {
//            cout << tree[i].children[j] << ", ";
//        }
//        cout << endl;
//        cout << "------------------------------------\n";
//    }
    printf("graph {\n");
    for (int i = 0; i < tree.size(); i++) {
        cout << '\t' << '\"' << i << ". " << tree[i].name << '\"' << endl;
    }

    for (int i = 0; i < tree.size(); i++) {
        for (int j = 0; j < tree[i].children.size(); j++) {
            int k = tree[i].children[j];
            cout << '\t' << '\"' << i << ". " << tree[i].name << '\"' << " -- " << '\"' << k << ". " << tree[k].name << '\"' << endl;
        }
    }
    printf("}");
}