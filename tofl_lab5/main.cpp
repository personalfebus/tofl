#include <iostream>
#include <string>
#include <vector>

using namespace std;

int lex(int position, string program);
int lex_quoted_argument(int position, string program);
int lex_unquoted_argument(int position, string program);
int lex_quoted_argument(int position, string program);
int lex_argument(int position, string program);
int lex_identifier(int position, string program);
int lex_line_comment(int position, string program);
int lex_bracket_comment(int position, string program);
bool is_number(int code);
bool is_latin(int code);
bool is_separated(char a);
bool is_space_sym(char a);

string output_grammar;
vector<string> function_identifiers;
vector<string> arguments;
vector<string> values;
int newline_count = 1;

int main() {
    string program;
    for (string line; getline(cin, line);){
        program.append(line);
        program.push_back('\n');
    }
    if (program.length() > 0) {
        program.pop_back();
    }
    lex(0, program);
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
    return 0;
}

int lex(int position, string program) {
    for (int i = 0; i < program.length(); i++) {
        int code = (int)program[i];
        if (is_number(code) || program[i] == '$' || program[i] == '%' || program[i] == '-' || program[i] == '=') {
            i = lex_unquoted_argument(i, program);
            i--;
        } else if (program[i] == '\"') {
            i = lex_quoted_argument(i + 1, program);
            i--;
        } else if (program[i] == '(') {
            output_grammar.append("<(>");
        } else if (program[i] == ')') {
            output_grammar.append("<)>");
        } else if (program[i] == '#') {
            if ((i < program.length() - 2) && (program[i + 1] == '[') && (program[i + 2] == '[')) {
                i = lex_bracket_comment(i, program);
                i--;
            } else {
                i = lex_line_comment(i, program);
                i--;
            }
        } else if (program[i] == 'I') {
            if ((i < program.length() - 2) && (program[i + 1] == 'F') && (is_separated(program[i + 2]))) {
                output_grammar.append("<IF>");
                i++;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'O') {
            if ((i < program.length() - 2) && (program[i + 1] == 'R') && (is_separated(program[i + 2]))) {
                output_grammar.append("<OR>");
                i++;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'N') {
            if ((i < program.length() - 3) && (program[i + 1] == 'O')  && (program[i + 2] == 'T')
            && (is_separated(program[i + 3]))) {
                output_grammar.append("<NOT>");
                i += 2;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'A') {
            if ((i < program.length() - 3) && (program[i + 1] == 'N')  && (program[i + 2] == 'D')
                && (is_separated(program[i + 3]))) {
                output_grammar.append("<AND>");
                i += 2;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'G') {
            if ((i < program.length() - 7) && (program[i + 1] == 'R') && (program[i + 2] == 'E')
            && (program[i + 3] == 'A')  && (program[i + 4] == 'T')  && (program[i + 5] == 'E')
            && (program[i + 6] == 'R') && (is_separated(program[i + 7]))) {
                output_grammar.append("<GREATER>");
                i += 6;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'L') {
            if ((i < program.length() - 4) && (program[i + 1] == 'E') && (program[i + 2] == 'S')
                && (program[i + 3] == 'S') && (is_separated(program[i + 4]))) {
                output_grammar.append("<LESS>");
                i += 3;
            } else {
                i = lex_identifier(i, program);
                i--;
            }
        } else if (program[i] == 'E') {
            if ((i < program.length() - 4) && (program[i + 1] == 'L') && (program[i + 2] == 'S')
                && (program[i + 3] == 'E') && (is_separated(program[i + 4]))) {
                output_grammar.append("<ELSE>");
                i += 3;
            } else if ((i < program.length() - 6) && (program[i + 1] == 'L') && (program[i + 2] == 'S')
                       && (program[i + 3] == 'E') && (program[i + 4] == 'I') && (program[i + 5] == 'F')
                       && (is_separated(program[i + 6]))) {
                output_grammar.append("<ELSEIF>");
                i += 5;
            } else if ((i < program.length() - 5) && (program[i + 1] == 'Q') && (program[i + 2] == 'U')
                       && (program[i + 3] == 'A') && (program[i + 4] == 'L') && (is_separated(program[i + 5]))) {
                output_grammar.append("<EQUAL>");
                i += 4;
            } else if ((i < program.length() - 5) && (program[i + 1] == 'N') && (program[i + 2] == 'D')
                       && (program[i + 3] == 'I') && (program[i + 4] == 'F') && (is_separated(program[i + 5]))) {
                output_grammar.append("<ENDIF>");
                i += 4;
            }
        } else if (is_latin(code)) {
            i = lex_argument(i, program);
            i--;
        } else if (program[i] == '\n') {
            if ((output_grammar.length() > 1) && output_grammar[output_grammar.length() - 2] == ')') {
                output_grammar.push_back('\n');
            }
            newline_count++;
        } else if (is_space_sym(program[i])) {

        } else {
            cout << "Bad symbol " << program[i] << " on position " << i << " line " << newline_count << endl;
            //exit(1);
        }
    }
    return 0;
}

int lex_identifier(int position, string program) {
    string identifier;
    string output;
    for (int i = position; i < program.length(); i++) {
        int code = (int)program[i];
        if (program[i] == '$' || program[i] == '%' || program[i] == '-' || program[i] == '=') {
            cout << "Bad symbol " << program[i] << " in indentifier in line " << newline_count << endl;
            return program.length();
        }
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

int lex_unquoted_argument(int position, string program) {
    string arg;
    string output;

    for (int i = position; i < program.length(); i++) {
        if ((program[i] == '(') || (program[i] == ')')
            || (program[i] == '\\') || (program[i] == ' ') || (program[i] == '\t') || (program[i] == '\n')) {
            position = i;
            bool is_value = true;
            if (!is_number(arg[0])) is_value = false;
            else {
                for (char j : arg) {
                    if (!is_number(j) && j != '.' && j != ',') {
                        is_value = false;
                        cout << "Bad symbol in value in line " << newline_count << endl;
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

bool is_latin(int code){
    return ((code > 64) && (code < 91)) || ((code > 96) && (code < 123));
}

bool is_number(int code){
    return (code > 47) && (code < 58);
}

bool is_separated(char a) {
    return (a == '\t') || (a == ' ') || (a == '\n') || (a == '(');
}

bool is_space_sym(char a) {
    return (a == '\t') || (a == ' ');
}

int lex_line_comment(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] == '\n') {
            newline_count++;
            position = i + 1;
            break;
        }
    }
    return position;
}

int lex_bracket_comment(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] == '\n') newline_count++;
        if ((program[i] == ']') && (i < program.length() - 1) && (program[i + 1] == ']')) {
            position = i + 2;
            break;
        }
    }
    return position;
}

int lex_argument(int position, string program) {
    for (int i = position; i < program.length(); i++) {
        if (program[i] == '(') {
            return lex_identifier(position, program);
        } else if (program[i] == '$' || program[i] == '%' || is_space_sym(program[i])) {
            return lex_unquoted_argument(position, program);
        }
    }
    return lex_unquoted_argument(position, program);
}