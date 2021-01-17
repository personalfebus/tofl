#include <iostream>
#include <vector>
#include <unordered_map>

static std::vector<int> true_base;

void print_permur(std::vector<int> first, int orb, std::vector<int> orbit);
bool is_identity(std::vector<int> first);
bool base_resolver(std::vector<std::vector<int>> table, int n, std::vector<int> cur_base, std::vector<int> cur_gen);

void build_multi_table() {
    using namespace std;
    cout << "Введите число n для генерации Z(n)\n";
    int n;
    cin >> n;

    int k = 0;
    for (int j = 0; j < n; j++) {
        if (!j) cout << "|  *";
        if (j < 10) cout << "|  " << j;
        else cout << "| " << j;
    }
    cout << "|" << endl;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++) {
            if (!j) {
                if (i < 10) cout << "|  " << i;
                else cout << "| " << i;
            }
            if (i * j % n < 10) cout << "|  " << i * j % n;
            else cout << "| " << i * j % n;
        }
        cout << "|" << endl;
    }
}

void permut_analysis() {
    using namespace std;
    cout << "Введите элемент для которого будет найдена орбита\n";
    int orb;
    cin >> orb;
    cout << "Введите перестановку или подстановку\n";
    vector<int> orbit;
    vector<int> first;
    for (int i = 0; i < 7; i++){
        int a;
        cin >> a;
        first.push_back(a);
    }

    vector<int> current = first;
    vector<int> tmp = first;
    orbit.push_back(current[orb]);

    for (int k = 1; k < 15; k++) {
        if (is_identity(current)) break;
        cout << k << ") ";
        print_permur(current, orb, orbit);
        for (int i = 0; i < current.size(); i++) {
            current[i] = tmp[first[i]];
            if (i == orb) {
                bool is_present = false;
                for (int j = 0; j < orbit.size(); j++) {
                    if (current[i] == orbit[j]) {
                        is_present = true;
                        break;
                    }
                }
                if (!is_present) orbit.push_back(current[i]);
            }
        }
        tmp = current;
        if (k == 14) cout << "Порядок бесконечный\n";
    }
    std::cout << "Орбита элемента " << orb << ": ";
    for (int i = 0; i < orbit.size(); i++) {
        std::cout << orbit[i] << " ";
    }
    std::cout << std::endl;
}

bool is_identity(std::vector<int> first) {
    for (int i = 0; i < first.size(); i++) {
        if (i != first[i]) return false;
    }
    return true;
}

void print_permur(std::vector<int> first, int orb, std::vector<int> orbit) {
    std::vector<bool> is_visited;
    for (int i = 0; i < 7; i++){
        is_visited.push_back(false);
    }
    for (int i = 0; i < first.size(); i++) {
        std::cout << i << " --> " << first[i] << ", ";
    }

    bool is_permut = true;
    for (int i = 0; i < first.size(); i++) {
        for (int j = i + 1; j < first.size(); j++) {
            if (first[i] == first[j]) {
                is_permut = false;
                break;
            }
        }
    }

    if (is_permut) {
        std::cout << " Циклический вид: (";
        int last = 0;

        for (;;) {
            is_visited[last] = true;
            std::cout << last;
            last = first[last];

            if (is_visited[last]) {
                std::cout << ")";
                last = is_visited.size();
                for (int i = 0; i < is_visited.size(); i++) {
                    if (!is_visited[i]) {
                        last = i;
                        break;
                    }
                }
                if (last == is_visited.size()) break;
                std::cout << "(";
            }
        }
    }
    std::cout << std::endl;
}

