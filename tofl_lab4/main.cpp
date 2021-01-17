#include <iostream>
#include <regex>
#include <iterator>
#include <string>
#include <fstream>

int main() {
    setlocale(LC_ALL, "ru");
    std::ifstream infile("input.txt");

    std::string s = ".Получили ответ 5.321. Значит значение выражения 7.71 + 8,88 равно 578.9212. Замысел эпопеи формировался задолго до начала работы над тем текстом, который известен под названием «Война и мир». В наброске предисловия к «Войне и миру» Толстой писал, что в 1856 году начал писать повесть, «герой которой должен был быть декабрист, возвращающийся с семейством в Россию. Невольно от настоящего я перешёл к 1825 году… Но и в 1825 году герой мой был уже возмужалым, семейным человеком. Чтобы понять его, мне нужно было перенестись к его молодости, и молодость его совпала с … эпохой 1812 года… Ежели причина нашего торжества была не случайна, но лежала в сущности характера русского народа и войска, то характер этот должен был выразиться ещё ярче в эпоху неудач и поражений…» Так Лев Николаевич постепенно пришёл к необходимости начать повествование с 1805 года. Получили ответ 5.921. Значит 123,4567890 значение выражения 2.65 + 3.27 равно 578.9212. ";
    std::regex words_regex("([^\\..]*?\\d+\\.\\d{2}[^\\..]*?\\d+\\,\\d+.*?\\.[^\\d])|([^\\..]*?\\d+\\,\\d+[^\\..]*?\\d+\\.\\d{2}.*?\\.[^\\d])");
    //([^\..]*?\d+\.\d{2}[^\..]*?\d+\,\d+.*?\.[^\d])|([^\..]*?\d+\,\d+[^\..]*?\d+\.\d{2}.*?\.[^\d])
    auto words_begin = std::sregex_iterator (s.begin(), s.end(), words_regex);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {

        std::smatch smatch = *i;
        std::string match_str = smatch.str();

        std::regex num_regex("(\\d+\\.\\d{2})|(\\d+\\,\\d+)");
        auto num_begin = std::sregex_iterator (match_str.begin(), match_str.end(), num_regex);
        auto num_end = std::sregex_iterator();
        for (std::sregex_iterator j = num_begin; j != num_end; ++j) {
            std::smatch nmatch = *j;
            std::string num_match_str = nmatch.str();
            std::cout << "<n> " << num_match_str << " </n>" << std::endl;
        }
        std::cout << "<p>" << match_str << "</p>" << std::endl;
    }
    return 0;
}