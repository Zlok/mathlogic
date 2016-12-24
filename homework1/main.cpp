//
// Created by Appolinariya on 31.10.16.
//
#include <iostream>
#include <string>
#include <cstdio>
#include <utility>
#include <memory>
#include <vector>
#include <map>
#include <ctime>

std::string my_split(std::string tmp) {
    std::string t;
    size_t i = 0, n = tmp.size();
    while (i < n) {
        if (tmp[i] != ' ')
            t.push_back(tmp[i]);
        i++;
    }
    return t;
}

std::string brackets_check(std::string tmp) {
    std::string t;
    int count = 0;
    if (tmp[0] == '(' && tmp[tmp.size() - 1] == ')') {
        size_t i = 1, n = tmp.size() - 1;
        while (i < n) {
            t.push_back(tmp[i]);
            if (tmp[i] == '(')
                count++;
            else if (tmp[i] == ')') {
                if (count == 0)
                    return tmp;
                count--;
            }
            i++;
        }
        return t;
    }
    return tmp;
}

std::string del_brackets(std::string tmp) {
    std::string t;
    size_t n = tmp.size();
    for (size_t i = 0; i < n; i++)
        if (tmp[i] != '(' && tmp[i] != ')')
            t.push_back(tmp[i]);
    return t;
}

struct expression {
    struct node {
        std::string value;
        std::string oper;
        std::shared_ptr<node> left, right;
        node() : oper(), value(), left(NULL), right(NULL) {
        }
        node(std::string v) : value(v), oper(), left(NULL), right(NULL) {
            std::string s;
            while (true) {
                s = brackets_check(value);
                if (s.size() == value.size())
                    break;
                value = s;
            }
        }
        ~node() {
            if (left != NULL)
                left = NULL;
            if (right != NULL)
                right = NULL;
        }

        void parse() {
            std::string left_v, right_v;
            size_t n = value.size(), i;
            unsigned counter = 0;
            for (i = 0; i < n; i++) {
                if (value[i] == '(') {
                    counter++;
                } else if (value[i] == ')') {
                    counter--;
                } else if (counter == 0 && value[i] == '-' && value[i + 1] == '>') {
                    oper.push_back('-');
                    oper.push_back('>');
                    break;
                }
            }

            if (oper.size()) {
                for (size_t j = 0; j < i; j++)
                    left_v.push_back(value[j]);
                for (size_t j = i + 2; j < n; j++)
                    right_v.push_back(value[j]);
                left = std::shared_ptr<node>(new node(left_v));
                right = std::shared_ptr<node>(new node(right_v));
                left->parse();
                right->parse();
                value = del_brackets(value);
                return;
            }

            for (i = 0; i < n; i++) {
                if (value[i] == '(') {
                    counter++;
                } else if (value[i] == ')') {
                    counter--;
                } else if (counter == 0 && value[i] == '|') {
                    oper.push_back('|');
                    break;
                }
            }

            if (oper.size()) {
                for (size_t j = 0; j < i; j++)
                    left_v.push_back(value[j]);
                for (size_t j = i + 1; j < n; j++)
                    right_v.push_back(value[j]);
                left = std::shared_ptr<node>(new node(left_v));
                right = std::shared_ptr<node>(new node(right_v));
                left->parse();
                right->parse();
                value = del_brackets(value);
                return;
            }

            for (i = 0; i < n; i++) {
                if (value[i] == '(') {
                    counter++;
                } else if (value[i] == ')') {
                    counter--;
                } else if (counter == 0 && value[i] == '&') {
                    oper.push_back('&');
                    break;
                }
            }

            if (oper.size()) {
                for (size_t j = 0; j < i; j++)
                    left_v.push_back(value[j]);
                for (size_t j = i + 1; j < n; j++)
                    right_v.push_back(value[j]);
                left = std::shared_ptr<node>(new node(left_v));
                right = std::shared_ptr<node>(new node(right_v));
                left->parse();
                right->parse();
                value = del_brackets(value);
                return;
            }

            if (value[0] == '!') {
                oper.push_back('!');
                for (size_t j = 1; j < n; j++)
                    right_v.push_back(value[j]);
                right = std::shared_ptr<node>(new node(right_v));
                right->parse();
                value = del_brackets(value);
                return;
            }
            value = del_brackets(value);
        }

        bool operator==(node& other) {
            if (oper.size()) {
                if (oper != other.oper)
                    return false;
                if (value != other.value)
                    return false;
                return ((oper == "!" || (*left == *(other.left))) && (*right == *(other.right)));
            } else {
                if (other.oper.size())
                    return false;
                return value == other.value;
            }
        }

    };