void parse_tables() {
    using namespace std;
    cout << "Введите мощность алфавита\n";
    int n;
    cin >> n;
    cout << "Введите элементы\n";
    unordered_map<char, int> sym_to_num;
    for (int i = 0; i < n; i++) {
        char a;
        cin >> a;
        sym_to_num.insert(make_pair(a, i));
    }

    cout << "Введите таблицу Кэли\n";
    vector<vector<int>> table;
    for (int i = 0; i < n; i++) {
        vector<int> vc;
        table.push_back(vc);
        for (int j = 0; j < n; j++) {
            char a;
            cin >> a;
            table[i].push_back(sym_to_num[a]);
            //cout << table[i][j] << " ";
        }
        //cout << endl;
    }

    bool is_associative = true;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                // (i * j) * k || i * (j * k)
                if (table[table[i][j]][k] != table[i][table[j][k]]) {
                    cout << "\nСломался на " << i << " " << j << " " << k << endl;
                    is_associative = false;
                    break;
                }
            }
            if (!is_associative) break;
        }
        if (!is_associative) break;
    }
    if (is_associative) cout << "\nАссоциативность выполняется\n";
    else cout << "\nАссоциативность не выполняется\n";

    int left_neutral_elem = -1;
    for (int i = 0; i < n; i++) {
        bool is_left_neutral = true;

        for (int j = 0; j < n; j++) {
            if (table[i][j] != j) {
                is_left_neutral = false;
                break;
            }
        }

        if (is_left_neutral) {
            left_neutral_elem = i;
            break;
        }
    }

    if (left_neutral_elem > -1) {
        char ans;
        for (auto mp : sym_to_num) {
            if (mp.second == left_neutral_elem) {
                ans = mp.first;
            }
        }
        cout << "Найдена левая еденица " << ans << endl;
    }
    else cout << "Левой единицы нет\n";

    int right_neutral_elem = -1;
    for (int i = 0; i < n; i++) {
        bool is_right_neutral = true;

        for (int j = 0; j < n; j++) {
            if (table[j][i] != j) {
                is_right_neutral = false;
                break;
            }
        }

        if (is_right_neutral) {
            right_neutral_elem = i;
            break;
        }
    }

    if (right_neutral_elem > -1) {
        char ans;
        for (auto mp : sym_to_num) {
            if (mp.second == right_neutral_elem) {
                ans = mp.first;
            }
        }
        cout << "Найдена правая еденица " << ans << endl;
    }
    else cout << "Правой единицы нет\n";

    for (int i = 0; i < n; i++) {
        true_base.push_back(i);
    }
    for (int i = 0; i < n; i++) {
        vector<int> tmp_b;
        vector<int> tmp_all;
        tmp_b.push_back(i);
        tmp_all.push_back(i);
        bool end = base_resolver(table, n, tmp_b, tmp_all);
        //if (end) break;
    }

    cout << "Пораждающие = <";
    for (int i = 0; i < true_base.size(); i++) {
        char ans;
        for (auto mp : sym_to_num) {
            if (mp.second == true_base[i]) {
                ans = mp.first;
            }
        }
        cout << ans;
        if (i == true_base.size() - 1) cout << ">";
        else cout << ", ";
    }
    cout << endl;
}

bool base_resolver(std::vector<std::vector<int>> table, int n, std::vector<int> cur_base, std::vector<int> cur_gen) {
    for (int i = 0; i < cur_gen.size(); i++) {
        for (int j = 0; j < cur_gen.size(); j++) {
            int elem = table[cur_gen[i]][cur_gen[j]];
            bool is_new = true;
            for (int k = 0; k < cur_gen.size(); k++) {
                if (elem == cur_gen[k]) {
                    is_new = false;
                    break;
                }
            }
            if (is_new) {
                cur_gen.push_back(elem);
                i = -1;
                break;
            }
        }
        if (cur_gen.size() == n) {
            if (cur_base.size() < true_base.size()) true_base = cur_base;
            return true;
        }
    }

    for (int i = 0; i < n; i++) {
        bool is_new = true;
        for (int j = 0; j < cur_base.size(); j++) {
            if (cur_base[j] == i) {
                is_new = false;
                break;
            }
        }
        if (is_new) {
            std::vector<int> tmp = cur_base;
            tmp.push_back(i);
            cur_gen = tmp;
            base_resolver(table, n, tmp, cur_gen);
        }
    }

    return false;
}

int main() {
    //build_multi_table();
    //permut_analysis();
    parse_tables();
    return 0;
}