    std::shared_ptr<node> expr;

    expression() {
    }

    expression(std::string s) {
        std::string tmp = my_split(s);
        expr = std::shared_ptr<node>(new node(tmp));
        expr->parse();
    }

    expression(const expression& other) {
        expr = other.expr;
    }

    ~expression() {
        expr = NULL;
    }
};

bool equally(std::shared_ptr<expression::node> ex, std::shared_ptr<expression::node> tmp, std::map<std::string, std::string>& match) {
    if (!ex->oper.size()) {
        std::map<std::string, std::string>::iterator it = match.find(ex->value);
        if (it == match.end()) {
            match.insert(std::make_pair(ex->value, tmp->value));
            return true;
        } else {
            return (match[ex->value] == tmp->value);
        }
    } else {
        if (ex->oper == tmp->oper)
            if (ex->oper == "!" || equally(ex->left, tmp->left, match))
                return equally(ex->right, tmp->right, match);
        return false;
    }
}

bool equally(expression ax, expression tmp) {
    std::map<std::string, std::string> match;
    return equally(ax.expr, tmp.expr, match);
}

expression head, state;
std::vector<expression> assumps;
std::vector<expression> proofs;
std::vector<expression> axioms;

void create_axioms() {
    /*1*/axioms.push_back(expression("A->B->A"));
    /*2*/axioms.push_back(expression("(A->B)->(A->B->C)->(A->C)"));
    /*3*/axioms.push_back(expression("A->B->A&B"));
    /*4*/axioms.push_back(expression("A&B->A"));
    /*5*/axioms.push_back(expression("A&B->B"));
    /*6*/axioms.push_back(expression("A->A|B"));
    /*7*/axioms.push_back(expression("B->A|B"));
    /*8*/axioms.push_back(expression("(A->C)->(B->C)->(A|B->C)"));
    /*9*/axioms.push_back(expression("(A->B)->(A->!B)->!A"));
    /*10*/axioms.push_back(expression("!!A->A"));
}

void parse_head(std::string h) {
    size_t n = h.size();
    size_t i = 0;
    unsigned count = 0;
    std::string tmp;
    while (i < n) {
        if (h[i] == '|' && h[i + 1] == '-') {
            if (tmp.size()) {
                count++;
                assumps.push_back(expression(tmp));
                tmp.clear();
                tmp.resize(0);
            }
            i += 2;
        } else if (h[i] == ',') {
            count++;
            assumps.push_back(tmp);
            tmp.clear();
            tmp.resize(0);
            i++;
        } else if (h[i] != ' ') {
            tmp.push_back(h[i]);
            i++;
        } else {
            i++;
        }
    }
    state = tmp;
}

expression tmp;

int is_assump() {
    size_t n = assumps.size();
    for (size_t i = 0; i < n; i++) {
        if (*(tmp.expr) == *(assumps[i].expr))
            return i + 1;
    }
    return 0;
}

int is_axiom() {
    size_t n = axioms.size();
    for (size_t i = 0; i < n; i++)
        if (equally(axioms[i], tmp))
            return i + 1;
    return 0;
}

std::pair<int, int> is_mp() {
    size_t n = proofs.size();
    for (size_t i = 0; i < n; i++) {
        if (proofs[i].expr->oper == "->" && *(tmp.expr) == *(proofs[i].expr->right))
            for (size_t j = 0; j < n; j++) {
                if (*(proofs[j].expr) == *(proofs[i].expr->left))
                    return std::make_pair(i + 1, j + 1);
            }
    }
    return std::make_pair(0, 0);
};

std::string str;

void check() {
    static unsigned counter = 1;
    std::cout << "(" << counter << ")" << " " << str << " ";
    counter++;
    int t1 = is_assump();
    if (t1)
        std::cout << "(Предп. " << t1 << ")" << std::endl;
    else {
        t1 = is_axiom();
        if (t1)
            std::cout << "(Сх. акс. " << t1 << ")" << std::endl;
        else {
            std::pair<int, int> t2 = is_mp();
            if (t2.first)
                std::cout << "(M.P. " << t2.first << ", " << t2.second << ")" << std::endl;
            else
                std::cout << "(Не доказано)" << std::endl;
        }
    }
}

int main() {
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    double tin = std::time(NULL);
    std::string h;
    getline(std::cin, h);
    h = my_split(h);
    parse_head(h);
    create_axioms();
    while (getline(std::cin, str)) {
        tmp = str;
        check();
        proofs.push_back(tmp);
    }
    double tout = std::time(NULL);
    std::cout << "Время работы: " << tout - tin << std::endl;
    return 0;
}
